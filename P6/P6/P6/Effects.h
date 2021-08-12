#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <vector>
#include <math.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


enum EffectType {NOTHING, FADEIN, FADEOUT};

class Effects{
    ShaderProgram program;
    float alpha = 0;
    EffectType currentEffect;
    
public:
    Effects(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
    void DrawOverlay();
    void Start(EffectType effectType);
    void Update(float deltaTime);
    void Render();
};
