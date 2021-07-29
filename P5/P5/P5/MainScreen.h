#pragma once

#include "Scene.h"


class MainScreen: public Scene{
public:
    void Initialize(Entity *player) override;
    void Update(float& lastTick, float deltaTime, float animStep, float& animTime) override;
    void Render(SDL_Window* displayWindow, ShaderProgram* program, GLuint fontTex, GLuint HPTex, bool& gameWon, bool& gameLost) override;
};
