#pragma once
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include "Entity.h"

struct GameState{
    Mix_Music* music;
    Map* map;
    Entity* Player;
    Entity* Enemy;
    Entity* Background;
    Entity* Goal;
};

class Util{
public:
    static GLuint LoadTexture(const char* filepath);
    static void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position);
    static void CustomRender(ShaderProgram* program, float yRepeat, float xRepeat, float height, float width, Entity* object, int index, int cols, int rows, bool xFlip = false, bool yFlip = false);
    static void AnimUpdate(Entity* entity, float& animTime);
    static void AnimRender(Entity* entity, ShaderProgram* program);
    static void DisplayHealth(ShaderProgram* program, GameState& state, GLuint HPTex, glm::vec3 position);
};
