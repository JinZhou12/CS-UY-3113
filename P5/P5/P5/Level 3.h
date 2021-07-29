#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include "Scene.h"

class Level3 : public Scene{
    
public:
    void Initialize(Entity *player) override;
    void Update(float& lastTick, float deltaTime, float animStep, float& animTime) override;
    void Render(SDL_Window* displayWindow, ShaderProgram* program, GLuint fontTex, GLuint HPTex, bool& gameWon, bool& gameLost) override;
};
