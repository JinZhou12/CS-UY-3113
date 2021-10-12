#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define FIXEDTIMESTEP 0.016666666
#define FIXEDANIMATIONSTEP 0.15
#define MYSHOTCOUNT 5
#define BADSHOTCOUNT 10

#include "Effects.h"
#include "Util.h"
#include "MainScreen.h"
#include "Room1.h"
#include "Room2.h"
#include "Room3.h"
#include "Room4.h"

void Initialize(SDL_Window *&displayWindow,ShaderProgram* program, Mix_Chunk *&poof, Mix_Chunk *&pew, Scene *&main, Scene *&current, Scene *&room1, Scene *&room2, Scene *&room3, Scene *&room4, GLuint& fontTex, GLuint& HPTex, Effects *&effect);
void ProcessInput(ShaderProgram *program, Mix_Chunk *pew, Scene *&current, Effects *effect, bool& gameIsRunning, bool& gameWon, bool& gameLost);
void Update(ShaderProgram* program, float& lastTick, float& deltaTime, float& animTime, Mix_Chunk *poof, Scene *&current, Effects *effect, bool& gameWon, bool& gameLost);
void Render(SDL_Window* displayWindow, Scene *current, ShaderProgram* program, GLuint fontTex, GLuint HPTex, Effects *effect, bool& gameWon, bool& gameLost);
void Shutdown();

using namespace glm;
using namespace std;

// main game loop
int main(int argc, const char * argv[]){
    bool gameIsRunning = true;
    bool gameWon = false;
    bool gameLost = false;
    Scene *currScene;
    Scene *main;
    Scene *room1;
    Scene *room2;
    Scene *room3;
    Scene *room4;
    Mix_Chunk *poof;
    Mix_Chunk *pew;
    ShaderProgram program;
    SDL_Window *displayWindow;
    GLuint fontTex, HPTex;
    Effects *effect;
    
    float lastTick = 0;
    float deltaTime = 0;
    float animTime = 0;
    
    Initialize(displayWindow, &program, poof, pew, currScene, main, room1, room2, room3, room4, fontTex, HPTex, effect);
    while (gameIsRunning){
        ProcessInput(&program, pew, currScene, effect, gameIsRunning, gameWon, gameLost);
        Update(&program, lastTick, deltaTime, animTime, poof, currScene, effect, gameWon, gameLost);
        Render(displayWindow, currScene, &program, fontTex, HPTex, effect, gameWon, gameLost);
    }
    Shutdown();
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
    current->state.music = Mix_LoadMUS("theme.mp3");
    Mix_VolumeMusic(MIX_MAX_VOLUME / 12);
    Mix_PlayMusic(current->state.music, -1);
    current->Initialize(player);
}


void Initialize(SDL_Window *&displayWindow, ShaderProgram* program, Mix_Chunk *&poof, Mix_Chunk *&pew, Scene *&current, Scene *&main, Scene *&room1, Scene *&room2, Scene *&room3, Scene *&room4, GLuint& fontTex, GLuint& HPTex, Effects *&effect) {
    
    SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Demon Castle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 960, 720);
    
    program->Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

    mat4 viewMatrix = translate(mat4(1),vec3(-7.5f, 5.5f, 0));
    mat4 projectionMatrix = ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program->SetProjectionMatrix(projectionMatrix);
    program->SetViewMatrix(viewMatrix);
    
    glUseProgram(program->programID);
    
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTex = Util::LoadTexture("font.png");
    HPTex = Util::LoadTexture("heart.png");
    
    poof = Mix_LoadWAV("poof.wav");
    pew = Mix_LoadWAV("playerShot.wav");
    Mix_VolumeChunk(pew, MIX_MAX_VOLUME / 12);
    
    effect = new Effects(projectionMatrix, viewMatrix);
    
    main = new Main();
    main->scenetype = MAIN;
    
    room1 = new Room1();
    room1->scenetype = ROOM;
    
    room2 = new Room2();
    room2->scenetype = ROOM;
    
    room3 = new Room3();
    room3->scenetype = ROOM;
    
    room4 = new Room4();
    room4->scenetype = ROOM;
    
    main->nextScene = room1;
    room1->nextScene = room2;
    room2->nextScene = room3;
    room3->nextScene = room4;
    
    SwitchToScene(current, main, effect);
}


void ProcessInput(ShaderProgram *program, Mix_Chunk *pew, Scene *&current, Effects *effect, bool& gameIsRunning, bool& gameWon, bool& gameLost) {
    SDL_Event event;
    
    // Start/ Restart/ Quit inputs
    while (SDL_PollEvent(&event)) {
        vec3 myPosition;
        if (current->scenetype != MAIN){
            myPosition = current->state.Player->position;
            if (!current->state.Player->texCentered){
                myPosition.y = (current->state.Player->position.y - (1 - current->state.Player->contactHeight)* current->state.Player->height* current->state.Player->yRepeat* current->state.Player->spriteheight / 2);
            }
        }
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                        
                    case SDLK_UP:
                        if (current->scenetype != MAIN && current->state.Player->attackCooldown <= current->state.Player->cooldownCounter){
                            current->state.Player->cooldownCounter = 0;
                            for (int i=0; i< MYSHOTCOUNT; i++){
                                if (!current->state.Myshots[i].active){
                                    Mix_PlayChannel(-1, pew, 0);
                                    current->state.Myshots[i].active = true;
                                    current->state.Myshots[i].movement = vec3(0, 1.0f, 0);
                                    current->state.Myshots[i].position = myPosition;
                                    break;
                                }
                            }
                        }
                        break;
                        
                    case SDLK_DOWN:
                        if (current->scenetype != MAIN && current->state.Player->attackCooldown <= current->state.Player->cooldownCounter){
                            current->state.Player->cooldownCounter = 0;
                            for (int i=0; i< MYSHOTCOUNT; i++){
                                if (!current->state.Myshots[i].active){
                                    Mix_PlayChannel(-1, pew, 0);
                                    current->state.Myshots[i].active = true;
                                    current->state.Myshots[i].movement = vec3(0, -1.0f, 0);
                                    current->state.Myshots[i].position = myPosition;
                                    break;
                                }
                            }
                        }
                        break;
                        
                    case SDLK_LEFT:
                        if (current->scenetype != MAIN && current->state.Player->attackCooldown <= current->state.Player->cooldownCounter){
                            current->state.Player->cooldownCounter = 0;
                            for (int i=0; i< MYSHOTCOUNT; i++){
                                if (!current->state.Myshots[i].active){
                                    Mix_PlayChannel(-1, pew, 0);
                                    current->state.Myshots[i].active = true;
                                    current->state.Myshots[i].movement = vec3(-1.0f, 0, 0);
                                    current->state.Myshots[i].position = myPosition;
                                    break;
                                }
                            }
                        }
                        break;
                        
                    case SDLK_RIGHT:
                        if (current->scenetype != MAIN && current->state.Player->attackCooldown <= current->state.Player->cooldownCounter){
                            current->state.Player->cooldownCounter = 0;
                            for (int i=0; i< MYSHOTCOUNT; i++){
                                if (!current->state.Myshots[i].active){
                                    Mix_PlayChannel(-1, pew, 0);
                                    current->state.Myshots[i].active = true;
                                    current->state.Myshots[i].movement = vec3(1.0f, 0, 0);
                                    current->state.Myshots[i].position = myPosition;
                                    break;
                                }
                            }
                        }
                        break;
                        
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
    
    if (!gameWon && !gameLost && current->scenetype != MAIN){
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        if (keys[SDL_SCANCODE_A]){
            current->state.Player->movement.x = -1;
        }   else if (keys[SDL_SCANCODE_D]){
            current->state.Player->movement.x = 1;
        }   else{
            current->state.Player->movement.x = 0;
        }
            
        if (keys[SDL_SCANCODE_W]){
            current->state.Player->movement.y = 1;
        }   else if (keys[SDL_SCANCODE_S]){
            current->state.Player->movement.y = -1;
        }   else{
            current->state.Player->movement.y = 0;
        }
            
        if (current->state.Player->movement.x != 0 && current->state.Player->movement.y != 0){
            current->state.Player->movement = normalize(current->state.Player->movement);
        }
            
    }
}


void Update(ShaderProgram* program, float& lastTick, float& deltaTime, float& animTime, Mix_Chunk *poof, Scene *&current, Effects *effect, bool& gameWon, bool& gameLost){
    
    // Calculating deltaTime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    deltaTime += ticks - lastTick;
    animTime += ticks - lastTick;
    lastTick = ticks;
    
    if (current->scenetype != MAIN){
        if (!current->state.Player->active && !gameWon){
            gameLost = true;
        }
    }
        
        // Updating status of enemies and player running sound effect
//        if(current->state.Player->onGround){
//            if(current->state.Player->movement.x != 0){
//                Mix_VolumeChunk(current->state.Player->running, MIX_MAX_VOLUME);
//            }   else{
//                Mix_VolumeChunk(current->state.Player->running, 0);
//            }
//        }   else{
//            Mix_VolumeChunk(current->state.Player->running, 0);
//        }
//
//        if (gameWon || gameLost){
//            Mix_VolumeChunk(current->state.Player->running, 0);
//        }
  //  }
    
    // Updating
    if (!gameWon && !gameLost){
        while(deltaTime >= FIXEDTIMESTEP){

            current->Update(lastTick, FIXEDTIMESTEP, FIXEDANIMATIONSTEP, animTime);
            
            if (!current->state.map->finished()){
                bool cleared = true;
                for (int i=0; i<current->EnemyCount(); i++){
                    if (current->state.Enemy[i].active){
                        cleared = false;
                        break;
                    }
                }
                
                if (cleared){
                    if (current->nextScene != NULL){
                        current->state.map->adjustMap(6, 17);
                        current->state.map->adjustMap(7, 152);
                        current->state.map->adjustMap(8, 152);
                        current->state.map->adjustMap(9, 15);
                        current->state.map->adjustMap(22, 32);
                        current->state.map->adjustMap(23, 152);
                        current->state.map->adjustMap(24, 152);
                        current->state.map->adjustMap(25, 30);
                        current->state.map->adjustMap(39, 152);
                        current->state.map->adjustMap(40, 152);
                        Mix_PlayChannel(-1, poof, 0);
                        current->state.map->rebuild();
                    }   else{
                        gameWon = true;
                    }
                }
            }
            
            if (current->scenetype != MAIN && current->state.Player->position.y > 0){
                SwitchToScene(current, current->nextScene, effect);
            }
            
            // Updating view matrix
            if (current->scenetype != MAIN){
                float viewX = -5.5f;
                float viewY = -current->state.map->getEndY();

                if (current->state.Player->position.x > 5.5f){
                    if (current->state.Player->position.x < current->state.map->getEndX()){
                        viewX = -current->state.Player->position.x;
                    }   else{
                        viewX = -current->state.map->getEndX();
                    }
                }

                if (current->state.Player->position.y > current->state.map->getEndY()){
                    if (current->state.Player->position.y < -4.3f){
                        viewY = -current->state.Player->position.y;
                    }   else{
                        viewY = 4.3f;
                    }
                }

                mat4 viewMatrix = translate(mat4(1.0f), vec3(viewX, viewY, 0));
                program->SetViewMatrix(viewMatrix);
            }
            
            effect->Update(FIXEDTIMESTEP);
            deltaTime -= FIXEDTIMESTEP;
        }

    }
}


void Render(SDL_Window* displayWindow, Scene *current, ShaderProgram* program, GLuint fontTex, GLuint HPTex, Effects *effect, bool& gameWon, bool& gameLost){
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program->programID);
    
    current->Render(program, fontTex, HPTex, gameWon, gameLost);
    
    float HPposX = 1.0f;
    float HPposY = current->state.map->getEndY() + 3.3f;
    
    float endposX = 5.5f;
    float endposY = current->state.map->getEndY();

    if (current->scenetype != MAIN){
        if (current->state.Player->position.x > 5.5f){
            if (current->state.Player->position.x < current->state.map->getEndX()){
                HPposX = current->state.Player->position.x - 4.5f;
                endposX = current->state.Player->position.x;
            }   else{
                HPposX = current->state.map->getEndX() - 4.5f;
                endposX = current->state.map->getEndX();
            }
        }

        if (current->state.Player->position.y > current->state.map->getEndY()){
            if (current->state.Player->position.y < -4.3f){
                HPposY = current->state.Player->position.y + 3.3f;
                endposY = current->state.Player->position.y;
            }   else{
                HPposY = -1.0f;
                endposY = 4.3f;
            }
        }
        
        // Rendering player health
        Util::DisplayHealth(program, current->state, HPTex, vec3(HPposX,HPposY,0));
        
        // Rendering ending scene
        if(gameWon){
            Util::DrawText(program, fontTex, "You Won!", 1, -.5, vec3(endposX - 1.7f, endposY + 0.5f, 0));
            Util::DrawText(program, fontTex, "Press ESC to quit", .3, -.1, vec3(endposX - 1.7f, endposY - 0.65f, 0));
        }
        
        if(gameLost){
            Util::DrawText(program, fontTex, "Game Over", 1, -.5, vec3(endposX - 2.0f, endposY + 0.5f, 0));
            Util::DrawText(program, fontTex, "Press ESC to quit", .3, -.1, vec3(endposX - 1.7f, endposY - 0.65f, 0));
        }
    }
    effect->Render();
    
    SDL_GL_SwapWindow(displayWindow);
}


// Shuting down
void Shutdown() {
    /*
    if (current->scenetype != MAIN){
        Mix_FreeChunk(current->state.Player->jumping);
        Mix_FreeChunk(current->state.Player->running);
    }
    Mix_FreeMusic(current->state.music);
    Mix_CloseAudio();
     */
    SDL_Quit();
}
