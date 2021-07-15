#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


class Entity{
public:
    
    struct Entitytype{
        explicit Entitytype(std::string obtype);
        std::string type;
    };
    
    explicit Entity(std::string obtype = "None", GLuint texture = NULL);
    bool checkCollision(Entity* other);
    void checkCollisionx(Entity *objects, int numobjects,  bool& gameOver, bool& gameWon, float deltaTime);
    void checkCollisiony(Entity *objects, int numobjects,  bool& gameOver, bool& gameWon);
    void update(Entity* other, Entity* player, int numOfEntities, float deltaTime, bool& gameOver, bool& gameWon);
    void render(ShaderProgram* program);
    
    Entitytype selftype;
    GLuint texture;
    int xRepeat = 1;
    int yRepeat = 1;
    float height = 1.0f;
    float contactHeight = 1.0f;
    float width = 1.0f;
    float contactWidth = 1.0f;
    float speed = 1.0f;
    bool bounce = false;
    
    glm::vec3 position;
    glm::vec3 acceleration = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 movement = glm::vec3(0);
    glm::mat4 matrix = glm::mat4(1.0);
};
