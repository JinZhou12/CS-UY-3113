
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include "Level 1.h"

#define STB_IMAGE_IMPLEMENTATION

#define ENEMYCOUNT 2

using namespace glm;
using namespace std;


// Initializing all Entities
void Level1::Initialize(Entity *player) {
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // Map
    static unsigned int levelData[] =
        {
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 42, 43, 43, 43, 43, 43, 43, 44, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            13, 27,  1,  1,  1,  1,  1,  1,  1,  1,  2, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45, 45,  0,  1,  1,  1,  1,  1,  1,  1,  1,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 45, 45, 45, 45 ,45 ,45, 45, 45, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 13, 13,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 13, 13,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45,  0,  1,  1,  1,  1,  1,  1,  2, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 13, 13,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 13, 13
        };
    
    state.map = new Map(34, 15, levelData ,Util::LoadTexture("tilesheet.png"), 0.5f, 12, 6);
    vector<vec3> spawnPoints = {vec3(8,8,0), vec3(20,5,0)};
    state.map->setSpawn(spawnPoints);
    
    
    //Background
    state.Background = new Entity(BACKGROUND, Util::LoadTexture("background.png"), 1, 1, 1.0f, 1.5f);
    state.Background->xRepeat = 2;
    state.Background->position = vec3(7.5f*1.5f, -3.5f, 0);
    state.Background->height = 7.5f;
    state.Background->width = 7.5f;
    
    //Music
    state.music = Mix_LoadMUS("theme.mp3");
    Mix_PlayMusic(state.music, -1);
    
    //Player
    state.Player = new Entity(PLAYER,Util::LoadTexture("adventurer.png"), 10, 11, 0.74f, 1);
    state.Player->life = 3;
    state.Player->acceleration.y = -8.0f;
    state.Player->position = vec3(2.0f, -2.0f, 0);
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

    // Music
    Mix_VolumeChunk(state.Player->running, 0);
    Mix_PlayChannel(-1, state.Player->running, -1);
    
    // Enemy
    state.Enemy = new Entity[ENEMYCOUNT]();
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
    
    state.Enemy[0].position = vec3(10.0f, -4.5f, 0);
    state.Enemy[1].position = vec3(7.5f, -2.0f, 0);


    // Goal
    state.Goal = new Entity(GOAL, Util::LoadTexture("door.png"));
    state.Goal->position = vec3(15.5f, -4.5f, 0);
    state.Goal->movement.y = -0.1f;
    
    //For transparency of the image background
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void Level1::Update(float& lastTick, float deltaTime, float animStep, float& animTime) {

            
        // General Updates
        state.Player->update(state.map , state.Goal, state.Enemy, ENEMYCOUNT, deltaTime);
            
        state.Background->update(state.map, NULL, NULL, 0, deltaTime);
            
        for (int i = 0; i < ENEMYCOUNT; i++){
            if (state.Enemy[i].active){
                state.Enemy[i].update(state.map, NULL, state.Player, 1, deltaTime);
            }
        }
        
        state.Goal->update(state.map, NULL, NULL, 0, deltaTime);

       
        // Updating animation
        while (animTime >= animStep){
                
            for (int i= 0; i < ENEMYCOUNT; i++){
                if (state.Enemy[i].active){
                    Util::AnimUpdate(&state.Enemy[i], animTime);
                }
            }
                
            Util::AnimUpdate(state.Player, animTime);
                
            animTime -= animStep;
        }
}


void Level1::Render(SDL_Window* displayWindow, ShaderProgram* program, GLuint fontTex, GLuint HPTex, bool& gameWon, bool& gameLost) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Rendering Background
    Util::CustomRender(program, state.Background->yRepeat, state.Background->xRepeat, state.Background->height, state.Background->width, state.Background, 1, 1, 1);
    
    // Rendering Map
    state.map->Render(program);
    
    // Rendering door
    Util::CustomRender(program, 1, 1, 1, 1, state.Goal, 0, 1, 1);
    
    // Rendering enemies
    for (int i = 0; i < ENEMYCOUNT; i++){
        if (state.Enemy[i].active){
            Util::AnimRender(&state.Enemy[i], program);
        }
    }

    // Rendering Player
    Util::AnimRender(state.Player, program);
    
    // Rendering player hp
    vec3 position;
    
    if (state.Player->position.x >5.0f){
        if (state.Player->position.x < state.map->getEnd()){
            position = vec3(state.Player->position.x - 4.6f, -0.1f , 0);
        }   else{
            position = vec3(state.map->getEnd() - 4.6f, -0.1f , 0);
        }
    }   else{
        position = vec3(0.4f, -0.1f, 0);
    }
    
    Util::DisplayHealth(program, state, HPTex, position);
    
    bool hint = true;
    for (int i = 0; i < ENEMYCOUNT; i++){
        if (!state.Enemy[i].active){
            hint = false;
        }
    }
    
    if (hint){
        Util::DrawText(program, fontTex, "Jump on the enemies", 0.3, -0.15, vec3(state.Player->position.x - 1.2f, state.Player->position.y + 0.6f, 0));
        Util::DrawText(program, fontTex, "to kill them", 0.3, -0.15, vec3(state.Player->position.x - 0.7f, state.Player->position.y + 0.4f, 0));
    }
}



