#include "Entity.h"
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#define FIXEDTIMESTEP 0.016666666
#define FIXEDANIMATIONSTEP 0.15
#define PLATFORMCOUNT 7
#define ENEMYCOUNT 3
#define MAXBULLETS 3

using namespace glm;
using namespace std;

struct GameState{
    Mix_Music* music;
    Entity* Background;
    Entity* Player;
    Entity* Platform;
    Entity* Enemy;
    Entity* herobullets;
    Entity* enemybullets;
};


void Initialize(ShaderProgram* program, GameState& state, GLuint& fontTex);
void ProcessInput(GameState& state, bool& gameIsRunning, bool& gameWon, bool& gameLost);
void Update(float& lastTick, float& deltaTime, float& animTime, GameState& state, bool& gameWon, bool& gameLost);
void Render(SDL_Window* displayWindow, GameState& state, ShaderProgram* program, GLuint fontTex, bool& gameWon, bool& gameLost);
void Shutdown(GameState& state);
GLuint LoadTexture(const char* filepath);
GLuint LoadTextureHighRes(const char* filepath);
void CustomRender(ShaderProgram* program, float yRepeat, float xRepeat, float height, float width, Entity* object, int index, int cols, int rows, bool xFlip = false, bool yFlip = false);
void DrawText(ShaderProgram* program, GLuint fontTextureID, string text, float size, float spacing, vec3 position);
void AnimUpdate(Entity* entity, GameState& state, float& animTime);
void AnimRender(Entity* entity, ShaderProgram* program);


// main game loop
int main(int argc, const char * argv[]) {
    ShaderProgram program;
    SDL_Window* displayWindow;
    GLuint fontTex;
    GameState state;
    bool gameIsRunning = true;
    bool gameWon = false;
    bool gameLost = false;
    
    SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Monster Slayer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    float lastTick = 0;
    float deltaTime = 0;
    float animTime = 0;
    
    Initialize(&program, state, fontTex);
    while (gameIsRunning){
        ProcessInput(state, gameIsRunning, gameWon, gameLost);
        Update(lastTick, deltaTime, animTime, state, gameWon, gameLost);
        Render(displayWindow, state, &program, fontTex, gameWon, gameLost);
    }
    Shutdown(state);
    
    return 0;
}


// Initializing all Entities
void Initialize(ShaderProgram* program, GameState& state, GLuint& fontTex) {
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program->Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    
    state.music = Mix_LoadMUS("theme.mp3");
    Mix_PlayMusic(state.music, -1);
    
    mat4 viewMatrix = mat4(1.0f);
    mat4 projectionMatrix = ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program->SetProjectionMatrix(projectionMatrix);
    program->SetViewMatrix(viewMatrix);
    
    glUseProgram(program->programID);
    
    glEnable(GL_BLEND);
    
    fontTex = LoadTexture("font.png");
    
    //Background
    state.Background = new Entity(BACKGROUND, LoadTexture("background.png"), 1, 1, 0.8f);
    state.Background->height = 10.0f;
    state.Background->width = 10.0f;
    state.Background->position = vec3(0, 0.5f, 0);
    // Updating background
    state.Background->update(NULL, NULL, 0, 0, FIXEDTIMESTEP);
    
    
    //Player
    state.Player = new Entity(PLAYER,LoadTexture("adventurer.png"), 10, 11, 0.74f, 1);
    state.Player->acceleration.y = -8.0f;
    state.Player->position = vec3(-4.0f, -2.5f, 0);
    state.Player->speed = 2.0f;
    state.Player->idleTex = {65,66,71,72};
    state.Player->moveTex = {84,85,86,87,88,89};
    state.Player->jumpTex = {77,78,79};
    state.Player->hitTex = {62,63,64};
    state.Player->deathTex = {53,54,55,56,57,58,59};
    state.Player->castTex = {30,31,32,33};
    state.Player->attackTex = {13,14,15,16,17};
    state.Player->airAttackTex = {10,11,12};
    state.Player->wallSlideTex = {107,108};
    state.Player->fallTex = {60,61};
    state.Player->validattack = 2;
    state.Player->validcast = 3;
    state.Player->height = 1.2;
    state.Player->width = 1.2;
    state.Player->contactHeight = 0.8f;
    state.Player->contactWidth = 0.3f;
    state.Player->attackdisx = 2.5f;
    state.Player->attackdisy = 1.3f;
    state.Player->jumping = Mix_LoadWAV("heroJump.wav");
    state.Player->running = Mix_LoadWAV("heroRun.wav");
    state.Player->attacking = Mix_LoadWAV("heroAttack.wav");
    state.Player->casting = Mix_LoadWAV("playerShot.wav");
    state.Player->hurt = Mix_LoadWAV("playerHurt.wav");
    Mix_VolumeChunk(state.Player->running, 0);
    Mix_PlayChannel(-1, state.Player->running, -1);
    
    //Player bullets
    GLuint heroBulletTex = LoadTexture("playerShot.png");
    state.herobullets = new Entity[MAXBULLETS];
    for (int i =0; i< MAXBULLETS; i++){
        state.herobullets[i].selftype = HEROBULLET;
        state.herobullets[i].texture = heroBulletTex;
        state.herobullets[i].dead = true;
        state.herobullets[i].speed = 5.0f;
        state.herobullets[i].height = 0.33f;
        state.herobullets[i].width = 0.33f;
        state.herobullets[i].contactHeight = 0.2f;
        state.herobullets[i].contactWidth = 0.2f;
        state.herobullets[i].spritewidth = 1.6f;
    }
    
    
    //Enemy bullets
    GLuint enemyBulletTex = LoadTexture("enemyShot.png");
    state.enemybullets = new Entity[MAXBULLETS];
    for (int i =0; i< MAXBULLETS; i++){
        state.enemybullets[i].selftype = ENEMYBULLET;
        state.enemybullets[i].texture = enemyBulletTex;
        state.enemybullets[i].dead = true;
        state.enemybullets[i].speed = 4.3f;
        state.enemybullets[i].height = 0.33f;
        state.enemybullets[i].width = 0.33f;
        state.enemybullets[i].contactHeight = 0.2f;
        state.enemybullets[i].contactWidth = 0.2f;
        state.enemybullets[i].spritewidth = 1.6f;
    }
    

    // Platforms
    state.Platform = new Entity[PLATFORMCOUNT];
    for (int i = 0; i < PLATFORMCOUNT; i++){
        state.Platform[i].selftype = PLATFORM;
    }
    
    GLuint wall = LoadTexture("wall.png");
    state.Platform[2].selftype = BORDER;
    state.Platform[2].texture = wall;
    state.Platform[2].spritewidth = 0.4f;
    state.Platform[2].spriteheight = 0.32f;
    state.Platform[2].height = 1.5f;
    state.Platform[2].width = 1.5f;
    state.Platform[2].yRepeat = 15;
    state.Platform[2].position = vec3(-5.0f, 0.25f, 0);
    
    
    state.Platform[3].selftype = BORDER;
    state.Platform[3].texture = wall;
    state.Platform[3].spritewidth = 0.4f;
    state.Platform[3].spriteheight = 0.32f;
    state.Platform[3].height = 1.5f;
    state.Platform[3].width = 1.5f;
    state.Platform[3].yRepeat = 15;
    state.Platform[3].position = vec3(5.0f, 0.25f, 0);
    
    
    GLuint ground = LoadTexture("ground.png");
    
    state.Platform[4].selftype = BORDER;
    state.Platform[4].texture = ground;
    state.Platform[4].spritewidth = 1.091f;
    state.Platform[4].spriteheight = 0.228f;
    state.Platform[4].xRepeat = 5;
    state.Platform[4].height = 3.0f;
    state.Platform[4].width = 3.0f;
    state.Platform[4].position = vec3(0, -3.5f, 0);
    
    state.Platform[5].texture = ground;
    state.Platform[5].spritewidth = 1.091f;
    state.Platform[5].spriteheight = 0.228f;
    state.Platform[5].height = 3.0f;
    state.Platform[5].width = 3.0f;
    state.Platform[5].position = vec3(0, -1.5f, 0);
    
    state.Platform[0].texture = ground;
    state.Platform[0].spritewidth = 1.091f;
    state.Platform[0].spriteheight = 0.228f;
    state.Platform[0].height = 3.0f;
    state.Platform[0].width = 3.0f;
    state.Platform[0].position = vec3(-4.0f, 0., 0);
    
    state.Platform[1].texture = ground;
    state.Platform[1].spritewidth = 1.091f;
    state.Platform[1].spriteheight = 0.228f;
    state.Platform[1].height = 3.0f;
    state.Platform[1].width = 3.0f;
    state.Platform[1].position = vec3(4.0f, 0, 0);
    
    state.Platform[6].texture = ground;
    state.Platform[6].spritewidth = 1.091f;
    state.Platform[6].spriteheight = 0.228f;
    state.Platform[6].height = 3.0f;
    state.Platform[6].width = 3.0f;
    state.Platform[6].position = vec3(0, 1.5f, 0);
    
    
    //Enemies
    state.Enemy = new Entity[ENEMYCOUNT];
    
    for (int i=0; i<ENEMYCOUNT; i++){
        state.Enemy[i].hurt = Mix_LoadWAV("hit.wav");
    }
    
        //Bandit
    state.Enemy[0].texture = LoadTexture("LightBandit.png");
    state.Enemy[0].selftype = ENEMY;
    state.Enemy[0].acceleration.y = -8.0f;
    state.Enemy[0].speed = 1.2f;
    state.Enemy[0].position = vec3(4.0f, -2.5f, 0);
    state.Enemy[0].cols = 8;
    state.Enemy[0].rows = 5;
    state.Enemy[0].idleTex = {7,6,5,4};
    state.Enemy[0].attackTex = {23,22,21,20,19,18,17,16};
    state.Enemy[0].moveTex = {15,14,13,12,11,10,9,8};
    state.Enemy[0].hitTex = {39,38,37};
    state.Enemy[0].deathTex = {36,35};
    state.Enemy[0].validattack = 4;
    state.Enemy[0].height = 1.19f;
    state.Enemy[0].width = 1.19f;
    state.Enemy[0].contactHeight = 0.8f;
    state.Enemy[0].contactWidth = 0.4f;
    state.Enemy[0].attackdisx = 1.9f;
    state.Enemy[0].attacking = Mix_LoadWAV("banditAttack.wav");
    
        //Flying eye
    
    state.Enemy[1].texture = LoadTexture("flyingeye.png");
    state.Enemy[1].selftype = FLYENEMY;
    state.Enemy[1].speed = 1.0f;
    state.Enemy[1].position = vec3(-3.0f, 6.0f, 0);
    state.Enemy[1].cols = 12;
    state.Enemy[1].rows = 2;
    state.Enemy[1].idleTex = {12};
    state.Enemy[1].attackTex = {0,1,2,3,4,5,6,7};
    state.Enemy[1].moveTex = {12,13,14,15,16,17,18,19};
    state.Enemy[1].hitTex = {20,21,22,23};
    state.Enemy[1].deathTex = {8,9,10,11};
    state.Enemy[1].validattack = 6;
    state.Enemy[1].height = 3.0f;
    state.Enemy[1].width = 3.0f;
    state.Enemy[1].contactHeight = 0.189f;
    state.Enemy[1].contactWidth = 0.19f;
    state.Enemy[1].attacking = Mix_LoadWAV("flyAttack.wav");
    
        // Mage
    
    state.Enemy[2].texture = LoadTexture("MAGE.png");
    state.Enemy[2].selftype = ENEMY;
    state.Enemy[2].position = vec3(0, 2.5f, 0);
    state.Enemy[2].speed = 0.9f;
    state.Enemy[2].acceleration.y = -8.0f;
    state.Enemy[2].cols = 8;
    state.Enemy[2].rows = 9;
    state.Enemy[2].contactWidth = 0.15f;
    state.Enemy[2].contactHeight = 0.9f;
    state.Enemy[2].hitTex = {64,65};
    state.Enemy[2].idleTex = {8};
    state.Enemy[2].moveTex = {16,17,18,19,20,21,22};
    state.Enemy[2].attackTex = {24,25,26,27,28,29,30,31};
    state.Enemy[2].castTex = {56,57,58,59};
    state.Enemy[2].deathTex = {66,67,68,69,70,71};
    state.Enemy[2].validattack = 3;
    state.Enemy[2].validcast = 3;
    state.Enemy[2].attackdisx = 2;
    state.Enemy[2].height = 1.3f;
    state.Enemy[2].width = 1.3f;
    state.Enemy[2].casting = Mix_LoadWAV("mageShot.wav");
    state.Enemy[2].attacking = Mix_LoadWAV("mageSwing.wav");
    
    Mix_VolumeMusic(MIX_MAX_VOLUME/10);
    Mix_Volume(-1, MIX_MAX_VOLUME/10);
    
    Mix_VolumeChunk(state.Player->casting, MIX_MAX_VOLUME);
    Mix_VolumeChunk(state.Enemy[2].attacking, MIX_MAX_VOLUME);
    Mix_VolumeChunk(state.Enemy[1].attacking, MIX_MAX_VOLUME);
    
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
                        
                    case SDLK_u:
                        if (!gameWon && !gameLost && !state.Player->attack && !state.Player->airAttack && !state.Player->cast){
                            state.Player->cast = true;
                        }
                    
                    // attacking
                    case SDLK_j:
                        if (!gameWon && !gameLost && !state.Player->attack && !state.Player->airAttack && !state.Player->cast){
                            if (state.Player->onGround){
                                state.Player->attack = true;
                            }   else{
                                state.Player->airAttack = true;
                            }
                        }
                        break;
                        
                    // jumping
                    case SDLK_k:
                        if (!gameWon && !gameLost && !state.Player->attack && !state.Player->cast && (!state.Player->jump ||
                                                                               (state.Player->wallSlide && state.Player->velocity.y < 1.0f))){
                            Mix_PlayChannel(-1, state.Player->jumping, 0);
                            state.Player->fall = false;
                            state.Player->jump = true;
                            state.Player->velocity.y = 4.5f;
                            state.Player->wallSlide = false;
                        }
                        break;
                    
                        
                    //
                    case SDLK_r:
                        if (gameWon || gameLost){
                            gameWon = false;
                            gameLost = false;
                            for (int i =0; i < ENEMYCOUNT; i++){
                                state.Enemy[i].life = 3;
                                state.Enemy[i].dead = false;
                                state.Enemy[i].deathcount = 0;
                            }
                            state.Enemy[0].position = vec3(4.0f, -2.5f, 0);
                            state.Enemy[1].position = vec3(-3.0f, 6.0f, 0);
                            state.Enemy[2].position = vec3(0, 2.5f, 0);
                            state.Player->life = 3;
                            state.Player->dead = false;
                            state.Player->deathcount = 0;
                            state.Player->position = vec3(-4.0f, -2.5f, 0);
                        }
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



void Update(float& lastTick, float& deltaTime, float& animTime, GameState& state, bool& gameWon, bool& gameLost) {
    
    // Calculating deltaTime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    deltaTime += ticks - lastTick;
    animTime += ticks - lastTick;
    lastTick = ticks;
    
    
    if (!gameWon && !gameLost){
        while(deltaTime >= FIXEDTIMESTEP){
            
            // Checking whether game is won or lost
            if (state.Player->deathcount == state.Player->deathTex.size() - 1){
                gameLost = true;
            }
            
            
            bool won = true;
            for (int i = 0; i < ENEMYCOUNT; i++){
                if (state.Enemy[i].deathcount != state.Enemy[i].deathTex.size()){
                    won = false;
                    break;
                }
            }
            if (won){
                gameWon = true;
            }
            
                    
            // General Updates
            for (int i = 0; i < ENEMYCOUNT; i++){
                if (state.Enemy[i].deathcount != state.Enemy[i].deathTex.size()){
                    state.Enemy[i].update(state.Platform, state.Player, PLATFORMCOUNT, 1, FIXEDTIMESTEP);
                }
            }
            
            state.Player->update(state.Platform, state.Enemy, PLATFORMCOUNT, ENEMYCOUNT, FIXEDTIMESTEP);
        
            for (int i = 0; i < PLATFORMCOUNT; i++){
                state.Platform[i].update(NULL, NULL, 0, 0, FIXEDTIMESTEP);
            }
            
            
            // Updating status of enemies and player running sound effect
                // Running sound effect
            if(state.Player->onGround){
                if(state.Player->movement.x != 0){
                    Mix_VolumeChunk(state.Player->running, MIX_MAX_VOLUME / 10);
                }   else{
                    Mix_VolumeChunk(state.Player->running, 0);
                }
            }   else{
                Mix_VolumeChunk(state.Player->running, 0);
            }
            
            
            if (gameWon || gameLost){
                Mix_VolumeChunk(state.Player->running, 0);
            }
            
                // Bandit
            if (state.Enemy[0].deathcount != state.Enemy[0].deathTex.size()){
                
                if  (distance(state.Player->position, state.Enemy[0].position) < 0.6f && !state.Enemy[0].attack){
                    if (state.Player->position.x < state.Enemy[0].position.x){
                        state.Enemy[0].facingRight = false;
                    }   else{
                        state.Enemy[0].facingRight = true;
                    }
                    state.Enemy[0].attack = true;
                }   else if (distance(state.Player->position, state.Enemy[0].position) < 2.5f
                             || fabs(state.Player->position.y - state.Enemy[0].position.y) < 0.3f){
                    if (abs(state.Player->position.x - state.Enemy[0].position.x) > 0.15f){
                        if (state.Player->position.x < state.Enemy[0].position.x){
                            state.Enemy[0].movement.x = -1.0f;
                        }   else{
                            state.Enemy[0].movement.x = 1.0f;
                        }
                    }
                }   else{
                    state.Enemy[0].movement.x = 0;
                }
                
            }
            
                // Flying eye
            if (state.Enemy[1].deathcount != state.Enemy[1].deathTex.size()){
                
                if  (distance(state.Player->position, state.Enemy[1].position) < 0.6f && !state.Enemy[1].attack){
                    if (state.Player->position.x < state.Enemy[1].position.x){
                        state.Enemy[1].facingRight = false;
                    }   else {
                        state.Enemy[1].facingRight = true;
                    }
                    state.Enemy[1].attack = true;
                }
                
                float checkxplayer = fabs(state.Enemy[1].position.x - state.Player->position.x) - (state.Enemy[1].contactWidth * state.Enemy[1].width* state.Enemy[1].spritewidth + state.Player->contactWidth* state.Player->width* state.Player->spritewidth) / 2;
                
                if (state.Player->position.x > state.Enemy[1].position.x ){
                    if (!state.Enemy[1].attack){
                        state.Enemy[1].facingRight = true;
                    }
                    
                    if (checkxplayer <= 0){
                        state.Enemy[1].movement.x = -1.0f;
                    }   else if (abs(state.Player->position.x - state.Enemy[1].position.x) > 1.0f){
                        state.Enemy[1].movement.x = 1.0f;
                    }
                }   else{
                    if (!state.Enemy[1].attack){
                        state.Enemy[1].facingRight = false;
                    }
                    
                    if (checkxplayer <= 0){
                        state.Enemy[1].movement.x = 1.0f;
                    }   else if (abs(state.Player->position.x - state.Enemy[1].position.x) > 1.0f){
                        state.Enemy[1].movement.x = -1.0f;
                    }
                }
                
                if (state.Enemy[1].position.y < state.Player->position.y + state.Player->height* state.Player->spriteheight* state.Player->contactHeight / 2){
                    state.Enemy[1].movement.y = 1.0f;
                }   else if (abs(state.Player->position.y - state.Enemy[1].position.y) > 0.6f){
                    state.Enemy[1].movement.y = -1.0f;
                }
            
            }
            
                //Mage
            
            if (state.Enemy[2].deathcount != state.Enemy[2].deathTex.size()){
                
                if  (distance(state.Player->position, state.Enemy[2].position) < 1.3f && !state.Enemy[2].attack && !state.Enemy[2].cast){
                    
                    if (state.Player->position.x < state.Enemy[2].position.x){
                        state.Enemy[2].facingRight = false;
                    }   else{
                        state.Enemy[2].facingRight = true;
                    }
                    
                    if (state.Player->position.x < state.Enemy[2].position.x){
                        state.Enemy[2].movement.x = 1.0f;
                    }   else{
                        state.Enemy[2].movement.x = -1.0f;
                    }
                    
                    if (state.Enemy[2].lastcollideleft != NONE || state.Enemy[2].lastcollideright != NONE){
                        
                        state.Enemy[2].movement.x *= -1;
                        
                    }
                    
                    if (abs(state.Player->position.x - state.Enemy[2].position.x) < 0.6f){
                        
                        state.Enemy[2].attack = true;
                        
                    }
                    
                }   else{
                    
                    state.Enemy[2].castCooldown += FIXEDTIMESTEP;
                    
                    bool castable = false;
                    if (state.Enemy[2].castCooldown >= 2.5f){
                        
                        for (int i =0; i<MAXBULLETS; i++){
                            if (state.enemybullets[i].dead){
                                castable = true;
                            }
                        }
                        
                        if (castable && !state.Enemy[2].cast && !state.Enemy[2].attack){
                            state.Enemy[2].cast = true;
                            state.Enemy[2].castCooldown = 0;
                            if (state.Player->position.x < state.Enemy[0].position.x){
                                state.Enemy[2].facingRight = false;
                            }   else{
                                state.Enemy[2].facingRight = true;
                            }
                        }
                    }
                }
            }
            
            
            // Updating animation
            while (animTime >= FIXEDANIMATIONSTEP){
                
                for (int i= 0; i < ENEMYCOUNT; i++){
                    if (state.Enemy[i].deathcount != state.Enemy[i].deathTex.size()){
                        AnimUpdate(&state.Enemy[i], state, animTime);
                    }
                }
                
                AnimUpdate(state.Player, state, animTime);
                
                animTime -= FIXEDANIMATIONSTEP;
            }
            
            
            // Updating bullets
            for (int i = 0; i < MAXBULLETS; i++){
                if (!state.herobullets[i].dead){
                    state.herobullets[i].update(state.Platform, state.Enemy, PLATFORMCOUNT, ENEMYCOUNT, FIXEDTIMESTEP);
                    if (state.herobullets[i].position.y >= 5.0f){
                        state.herobullets[i].dead = true;
                    }
                }
            }
            
            for (int i = 0; i < MAXBULLETS; i++){
                if (!state.enemybullets[i].dead){
                    state.enemybullets[i].update(state.Platform, state.Player, PLATFORMCOUNT, 1, FIXEDTIMESTEP);
                    if (state.enemybullets[i].position.y >= 5.0f){
                        state.enemybullets[i].dead = true;
                    }
                }
            }
            
            deltaTime -= FIXEDTIMESTEP;
        }
    }
}


void Render(SDL_Window* displayWindow, GameState& state, ShaderProgram* program, GLuint fontTex, bool& gameWon, bool& gameLost) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Rendering background
    CustomRender(program, state.Background->yRepeat, state.Background->xRepeat, state.Background->height, state.Background->width, state.Background, 0, state.Background->cols, state.Background->rows);
    
    // Rendering platfomrs
    for (int i = 0; i < PLATFORMCOUNT; i++){
        CustomRender(program, state.Platform[i].yRepeat, state.Platform[i].xRepeat, state.Platform[i].height, state.Platform[i].width, &state.Platform[i], 0, state.Platform[i].cols, state.Platform[i].rows);
    }
    
    for (int i = 0; i < ENEMYCOUNT; i++){
        if (state.Enemy[i].deathcount != state.Enemy[i].deathTex.size()){
            AnimRender(&state.Enemy[i], program);
        }
    }
    
    AnimRender(state.Player, program);
    
    for (int i = 0; i < MAXBULLETS; i++){
        if (!state.herobullets[i].dead){
            if (state.herobullets[i].facingRight){
                CustomRender(program, state.herobullets[i].yRepeat, state.herobullets[i].xRepeat, state.herobullets[i].height, state.herobullets[i].width, &state.herobullets[i], 0, state.herobullets[i].cols, state.herobullets[i].rows);
            }   else{
                CustomRender(program, state.herobullets[i].yRepeat, state.herobullets[i].xRepeat, state.herobullets[i].height, state.herobullets[i].width, &state.herobullets[i], 0, state.herobullets[i].cols, state.herobullets[i].rows, true);
            }
        }
    }
    
    for (int i = 0; i < MAXBULLETS; i++){
        if (!state.enemybullets[i].dead){
            if (state.enemybullets[i].facingRight){
                CustomRender(program, state.enemybullets[i].yRepeat, state.enemybullets[i].xRepeat, state.enemybullets[i].height, state.enemybullets[i].width, &state.enemybullets[i], 0, state.enemybullets[i].cols, state.enemybullets[i].rows);
            }   else{
                CustomRender(program, state.enemybullets[i].yRepeat, state.enemybullets[i].xRepeat, state.enemybullets[i].height, state.enemybullets[i].width, &state.enemybullets[i], 0, state.enemybullets[i].cols, state.enemybullets[i].rows, true);
            }
        }
    }
    
    if(gameWon){
        DrawText(program, fontTex, "You Won!", 1, -.5, vec3(-1.7f,0.5f,0));
        DrawText(program, fontTex, "Press R to restart", .3, -.1, vec3(-1.7f,-.15f,0));
    }

    if(gameLost){
        DrawText(program, fontTex, "Game Over", 1, -.5, vec3(-2.0f,0.5f,0));
        DrawText(program, fontTex, "Press R to restart", .3, -.1, vec3(-1.7f,-0.15f,0));
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


// Shuting down
void Shutdown(GameState& state) {
    Mix_FreeChunk(state.Player->jumping);
    Mix_FreeChunk(state.Player->running);
    Mix_FreeMusic(state.music);
    Mix_CloseAudio();
    SDL_Quit();
}



// Additional Functions
    // Updating Player
void AnimUpdate(Entity* entity, GameState& state,float& animTime){
    
    if (entity->dead){
        
        if (entity->deathcount < entity->deathTex.size()){
            if (entity->selftype == PLAYER){
                if (entity->deathcount < entity->deathTex.size()-1){
                    entity->deathcount += 1;
                }
            }   else if (entity->selftype == FLYENEMY){
                if (!entity->onGround){
                    if (entity->deathcount < 1){
                        entity->deathcount += 1;
                    }
                }   else{
                    entity->deathcount += 1;
                }
            }   else{
                entity->deathcount += 1;
            }
        }
    
    }   else if (entity->cast){
        
        if (entity->castcount == 0){
            Mix_PlayChannel(-1, entity->casting, 0);
        }
        
        if (entity->castcount < entity->castTex.size()-1){
            
            if (!entity->onGround && entity->castcount < entity->validcast - 1 && entity->selftype == PLAYER){
                entity->castcount = entity->validcast - 1;
            }   else{
                entity->castcount += 1;
            }
            
            if (entity->castcount == entity->validcast){
                if (entity->selftype == PLAYER){
                    
                    for (int i=0; i< MAXBULLETS; i++){
                        if (state.herobullets[i].dead){
                            state.herobullets[i].dead = false;
                            state.herobullets[i].position = entity->position;
                            if (entity->facingRight){
                                state.herobullets[i].movement.x = 1.0f;
                                state.herobullets[i].position.x += entity->spritewidth* entity->width* entity->contactWidth;
                            }   else{
                                state.herobullets[i].movement.x = -1.0f;
                                state.herobullets[i].position.x -= entity->spritewidth* entity->width* entity->contactWidth;
                            }
                            break;
                        }
                    }

                }   else{
                    
                    for (int i=0; i< MAXBULLETS; i++){
                        if (state.enemybullets[i].dead){
                            state.enemybullets[i].dead = false;
                            state.enemybullets[i].position = entity->position;
                            vec3 direction = state.Player->position - entity->position;
                            
                            if (entity->facingRight){
                                direction.x -= entity->spritewidth* entity->width* entity->contactWidth * 2;
                            }   else{
                                direction.x += entity->spritewidth* entity->width* entity->contactWidth * 2;
                            }
                            
                            state.enemybullets[i].movement = normalize(direction);
                            state.enemybullets[i].angle = atan(state.enemybullets[i].movement.y/state.enemybullets[i].movement.x);
                            if (entity->facingRight){
                                state.enemybullets[i].position.x += entity->spritewidth* entity->width* entity->contactWidth * 2;

                            }   else{
                                state.enemybullets[i].facingRight = false;
                                state.enemybullets[i].position.x -= entity->spritewidth* entity->width* entity->contactWidth * 2;
                            }
                            break;
                        }
                    }
                    
                }
            }
        }   else{
            entity->castcount = 0;
            entity->cast = false;
        }
        
    }   else if (entity->airAttack){
        
            Mix_PlayChannel(-1, entity->attacking, 0);
            
        if (entity->airAttackcount < entity->airAttackTex.size()-1){
            entity->airAttackcount += 1;
        }
            
    }   else if (entity->hit){
        
        if (entity->hitcount == 0){
                Mix_PlayChannel(-1, entity->hurt, 0);
        }
        
        if (entity->hitcount < entity->hitTex.size()-1){
            entity->hitcount += 1;
        }   else{
            entity->hitcount = 0;
            entity->hit = false;
            if (entity->life <= 0){
                entity->dead = true;
            }
        }
        
    }   else if (entity->fall){
            
            if (entity->fallcount < entity->fallTex.size()-1){
                entity->fallcount += 1;
            }   else{
                entity->fallcount = 0;
            }
            
    }   else if (entity->wallSlide && entity->velocity.y < 0){
        
            if (entity->wallSlidecount < entity->wallSlideTex.size()-1){
                entity->wallSlidecount += 1;
            }   else{
                entity->wallSlidecount = 0;
            }
            
    }   else if (entity->jump){
            
            if (entity->jumpcount < entity->jumpTex.size()-1){
                entity->jumpcount += 1;
            }   else if (!entity->wallSlide){
                entity->jumpcount = 0;
                entity->fall = true;
            }
            
    }   else {
        
        if (entity->attack){
            
            if (entity->attackcount == 0){
                
                Mix_PlayChannel(-1, entity->attacking, 0);
            
            }
            
            if (entity->attackcount < entity->attackTex.size()-1){
                entity->attackcount += 1;
            }   else{
                entity->attackcount = 0;
                entity->attack = false;
                entity->attacked = false;
            }
                
        }   else{
        
            if (entity->movement.x == 0 ){
                    
                    if (entity->idlecount < entity->idleTex.size()-1){
                        entity->idlecount += 1;
                    }   else{
                        entity->idlecount = 0;
                    }
                   
            }   else{
                entity->idlecount = 0;
            }
            
            if (entity->movement.x != 0){
                
                if (entity->movecount < entity->moveTex.size()-1){
                    entity->movecount += 1;
                }   else{
                    entity->movecount = 0;
                }
                  
            }   else{
                entity->movecount = 0;
            }
        }
    }
}


    //Rendering Player
void AnimRender(Entity* entity, ShaderProgram* program){
    
    if (entity->dead && !entity->deathTex.empty()){
    
        if (entity->facingRight ){
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->deathTex[entity->deathcount], entity->cols, entity->rows);
        }   else{
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->deathTex[entity->deathcount], entity->cols, entity->rows, true);
        }
        
    }   else if (entity->cast && !entity->castTex.empty()){
        
        if (entity->facingRight ){
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->castTex[entity->castcount], entity->cols, entity->rows);
        }   else{
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->castTex[entity->castcount], entity->cols, entity->rows, true);
        }
        
    }   else if (entity->hit && !entity->hitTex.empty()){
        
        if (entity->facingRight){
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->hitTex[entity->hitcount], entity->cols, entity->rows);
        }   else{
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->hitTex[entity->hitcount], entity->cols, entity->rows, true);
        }
        
    }   else if (entity->airAttack && !entity->airAttackTex.empty()){
        
        if (entity->facingRight){
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->airAttackTex[entity->airAttackcount], entity->cols, entity->rows);
        }   else{
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->airAttackTex[entity->airAttackcount], entity->cols, entity->rows, true);
        }
        
    }   else if(entity->fall && !entity->fallTex.empty()){
        
        if (entity->facingRight){
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->fallTex[entity->fallcount], entity->cols, entity->rows);
        }   else{
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->fallTex[entity->fallcount], entity->cols, entity->rows, true);
        }
        
    }   else if (entity->wallSlide && entity->velocity.y < 0 && !entity->wallSlideTex.empty()){
        
        if (entity->facingRight){
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->wallSlideTex[entity->wallSlidecount], entity->cols, entity->rows);
        }   else{
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->wallSlideTex[entity->wallSlidecount], entity->cols, entity->rows, true);
        }
        
    }   else if (entity->jump && !entity->jumpTex.empty()){
        
        if (entity->facingRight){
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->jumpTex[entity->jumpcount], entity->cols, entity->rows);
        }   else{
            CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->jumpTex[entity->jumpcount], entity->cols, entity->rows, true);
        }
        
    }   else{
        
        if (entity->attack && !entity->attackTex.empty()){
            
            if (entity->facingRight){
                CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->attackTex[entity->attackcount], entity->cols, entity->rows);
            }   else{
                CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->attackTex[entity->attackcount], entity->cols, entity->rows, true);
            }
            
        }   else{
        
            if (entity->velocity.x == 0 && !entity->idleTex.empty()){
                
                if (entity->facingRight){
                    CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->idleTex[entity->idlecount], entity->cols, entity->rows);
                }   else{
                    CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->idleTex[entity->idlecount], entity->cols, entity->rows, true);
                }
                
            }
            
            if (entity->movement.x != 0 && !entity->moveTex.empty()){
                
                if (entity->facingRight){
                    CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->moveTex[entity->movecount], entity->cols, entity->rows);
                }   else{
                    CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->moveTex[entity->movecount], entity->cols, entity->rows, true);
                }
                
            }
            
        }
    }
}
    // Function for importing texture
GLuint LoadTexture(const char* filepath){
    int w, h, n;
    unsigned char* image = stbi_load(filepath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL){
        std::cout << "Unable to load image, check the filepath!\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    
    stbi_image_free(image);
    return textureID;
}


GLuint LoadTextureHighRes(const char* filepath){
    int w, h, n;
    unsigned char* image = stbi_load(filepath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL){
        std::cout << "Unable to load image, check the filepath!\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    stbi_image_free(image);
    return textureID;
}


// Customized Render with repeats and size changes and animation
void CustomRender(ShaderProgram* program, float yRepeat, float xRepeat, float height, float width, Entity* object, int index, int cols, int rows, bool xFlip, bool yFlip){
    
    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;
    
    float wid = 1 / (float)cols;
    float heig = 1 / (float)rows;
    
    float texCoords[12];
    
    if (cols == 1 && rows == 1){
        float inputTexCoords[] = { 0.0, yRepeat, xRepeat, yRepeat, xRepeat, 0.0, 0.0, yRepeat, xRepeat, 0.0, 0.0, 0.0 };
        
        if (xFlip) {
            for (int i=0; i< 12; i++){
                if (i % 2 == 0){
                    inputTexCoords[i] = xRepeat - inputTexCoords[i];
                }
            }
        }
        
        if (yFlip){
            for (int i=0; i< 12; i++){
                if (i % 2 != 0){
                    inputTexCoords[i] = yRepeat - inputTexCoords[i];
                }
            }
        }
        
        for (int i=0; i< 12; i++){
            texCoords[i] = inputTexCoords[i];
        }
    }   else{
        float inputTexCoords[] = {u, v + heig, u + wid, v + heig, u + wid, v, u, v + heig, u + wid, v, u, v};
        
        if (xFlip) {
            for (int i=0; i< 12; i++){
                if (i % 2 == 0){
                    inputTexCoords[i] = (2 * u) + wid - inputTexCoords[i];
                }
            }
        }
        
        if (yFlip){
            for (int i=0; i< 12; i++){
                if (i % 2 != 0){
                    inputTexCoords[i] = (2 * v) + heig - inputTexCoords[i];
                }
            }
        }
        
        for (int i=0; i< 12; i++){
            texCoords[i] = inputTexCoords[i];
        }
    }
    
    float vertices[]  = { -width*xRepeat*object->spritewidth/2, -height*yRepeat*object->spriteheight/2, width*xRepeat*object->spritewidth/2, -height*yRepeat*object->spriteheight/2, width*xRepeat*object->spritewidth/2, height*yRepeat*object->spriteheight/2, -width*xRepeat*object->spritewidth/2, -height*yRepeat*object->spriteheight/2, width*xRepeat*object->spritewidth/2, height*yRepeat*object->spriteheight/2, -width*xRepeat*object->spritewidth/2, height*yRepeat*object->spriteheight/2 };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    object->render(program);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


//Rendering Text
void DrawText(ShaderProgram* program, GLuint fontTextureID, string text, float size, float spacing, vec3 position){
    
    float width = 1.0f/ 16.0f;
    float height = 1.0f/ 16.0f;
    
    vector<float> vertices;
    vector<float> textCoords;
    
    for (int i = 0; i < text.size(); i++){
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(),{
            offset +(-0.5f*size), 0.5f* size,
            offset +(-0.5f*size), -0.5f* size,
            offset +(0.5f*size), 0.5f* size,
            offset +(0.5f*size), -0.5f* size,
            offset +(0.5f*size), 0.5f* size,
            offset +(-0.5f*size), -0.5f* size
        });
        
        textCoords.insert(textCoords.end(),{
            u, v,
            u, v+height,
            u + width, v,
            u + width, v+height,
            u + width, v,
            u, v + height
        });
    }
    
    mat4 modelMatrix = translate(mat4(1),position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)text.size()*6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

