#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define FIXEDTIMESTEP 0.016666666
#define FIXEDANIMATIONSTEP 0.15

#include "Level 1.h"
#include "Level 2.h"
#include "Level 3.h"
#include "MainScreen.h"
#include "Effects.h"

void Initialize(SDL_Window *& displayWindow,ShaderProgram* program, Scene *&current, MainScreen *&main, Level1 *&level1, Level2 *&level2,
                Level3 *&level3, GLuint& fontTex, GLuint& HPTex, Effects *&effect);
void ProcessInput(ShaderProgram *program, Scene *&current, Effects *effect, bool& gameIsRunning, bool& gameWon, bool& gameLost);
void Update(ShaderProgram* program, float& lastTick, float& deltaTime, float& animTime, Scene *current, Effects *effect, bool& gameWon, bool& gameLost);
void Render(SDL_Window* displayWindow, Scene *current, ShaderProgram* program, GLuint fontTex, GLuint HPTex, Effects *effect, bool& gameWon, bool& gameLost);
void Shutdown(Scene *current);

using namespace glm;
using namespace std;

// main game loop
int main(int argc, const char * argv[]){
    bool gameIsRunning = true;
    bool gameWon = false;
    bool gameLost = false;
    ShaderProgram program;
    SDL_Window *displayWindow;
    GLuint fontTex, HPTex;
    Effects *effect;
    Scene *currentScene = NULL;
    MainScreen *main;
    Level1 *level1;
    Level2 *level2;
    Level3 *level3;
    
    float lastTick = 0;
    float deltaTime = 0;
    float animTime = 0;
    
    Initialize(displayWindow, &program, currentScene, main, level1, level2, level3, fontTex, HPTex, effect);
    while (gameIsRunning){
        ProcessInput(&program, currentScene, effect, gameIsRunning, gameWon, gameLost);
        Update(&program, lastTick, deltaTime, animTime, currentScene, effect, gameWon, gameLost);
        Render(displayWindow, currentScene, &program, fontTex, HPTex, effect, gameWon, gameLost);
    }
    Shutdown(currentScene);
}


void SwitchToScene(Scene *&current, Scene *scene, Effects *effect){
    effect->Start(FADEIN);
    Entity *player = NULL;
    if (current != NULL){
        Mix_Pause(-1);
        Mix_FreeMusic(current->state.music);
        player = current->state.Player;
    }
    current = scene;
    current->Initialize(player);
}


void Initialize(SDL_Window *&displayWindow,ShaderProgram* program, Scene *&current, MainScreen *&main, Level1 *&level1, Level2 *&level2,
                Level3 *&level3, GLuint& fontTex, GLuint& HPTex, Effects *&effect) {
    
    SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Monster Slayer2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program->Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

    mat4 viewMatrix = mat4(1);
    mat4 projectionMatrix = ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program->SetProjectionMatrix(projectionMatrix);
    program->SetViewMatrix(viewMatrix);

    
//    state.music = Mix_LoadMUS("theme.mp3");
//    Mix_PlayMusic(state.music, -1);
    
    glUseProgram(program->programID);
    
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTex = Util::LoadTexture("font.png");
    HPTex = Util::LoadTexture("heart.png");
    
    effect = new Effects(projectionMatrix, viewMatrix);
    
    main = new MainScreen();
    main->scenetype = MAIN;
    
    level1 = new Level1();
    level1->scenetype = LEVEL;
    
    level2 = new Level2();
    level2->scenetype = LEVEL;
    
    level3 = new Level3();
    level3->scenetype = LEVEL;
    
    main->nextScene = level1;
    level1->nextScene = level2;
    level2->nextScene = level3;
    
    SwitchToScene(current, main, effect);
}


void ProcessInput(ShaderProgram *program, Scene *&current, Effects *effect, bool& gameIsRunning, bool& gameWon, bool& gameLost) {
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
                        if (current->scenetype != MAIN && !gameWon && !gameLost && (!current->state.Player->jump || !current->state.Player->doubleJump)){
                            Mix_PlayChannel(-1, current->state.Player->jumping, 0);
                            if (current->state.Player->jump){
                                current->state.Player->doubleJump = true;
                            }
                            current->state.Player->fall = false;
                            current->state.Player->jump = true;
                            current->state.Player->velocity.y = 4.5f;
                        }
                        break;
                    
                    
                    case SDLK_e:
                        if (current->state.Player->atGoal && !gameWon && !gameLost){
                            current->state.Player->atGoal = false;
                            SwitchToScene(current, current->nextScene, effect);
                        }
                        
                        
                    case SDLK_RETURN:
                        if (current->scenetype == MAIN){
                            SwitchToScene(current, current->nextScene, effect);
                        }
                        
                    case SDLK_ESCAPE:
                        if (gameWon || gameLost){
                            gameIsRunning = false;
                        }
                }
        }
    }
    
    if (!gameWon && !gameLost){
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        if (!gameWon && !gameLost){
            if (current->scenetype != MAIN){
                if (keys[SDL_SCANCODE_A]){
                    current->state.Player->movement.x = -1;
                }   else if (keys[SDL_SCANCODE_D]){
                    current->state.Player->movement.x = 1;
                }   else{
                    current->state.Player->movement.x = 0;
                }
            }
        }
    }
}


void Update(ShaderProgram* program, float& lastTick, float& deltaTime, float& animTime, Scene *current, Effects *effect, bool& gameWon, bool& gameLost){
    
    // Calculating deltaTime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    deltaTime += ticks - lastTick;
    animTime += ticks - lastTick;
    lastTick = ticks;
    
    if (current->scenetype != MAIN){
        if (current->state.Player->atGoal && current->nextScene == NULL){
            gameWon = true;
        }
        
        if (!current->state.Player->active && !gameWon){
            gameLost = true;
        }
        
        // Updating status of enemies and player running sound effect
        if(current->state.Player->onGround){
            if(current->state.Player->movement.x != 0){
                Mix_VolumeChunk(current->state.Player->running, MIX_MAX_VOLUME);
            }   else{
                Mix_VolumeChunk(current->state.Player->running, 0);
            }
        }   else{
            Mix_VolumeChunk(current->state.Player->running, 0);
        }

        if (gameWon || gameLost){
            Mix_VolumeChunk(current->state.Player->running, 0);
        }
    }
    
    // Updating
    if ((!gameWon && !gameLost) || (gameWon && !current->state.Player->onGround)){
        while(deltaTime >= FIXEDTIMESTEP){
            current->Update(lastTick, FIXEDTIMESTEP, FIXEDANIMATIONSTEP, animTime);
            
            // Updating view matrix
            mat4 viewMatrix;
            if (current->scenetype != MAIN){
                if (current->state.Player->position.x > 5.0f){
                    if (current->state.Player->position.x < current->state.map->getEnd()){
                        viewMatrix = translate(mat4(1.0f),vec3(-current->state.Player->position.x, 3.5f, 0));
                        current->state.Background->position = vec3(7.5*1.5 - 5.0f/3 + current->state.Player->position.x / 3, -3.5f, 0);
                    }   else{
                        viewMatrix = translate(mat4(1.0f),vec3(-current->state.map->getEnd(), 3.5f, 0));
                    }
                }   else{
                    viewMatrix = translate(mat4(1.0f),vec3(-5.0f, 3.5f, 0));
                }
                program->SetViewMatrix(viewMatrix);
            }

            effect->Update(FIXEDTIMESTEP);
            deltaTime -= FIXEDTIMESTEP;
        }
    }
}


void Render(SDL_Window* displayWindow, Scene *current, ShaderProgram* program, GLuint fontTex, GLuint HPTex, Effects *effect, bool& gameWon, bool& gameLost){
    
    glUseProgram(program->programID);
    current->Render(displayWindow, program, fontTex, HPTex, gameWon, gameLost);
    
    if (current->scenetype != MAIN){
        if (current->state.Player->atGoal && current->nextScene != NULL){
            Util::DrawText(program, fontTex, "Press E to", 0.3, -0.15, vec3(current->state.Player->position.x - 0.6f, current->state.Player->position.y + 0.6f, 0));
            Util::DrawText(program, fontTex, "continue", 0.3, -0.15, vec3(current->state.Player->position.x - 0.5f, current->state.Player->position.y + 0.4f, 0));
        }
    }
    
    // Rendering ending scene
    if(gameWon){
        vec3 position1;
        vec3 position2;
        if (current->state.Player->position.x > 5.0f){
            if (current->state.Player->position.x < current->state.map->getEnd()){
                position1 = vec3(current->state.Player->position.x-1.7f,-3.0f,0);
                position2 = vec3(current->state.Player->position.x-1.7f,-3.65f,0);
            }   else{
                position1 = vec3(current->state.map->getEnd()-1.7f,-3.0f,0);
                position2 = vec3(current->state.map->getEnd()-1.7f,-3.65f,0);
            }
        }
        Util::DrawText(program, fontTex, "You Won!", 1, -.5, position1);
        Util::DrawText(program, fontTex, "Press ESC to quit", .3, -.1, position2);
    }

    if(gameLost){
        vec3 position1;
        vec3 position2;
        if (current->state.Player->position.x > 5.0f){
            if (current->state.Player->position.x < current->state.map->getEnd()){
                position1 = vec3(current->state.Player->position.x-2.0f,-3.0f,0);
                position2 = vec3(current->state.Player->position.x-1.7f,-3.65f,0);
            }   else{
                position1 = vec3(current->state.map->getEnd()-2.0f,-3.0f,0);
                position2 = vec3(current->state.map->getEnd()-1.7f,-3.65f,0);
            }
        }
        Util::DrawText(program, fontTex, "Game Over", 1, -.5, position1);
        Util::DrawText(program, fontTex, "Press ESC to quit", .3, -.1, position2);
    }
    
    effect->Render();
    
    SDL_GL_SwapWindow(displayWindow);
}


// Shuting down
void Shutdown(Scene *current) {
    if (current->scenetype != MAIN){
        Mix_FreeChunk(current->state.Player->jumping);
        Mix_FreeChunk(current->state.Player->running);
    }
    Mix_FreeMusic(current->state.music);
    Mix_CloseAudio();
    SDL_Quit();
}
