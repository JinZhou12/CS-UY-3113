
#include "Room3.h"
#define ENEMYCOUNT 8
#define MYSHOTCOUNT 5
#define BADSHOTCOUNT 20

using namespace std;
using namespace glm;

void Room3::Initialize(Entity *player){
    // Map
    static unsigned int levelData[] =
    {
        16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,
        16,   4,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,   5,  16,
        16,  17,  60, 135,  60, 135,  60,  60,  60,  60, 135,  60, 135,  60,  15,  16,
        16,  17,  64,  79,  16,  64,  64,  79,  64,  79,  65,  79,  79,  79,  15,  16,
        16,  17,  79,  79,  16,  64,  64,  65,  79,  79,  80,  64,  79,  65,  15,  16,
        16,  17,  16,  16,  16,  79,  79,  80,  64,  80,  79,  64,  79,  80,  15,  16,
        16,  17,  64,  65,  79,  79,  64,  64,  79,  64,  79,  79,  79,  79,  15,  16,
        16,  17,  64,  80,  79,  64,  64,  79,  64,  79,  65,  16,  16,  16,  15,  16,
        16,  17,  79,  79,  96,  64,  64,  65,  79,  79,  80,  16,  79,  80,  15,  16,
        16,  17,  64,  79, 111, 112,  79,  80,  64,  80,  79,  16,  79,  79,  15,  16,
        16,  19,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,  20,  16,
        16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16
    };
    
    static unsigned int levelOverlay[] =
    {
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,  55,  56,  56,  57,   0,   0,   0,  70,   0,   0,
        0,   0,   0,   0,   0,   0,  70,  71,  71,  72,   0,   0,   0,  70,   0,   0,
        0,   0,   0,   0,   0,   0,  70,  71,  71,  72,   0,   0,   0,  70,   0,   0,
        0,   0,  72,   0,   0,   0,  70,  71,  71,  72,   0,   0,   0,  70,   0,   0,
        0,   0,  72,   0,   0,   0,  70,  71,  71,  72,   0,   0,   0,   0,   0,   0,
        0,   0,  72,   0,   0,   0,  70,  71,  71,  72,   0,   0,   0,   0,   0,   0,
        0,   0,  72,   0,   0,   0,  85,  86,  86,  87,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
    };
    
    state.map = new Map(16, 12, levelData, levelOverlay, Util::LoadTexture("dungeon.png"), 1.0f, 15, 12);
    vector<int> solids = {0, 1, 2, 4, 5, 15, 16, 17, 19, 20, 30, 31, 32, 60, 61, 62, 135, 153};
    state.map->setSolids(solids);

    // Player
    state.Player = player;
    state.Player->position = vec3(7.5f, -8.0f, 0);

    
    // Player Weapon
    state.PlayerWeapon = new Entity(WEAPON, Util::LoadTexture("wand.png"), 1, 1, 3.75f);
    state.PlayerWeapon->height = 0.2f;
    state.PlayerWeapon->width = 0.2f;
    
    
    // Enemy
    state.Enemy = new Entity[ENEMYCOUNT];
    GLuint chaser = Util::LoadTexture("chort.png");
    for (int i=0; i< 4; i++){
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
    state.Enemy[0].position = vec3(2.0f, -8.0f, 0);
    state.Enemy[1].position = vec3(3.0f, -9.0f, 0);
    state.Enemy[2].position = vec3(12.0f, -3.0f, 0);
    state.Enemy[3].position = vec3(13.0f, -4.0f, 0);
    
    
    GLuint necro = Util::LoadTexture("necro.png");
    for (int i=4; i< ENEMYCOUNT; i++){
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
    state.Enemy[4].position = vec3(2.0f, -3.0f, 0);
    state.Enemy[5].position = vec3(3.0f, -4.0f, 0);
    state.Enemy[6].position = vec3(12.0f, -8.0f, 0);
    state.Enemy[7].position = vec3(13.0f, -9.0f, 0);
    
    
    // My shots
    state.Myshots = new Entity[MYSHOTCOUNT];
    GLuint myshotTex = Util::LoadTexture("myshot.png");
    for (int i=0; i < MYSHOTCOUNT; i++){
        state.Myshots[i].selftype = MYSHOT;
        state.Myshots[i].active = false;
        state.Myshots[i].texture = myshotTex;
        state.Myshots[i].speed = 5.0f;
        state.Myshots[i].height = 0.5f;
        state.Myshots[i].width = 0.5f;
        state.Myshots[i].contactHeight = 0.2f;
        state.Myshots[i].contactWidth = 0.2f;
    }
    
    
    // Enemy shots
    GLuint badshotTex = Util::LoadTexture("badshot.png");
    state.Enemyshots = new Entity[BADSHOTCOUNT];
    for (int i=0; i < BADSHOTCOUNT; i++){
        state.Enemyshots[i].selftype = BADSHOT;
        state.Enemyshots[i].active = false;
        state.Enemyshots[i].texture = badshotTex;
        state.Enemyshots[i].speed = 3.5f;
        state.Enemyshots[i].height = 0.5f;
        state.Enemyshots[i].width = 0.5f;
        state.Enemyshots[i].contactHeight = 0.2f;
        state.Enemyshots[i].contactWidth = 0.2f;
    }
}


void Room3::Update(float &lastTick, float deltaTime, float animStep, float &animTime){
    // General Updates
        // Player Shots
    
    for (int i=0; i < MYSHOTCOUNT; i++){
        if (state.Myshots[i].active){
            state.Myshots[i].update(state.map, state.Enemy, ENEMYCOUNT, NULL, 0, NULL, 0, deltaTime);
        }
    }
    
        // Enemy Shots
    for (int i=0; i < BADSHOTCOUNT; i++){
        if (state.Enemyshots[i].active){
            state.Enemyshots[i].update(state.map, state.Player, 1, NULL, 0, NULL, 0, deltaTime);
        }
    }
        
        // Player/ Wand
    state.Player->update(state.map, state.Enemy, ENEMYCOUNT, NULL, 0, NULL, 0, deltaTime);
        
    state.PlayerWeapon->position = state.Player->position;
    
    state.PlayerWeapon->position.y -= 0.15f;
        
    if (state.Player->facingRight){
        state.PlayerWeapon->position.x += 0.2f;
        state.PlayerWeapon->angle = radians(-30.0f);
    }   else{
        state.PlayerWeapon->position.x -= 0.2f;
        state.PlayerWeapon->angle = radians(30.0f);
    }
        
    state.PlayerWeapon->update(NULL, NULL, 0, NULL, 0, NULL, 0, deltaTime);
    state.PlayerWeapon->facingRight = state.Player->facingRight;
        
        
        // Enemy
        
    for (int i = 0; i < ENEMYCOUNT; i++){
        if (state.Enemy[i].active){
            state.Enemy[i].update(state.map, state.Player, 1, state.Enemy, ENEMYCOUNT, state.Enemyshots, BADSHOTCOUNT, deltaTime);
        }
    }
        
       
    // Updating animation
    while (animTime >= animStep){
            
        for (int i= 0; i < ENEMYCOUNT; i++){
            if (state.Enemy[i].active){
                Util::AnimUpdate(&state.Enemy[i]);
            }
        }
                
        Util::AnimUpdate(state.Player);
                
        animTime -= animStep;
    }
}


void Room3::Render(ShaderProgram *program, GLuint fontTex, GLuint HPTex, bool &gameWon, bool &gameLost){
    // Rendering Map
    state.map->Render(program);
    
    // Rendering Enemy Shots
    for (int i=0; i < BADSHOTCOUNT; i++){
        if (state.Enemyshots[i].active){
            Util::CustomRender(program, 1, 1, state.Enemyshots[i].height, state.Enemyshots[i].width, &state.Enemyshots[i], 1, 1, 1);
        }
    }
    
    // Rendering enemies
    for (int i = 0; i < ENEMYCOUNT; i++){
        if (state.Enemy[i].active){
            Util::AnimRender(&state.Enemy[i], program);
        }
    }
    
    // Rendering Player Weapon
    Util::CustomRender(program, 1, 1, state.PlayerWeapon->height, state.PlayerWeapon->width, state.PlayerWeapon, 1, 1, 1);
    
    // Rendering Player Shots
    for (int i=0; i < MYSHOTCOUNT; i++){
        if (state.Myshots[i].active){
            Util::CustomRender(program, 1, 1, state.Myshots[i].height, state.Myshots[i].width, &state.Myshots[i], 1, 1, 1);
        }
    }
    
    // Rendering Player
    Util::AnimRender(state.Player, program);
    
    // Rendering pole tops so it covers player and enemies
    state.map->RenderPoleTops(program);
}


int Room3::EnemyCount(){
    return ENEMYCOUNT;
}
