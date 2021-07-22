#define GL_SILENCE_DEPRECATION

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


enum EntityType {PLAYER, ENEMY, FLYENEMY, ENEMYBULLET, HEROBULLET, BACKGROUND, NONE};

class Entity{
public:
    explicit Entity(EntityType obtype = NONE, GLuint texture = NULL, int cols = 1, int rows = 1, float spriteheight = 1, float spritewidth = 1);
    bool checkCollision(Entity* other);
    void checkCollisionx(Entity *objects, int numobjects);
    void checkCollisiony(Entity *objects, int numobjects);
    void damaged();
    void update(Map* map, Entity *life, int numOfLives, float deltaTime);
    void render(ShaderProgram* program);
    
    
    EntityType selftype;
    EntityType lastcollidetop = NONE;
    EntityType lastcollidebot = NONE;
    EntityType lastcollideleft = NONE;
    EntityType lastcollideright = NONE;
    GLuint texture;
    int xRepeat = 1;
    int yRepeat = 1;
    int life = 3;
    float height = 1.0f;
    float contactHeight = 1.0f;
    float width = 1.0f;
    float contactWidth = 1.0f;
    float speed = 1.0f;
    float angle = 0;
    bool jump = false;
    bool fall = false;
    bool wallSlide = false;
    bool onGround = false;
    bool hit = false;
    bool facingRight = true;
    bool dead = false;
    
    
    int cols;
    int rows;
    float spriteheight;
    float spritewidth;
    int idlecount = 0;
    std::vector<int> idleTex;
    int movecount = 0;
    std::vector<int> moveTex;
    int jumpcount = 0;
    std::vector<int> jumpTex;
    int wallSlidecount = 0;
    std::vector<int> wallSlideTex;
    int fallcount = 0;
    std::vector<int> fallTex;
    int hitcount = 0;
    std::vector<int> hitTex;
    int deathcount = 0;
    std::vector<int> deathTex;

    
    
    Mix_Chunk* jumping;
    Mix_Chunk* running;
    Mix_Chunk* hurt;
    
    
    glm::vec3 position = glm::vec3(0);
    glm::vec3 acceleration = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 movement = glm::vec3(0);
    glm::mat4 matrix = glm::mat4(1.0);
};
