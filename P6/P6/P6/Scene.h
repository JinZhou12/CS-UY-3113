#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include "Util.h"

enum SceneType{ MAIN, ROOM, EMPTY};

class Scene{
public:
    GameState state;
    Scene *nextScene = NULL;
    SceneType scenetype = EMPTY;
    virtual void Initialize(Entity *player) = 0;
    virtual void Update(float &lastTick, float deltaTime, float animStep, float &animTime) = 0;
    virtual void Render(ShaderProgram* program, GLuint fontTex, GLuint HPTex, bool& gameWon, bool& gameLost) = 0;
    virtual int EnemyCount() = 0;
};
