#define GL_SILENCE_DEPRECATION
#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include "Map.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <vector>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


enum EntityType {PLAYER, CHASER, SHOOTER, BOSS, WEAPON, MYSHOT, BADSHOT, NONE};

class Entity{
public:
    explicit Entity(EntityType obtype = NONE, GLuint texture = NULL, int cols = 1, int rows = 1, float spriteheight = 1, float spritewidth = 1);
    bool checkCollision(Entity* other);
    void checkCollisionx(Map *map);
    void checkCollisiony(Map *map);
    void checkCollisionx(Entity *objects, int objectcount);
    void checkCollisiony(Entity *objects, int objectcount);
    void damaged();
    void update(Map* map, Entity *lives, int numOfLives, Entity *friends, int numFriends, Entity *bullets, int bulletcount, float deltaTime);
    void render(ShaderProgram* program);
    void AIMover();
    void AIChaser(Entity *player);
    void AIShooter(Entity *player, Entity *bullets, int maxbullets);
    void AIBoss(Entity *player, Entity *bullets, int maxbullets);
    
    
    EntityType selftype;
    bool collidedTop = false;
    bool collidedBot = false;
    bool collidedRight = false;
    bool collidedLeft = false;
    GLuint texture;
    int xRepeat = 1;
    int yRepeat = 1;
    int life = 3;
    float height = 1.0f;
    float contactHeight = 1.0f;
    float width = 1.0f;
    float contactWidth = 1.0f;
    float speed = 0;
    float maxSpeed = speed;
    float angle = 0;
    
    bool texCentered = true;
    bool hit = false;
    bool facingRight = true;
    bool dead = false;
    bool active = true;
    
    
    int cols;
    int rows;
    float spriteheight;
    float spritewidth;
    int hitcount = 0;
    int invinceframe = 1;
    int idlecount = 0;
    std::vector<int> idleTex;
    int movecount = 0;
    std::vector<int> moveTex;
    int deathcount = 0;
    std::vector<int> deathTex;
    
    float attackCooldown = 0.5f;
    float cooldownCounter = attackCooldown;
    
    Mix_Chunk* running;
    Mix_Chunk* hurt;
    
    
    glm::vec3 position = glm::vec3(0);
    glm::vec3 acceleration = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 movement = glm::vec3(0);
    glm::mat4 matrix = glm::mat4(1.0);
};
