
#include "Effects.h"

using namespace glm;

Effects::Effects(mat4 projectionMatrix, mat4 viewMatrix){
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    currentEffect = NOTHING;
    
}


void Effects::DrawOverlay(){
    glUseProgram(program.programID);
    float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
}


void Effects::Start(EffectType effectType){
    
    currentEffect = effectType;
    
    switch (currentEffect) {
            
        case NOTHING:
            break;
            
        case FADEIN:
            alpha = 1.0f;
            break;
            
        case FADEOUT:
            alpha = 0;
            break;
    }
}


void Effects::Update(float deltaTime){
    
    switch (currentEffect) {
            
        case NOTHING:
            break;
            
        case FADEIN:
            alpha -= 1.0f * deltaTime;
            if (alpha <= 0){
                currentEffect = NOTHING;
                alpha = 0;
            }
            break;
            
        case FADEOUT:
            alpha += 1.0f * deltaTime;
            if (alpha >= 1.0f){
                currentEffect = NOTHING;
                alpha = 1.0f;
            }
            break;
    }
    
}


void Effects::Render(){
    
    mat4 modelMatrix = scale(translate(mat4(1.0f),vec3(5.0f, -3.5f, 0)), vec3(100.0f, 100.0f, 0));
    
    switch (currentEffect) {
            
        case NOTHING:
            break;
            
        case FADEIN:
            program.SetModelMatrix(modelMatrix);
            program.SetColor(0, 0, 0, alpha);
            DrawOverlay();
            break;
            
        case FADEOUT:
            program.SetModelMatrix(modelMatrix);
            program.SetColor(0, 0, 0, alpha);
            DrawOverlay();
            break;
    }
}
