#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include "Level 2.h"

#define STB_IMAGE_IMPLEMENTATION

#define ENEMYCOUNT 4

using namespace glm;
using namespace std;


// Initializing all Entities
void Level2::Initialize(Entity *player) {
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // Map
    static unsigned int levelData[] =
        {
            14, 45, 45, 45, 45, 12, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 12, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 ,45, 45, 45, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 12, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 12, 14, 45, 45, 45, 45, 42, 43, 43, 43, 43, 43, 43, 43,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 12, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 12, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 12, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 12, 13, 13, 13,  3, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 12, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 12, 13, 13, 13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 12, 27,  1,  1,  1,  1,  1,  1,  1,  2, 45, 45, 45, 45, 12, 13, 13, 13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 45, 45, 45, 45, 12, 13, 13, 13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 12, 13, 13, 13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 12, 13, 13, 13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 12, 13, 13, 13, 14, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
            27,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2, 45, 45, 45,  0,  1,  1,  1,  1, 29, 13, 13, 13, 27,  1,  1,  2, 45, 45, 45, 45, 45,  0,  1,    1,  1,  1,  1,  1,
            13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 45, 45, 45, 45, 45, 12, 13, 13, 13, 13, 13, 13
        };
    
    state.map = new Map(39, 15, levelData ,Util::LoadTexture("tilesheet.png"), 0.5f, 12, 6);
    vector<vec3> spawnPoints = {vec3(9,4,0), vec3(35,5,0)};
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
    state.Player = player;
    state.Player->position = vec3(2.0f, -2.0f, 0);


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
    
    state.Enemy[0].position = vec3(6.5f, -3.5f, 0);
    state.Enemy[1].position = vec3(10.0f, 0, 0);
    state.Enemy[2].position = vec3(16.0f, -5.0f, 0);
    state.Enemy[3].position = vec3(7.5f, -5.5f, 0);


    // Goal
    state.Goal = new Entity(GOAL, Util::LoadTexture("door.png"));
    state.Goal->position = vec3(12.5f, -5.5f, 0);
    state.Goal->movement.y = -0.1f;
    
    //For transparency of the image background
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void Level2::Update(float& lastTick, float deltaTime, float animStep, float& animTime) {

            
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


void Level2::Render(SDL_Window* displayWindow, ShaderProgram* program, GLuint fontTex, GLuint HPTex, bool& gameWon, bool& gameLost) {
    
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
}

