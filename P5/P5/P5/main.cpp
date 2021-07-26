#include "Entity.h"
#include "Util.h"
#define STB_IMAGE_IMPLEMENTATION

#define FIXEDTIMESTEP 0.016666666
#define FIXEDANIMATIONSTEP 0.15
#define ENEMYCOUNT 1

using namespace glm;
using namespace std;

struct GameState;


void Initialize(ShaderProgram* program, GameState& state, GLuint& fontTex, GLuint& HPTex);
void ProcessInput(GameState& state, bool& gameIsRunning, bool& gameWon, bool& gameLost);
void Update(ShaderProgram* program,float& lastTick, float& deltaTime, float& animTime, GameState& state, bool& gameWon, bool& gameLost);
void Render(SDL_Window* displayWindow, GameState& state, ShaderProgram* program, GLuint fontTex, GLuint HPTex, bool& gameWon, bool& gameLost);
void Shutdown(GameState& state);
void AnimUpdate(Entity* entity, GameState& state, float& animTime);
void AnimRender(Entity* entity, ShaderProgram* program);


// main game loop
int main(int argc, const char * argv[]) {
    ShaderProgram program;
    SDL_Window* displayWindow;
    GLuint fontTex, HPTex;
    GameState state;
    bool gameIsRunning = true;
    bool gameWon = false;
    bool gameLost = false;
    
    SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Monster Slayer2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    float lastTick = 0;
    float deltaTime = 0;
    float animTime = 0;
    
    Initialize(&program, state, fontTex, HPTex);
    while (gameIsRunning){
        ProcessInput(state, gameIsRunning, gameWon, gameLost);
        Update(&program, lastTick, deltaTime, animTime, state, gameWon, gameLost);
        Render(displayWindow, state, &program, fontTex, HPTex, gameWon, gameLost);
    }
    Shutdown(state);
    
    return 0;
}


// Initializing all Entities
void Initialize(ShaderProgram* program, GameState& state, GLuint& fontTex, GLuint& HPTex) {
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
    program->Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    
//    state.music = Mix_LoadMUS("theme.mp3");
//    Mix_PlayMusic(state.music, -1);
    
    mat4 viewMatrix = translate(mat4(1.0f),vec3(-5.0f, 3.5f, 0));
    mat4 projectionMatrix = ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program->SetProjectionMatrix(projectionMatrix);
    program->SetViewMatrix(viewMatrix);
    
    glUseProgram(program->programID);
    
    glEnable(GL_BLEND);
    
    fontTex = Util::LoadTexture("font.png");
    HPTex = Util::LoadTexture("heart.png");
    
    //Background
    
    state.Background = new Entity(BACKGROUND, Util::LoadTexture("background.png"), 1, 1, 1.0f, 1.5f);
    state.Background->xRepeat = 2;
    state.Background->position = vec3(7.5f*1.5f, -3.5f, 0);
    state.Background->height = 7.5f;
    state.Background->width = 7.5f;
    
    //Player
    state.Player = new Entity(PLAYER,Util::LoadTexture("adventurer.png"), 10, 11, 0.74f, 1);
    state.Player->life = 3;
    state.Player->acceleration.y = -8.0f;
    state.Player->position = vec3(2.0f, 2.0f, 0);
    state.Player->speed = 2.0f;
    state.Player->idleTex = {65,66,71,72};
    state.Player->moveTex = {84,85,86,87,88,89};
    state.Player->jumpTex = {77,78,79};
    state.Player->doubleTex = {78,79};
    state.Player->deathTex = {53,54,55,56,57,58,59};
    state.Player->fallTex = {60,61};
    state.Player->height = 1.3f;
    state.Player->width = 1.3f;
    state.Player->contactHeight = 0.85f;
    state.Player->contactWidth = 0.3f;
    state.Player->jumping = Mix_LoadWAV("heroJump.wav");
    state.Player->running = Mix_LoadWAV("heroRun.wav");
//    state.Player->hurt = Mix_LoadWAV("playerHurt.wav");
//    Mix_VolumeChunk(state.Player->jumping, MIX_MAX_VOLUME / 4);
    Mix_VolumeChunk(state.Player->running, 0);
    Mix_PlayChannel(-1, state.Player->running, -1);
//

    
    
    // Enemy
    state.Enemy = new Entity();
    for (int i =0; i< ENEMYCOUNT; i++){
        state.Enemy[i].selftype = ENEMY;
        state.Enemy[i].texture = Util::LoadTexture("LightBandit.png");
        state.Enemy[i].acceleration.y = -8.0f;
        state.Enemy[i].speed = 1.2f;
        state.Enemy[i].cols = 8;
        state.Enemy[i].rows = 5;
        state.Enemy[i].idleTex = {7,6,5,4};
        state.Enemy[i].moveTex = {15,14,13,12,11,10,9,8};
        state.Enemy[i].deathTex = {36,35};
        state.Enemy[i].height = 1.1f;
        state.Enemy[i].width = 1.1f;
        state.Enemy[i].contactHeight = 0.8f;
        state.Enemy[i].contactWidth = 0.4f;
    }
    
    state.Enemy[0].position = vec3(9.0f, 0, 0);
    
    
    static unsigned int levelData[] =
        {
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45, 45, 45, 45, 45 ,45 ,45 ,45, 45, 45,
            13, 27,  1,  1,  1,  1,  1,  1,  1,  1,  2, 45, 45, 45,  0,  1,  1,  1,  1,  1,  1,  2, 45, 45, 45,  0,  1,  1,  1,  2,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 14,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 14,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 14,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 14
        };
    
    state.map = new Map(30, 15, levelData ,Util::LoadTexture("tilesheet.png"), 0.5f, 12, 6);
    vector<int> spawnPoints = {8,19,27};
    state.map->setSpawn(spawnPoints);


    
    //For transparency of the image background
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void ProcessInput(GameState& state, bool& gameIsRunning, bool& gameWon, bool& gameLost) {
    SDL_Event event;
    
    
    // Start/ Restart/ Quit inputs
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                        
                    // jumping
                    case SDLK_k:
                        if (!gameWon && !gameLost && (!state.Player->jump || !state.Player->doubleJump)){
                            Mix_PlayChannel(-1, state.Player->jumping, 0);
                            if (state.Player->jump){
                                state.Player->doubleJump = true;
                            }
                            state.Player->fall = false;
                            state.Player->jump = true;
                            state.Player->velocity.y = 4.5f;
                        }
                        break;
                    
                        
                    // restart
                    case SDLK_r:
                        if (gameWon || gameLost){
                            gameWon = false;
                            gameLost = false;
                            for (int i =0; i < ENEMYCOUNT; i++){
                                state.Enemy[i].life = 3;
                                state.Enemy[i].dead = false;
                                state.Enemy[i].deathcount = 0;
                            }
                            state.Player->life = 3;
                            state.Player->dead = false;
                            state.Player->deathcount = 0;
                            state.Player->position = vec3(-4.0f, -2.5f, 0);
                        }
                        break;
                }
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (!gameWon && !gameLost){
        if (keys[SDL_SCANCODE_A]){
            state.Player->movement.x = -1;
        }   else if (keys[SDL_SCANCODE_D]){
            state.Player->movement.x = 1;
        }   else{
            state.Player->movement.x = 0;
        }
    }
}



void Update(ShaderProgram* program, float& lastTick, float& deltaTime, float& animTime, GameState& state, bool& gameWon, bool& gameLost) {
    
    // Calculating deltaTime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    deltaTime += ticks - lastTick;
    animTime += ticks - lastTick;
    lastTick = ticks;
    
    
    if (!gameWon && !gameLost){
        while(deltaTime >= FIXEDTIMESTEP){
            
            if (!state.Player->active){
                gameLost = true;
            }
            
            // General Updates
            mat4 viewMatrix;
            if (state.Player->position.x > 5.0f){
                viewMatrix = translate(mat4(1.0f),vec3(-state.Player->position.x, 3.5f, 0));
                state.Background->position = vec3(7.5*1.5 - 5.0f/3 + state.Player->position.x / 3, -3.5f, 0);
            }   else{
                viewMatrix = translate(mat4(1.0f),vec3(-5.0f, 3.5f, 0));
            }
            program->SetViewMatrix(viewMatrix);
            
            state.Player->update(state.map ,state.Enemy, ENEMYCOUNT, FIXEDTIMESTEP);
            
            state.Background->update(state.map, NULL, 0, FIXEDTIMESTEP);
            
            for (int i = 0; i < ENEMYCOUNT; i++){
                if (state.Enemy[i].active){
                    state.Enemy[i].update(state.map, NULL, 0, FIXEDTIMESTEP);
                }
            }
            //cout << state.Enemy[0].active << "\n";
            
            
            // Updating status of enemies and player running sound effect
                // Running sound effect
            if(state.Player->onGround){
                if(state.Player->movement.x != 0){
                    Mix_VolumeChunk(state.Player->running, MIX_MAX_VOLUME);
                }   else{
                    Mix_VolumeChunk(state.Player->running, 0);
                }
            }   else{
                Mix_VolumeChunk(state.Player->running, 0);
            }
//
//
//            if (gameWon || gameLost){
//                Mix_VolumeChunk(state.Player->running, 0);
//            }
//
//                // Bandit
//            if (state.Enemy[0].deathcount != state.Enemy[0].deathTex.size()){
//
//                if  (distance(state.Player->position, state.Enemy[0].position) < 0.6f && !state.Enemy[0].attack){
//                    if (state.Player->position.x < state.Enemy[0].position.x){
//                        state.Enemy[0].facingRight = false;
//                    }   else{
//                        state.Enemy[0].facingRight = true;
//                    }
//                    state.Enemy[0].attack = true;
//                }   else if (distance(state.Player->position, state.Enemy[0].position) < 2.5f
//                             || fabs(state.Player->position.y - state.Enemy[0].position.y) < 0.3f){
//                    if (abs(state.Player->position.x - state.Enemy[0].position.x) > 0.15f){
//                        if (state.Player->position.x < state.Enemy[0].position.x){
//                            state.Enemy[0].movement.x = -1.0f;
//                        }   else{
//                            state.Enemy[0].movement.x = 1.0f;
//                        }
//                    }
//                }   else{
//                    state.Enemy[0].movement.x = 0;
//                }
//
//            }

            
            
            // Updating animation
            while (animTime >= FIXEDANIMATIONSTEP){
                
                for (int i= 0; i < ENEMYCOUNT; i++){
                    if (state.Enemy[i].deathcount != state.Enemy[i].deathTex.size()){
                        Util::AnimUpdate(&state.Enemy[i], animTime);
                    }
                }
                
                Util::AnimUpdate(state.Player, animTime);
                
                animTime -= FIXEDANIMATIONSTEP;
            }

            
            deltaTime -= FIXEDTIMESTEP;
        }
    }
}


void Render(SDL_Window* displayWindow, GameState& state, ShaderProgram* program, GLuint fontTex, GLuint HPTex, bool& gameWon, bool& gameLost) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    Util::CustomRender(program, state.Background->yRepeat, state.Background->xRepeat, state.Background->height, state.Background->width, state.Background, 1, 1, 1);
    
    state.map->Render(program);
    
    for (int i = 0; i < ENEMYCOUNT; i++){
        if (state.Enemy[i].active){
            Util::AnimRender(&state.Enemy[i], program);
        }
    }

    Util::AnimRender(state.Player, program);
    
    vec3 position;
    
    if (state.Player->position.x >5.0f){
        position = vec3(state.Player->position.x - 3.8f, -0.7f , 0);
    }   else{
        position = vec3(1.2f, -0.7f, 0);
    }
    
    Util::DisplayHealth(program, state, HPTex, position);
    
    if(gameWon){
        Util::DrawText(program, fontTex, "You Won!", 1, -.5, vec3(state.Player->position.x-1.7f,-3.0f,0));
        Util::DrawText(program, fontTex, "Press R to restart", .3, -.1, vec3(state.Player->position.x-1.7f,-3.65f,0));
    }

    if(gameLost){
        Util::DrawText(program, fontTex, "Game Over", 1, -.5, vec3(state.Player->position.x-2.0f,-3.0f,0));
        Util::DrawText(program, fontTex, "Press R to restart", .3, -.1, vec3(state.Player->position.x-1.7f,-3.65f,0));
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


// Shuting down
void Shutdown(GameState& state) {
//    Mix_FreeChunk(state.Player->jumping);
//    Mix_FreeChunk(state.Player->running);
//    Mix_FreeMusic(state.music);
    Mix_CloseAudio();
    SDL_Quit();
}


