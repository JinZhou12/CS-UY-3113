
#include "MainScreen.h"
#define ENEMYCOUNT 7
#define MYSHOTCOUNT 0
#define BADSHOTCOUNT 0

using namespace std;
using namespace glm;

void Main::Initialize(Entity *player){
    // Map
    static unsigned int levelData[] =
    {
        16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,
        16,   4,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,   5,  16,
        16,  17,  60,  60,  60,  60,  60,  60,  60,  60,  60,  60,  60,  60,  15,  16,
        16,  17,  64,  79,  79,  64,  64,  79,  64,  79,  65,  79,  79,  79,  15,  16,
        16,  17,  79,  79,  79,  64,  64,  65,  79,  79,  80,  64,  79,  65,  15,  16,
        16,  17,  64,  65,  79,  79,  79,  80,  64,  80,  79,  64,  79,  80,  15,  16,
        16,  17,  64,  80,  79,  79,  64,  64,  79,  64,  79,  79,  79,  79,  15,  16,
        16,  17,  64,  79,  79,  64,  64,  79,  64,  79,  65,  79,  79,  65,  15,  16,
        16,  17,  79,  79,  79,  64,  64,  65,  79,  79,  80,  64,  79,  80,  15,  16,
        16,  17,  64,  79,  79,  79,  79,  80,  64,  80,  79,  64,  79,  79,  15,  16,
        16,  19,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,  20,  16,
        16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16
    };
    
    static unsigned int levelOverlay[] =
    {
         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,  55,  56,  56,  57,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,  70,  71,  71,  72,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,  70,  71,  71,  72,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,  70,  71,  71,  72,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,  85,  86,  86,  87,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
    };
    
    state.map = new Map(16, 12, levelData, levelOverlay, Util::LoadTexture("dungeon.png"), 1.0f, 15, 12);
    vector<int> solids = {0, 1, 2, 4, 5, 15, 16, 17, 19, 20, 30, 31, 32, 60, 61, 62, 135, 153};
    state.map->setSolids(solids);
    
    
    // Enemy
    state.Enemy = new Entity[ENEMYCOUNT];

    state.Enemy[0].texture = Util::LoadTexture("boss.png");
    state.Enemy[0].cols = 8;
    state.Enemy[0].life = 25;
    state.Enemy[0].maxSpeed = 1.0f;
    state.Enemy[0].height = 2.0f;
    state.Enemy[0].width = 2.0f;
    state.Enemy[0].contactHeight = 0.5f;
    state.Enemy[0].contactWidth = 0.3f;
    state.Enemy[0].idleTex = {0,1,2,3};
    state.Enemy[0].moveTex = {4,5,6,7};
    state.Enemy[0].position = vec3(7.5f, -3.0f, 0);
    state.Enemy[0].attackCooldown = 3.0f;
    state.Enemy[0].texCentered = false;
    
    GLuint chaser = Util::LoadTexture("chort.png");
    for (int i=1; i< 5; i++){
        state.Enemy[i].selftype = CHASER;
        state.Enemy[i].texture = chaser;
        state.Enemy[i].cols = 8;
        state.Enemy[i].spriteheight = 1.75f;
        state.Enemy[i].contactHeight = 0.6f;
        state.Enemy[i].contactWidth = 0.7f;
        state.Enemy[i].height = 0.7f;
        state.Enemy[i].width = 0.7f;
        state.Enemy[i].speed = 0;
        state.Enemy[i].maxSpeed = 2.0f;
        state.Enemy[i].moveTex = {4,5,6,7};
        state.Enemy[i].idleTex = {0,1,2,3};
    }
    state.Enemy[1].position = vec3(3.0f, -6.0f, 0);
    state.Enemy[2].position = vec3(3.0f, -8.0f, 0);
    state.Enemy[3].position = vec3(12.0f, -6.0f, 0);
    state.Enemy[3].facingRight = false;
    state.Enemy[4].position = vec3(12.0f, -8.0f, 0);
    state.Enemy[4].facingRight = false;
    
    GLuint necro = Util::LoadTexture("necro.png");
    for (int i=5; i< ENEMYCOUNT; i++){
        state.Enemy[i].selftype = SHOOTER;
        state.Enemy[i].texCentered = false;
        state.Enemy[i].texture = necro;
        state.Enemy[i].rows = 4;
        state.Enemy[i].spriteheight = 1.2f;
        state.Enemy[i].contactHeight = 0.7f;
        state.Enemy[i].contactWidth = 0.7f;
        state.Enemy[i].height = 0.6f;
        state.Enemy[i].width = 0.7f;
        state.Enemy[i].maxSpeed = 1.0f;
        state.Enemy[i].moveTex = {0,1,2,3};
        state.Enemy[i].idleTex = {0,1,2,3};
        state.Enemy[i].attackCooldown = 1.5f;
    }
    state.Enemy[5].position = vec3(3.0f, -4.0f, 0);
    state.Enemy[6].position = vec3(12.0f, -4.0f, 0);
    state.Enemy[6].facingRight = false;
    
}


void Main::Update(float &lastTick, float deltaTime, float animStep, float &animTime){
    
    // General Updates
        // Enemy
        
    for (int i = 0; i < ENEMYCOUNT; i++){
        if (state.Enemy[i].active){
            state.Enemy[i].update(state.map, state.Player, 0, state.Enemy, ENEMYCOUNT, state.Enemyshots, BADSHOTCOUNT, deltaTime);
        }
    }
        
       
    // Updating animation
    while (animTime >= animStep){
            
        for (int i= 0; i < ENEMYCOUNT; i++){
            if (state.Enemy[i].active){
                Util::AnimUpdate(&state.Enemy[i]);
            }
        }
                
        animTime -= animStep;
    }
}


void Main::Render(ShaderProgram *program, GLuint fontTex, GLuint HPTex, bool &gameWon, bool &gameLost){
    
    // Rendering Map
    state.map->Render(program);

    // Rendering enemies
    for (int i = 0; i < ENEMYCOUNT; i++){
        if (state.Enemy[i].active){
            Util::AnimRender(&state.Enemy[i], program);
        }
    }
    
    Util::DrawText(program, fontTex, "Demon Castle", 1, -0.5f, vec3(4.9f, -4.5f, 0));
    Util::DrawText(program, fontTex, "Press Enter to", 0.3f, -0.15f, vec3(6.6f, -5.8f, 0));
    Util::DrawText(program, fontTex, "start the game", 0.3f, -0.15f, vec3(6.6f, -6.2f, 0));

    // Rendering pole tops so it covers player and enemies
    state.map->RenderPoleTops(program);
}


int Main::EnemyCount(){
    return ENEMYCOUNT;
}
