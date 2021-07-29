//
//  MainScreen.cpp
//  P5
//
//  Created by Jin Zhou on 7/28/21.
//

#include "MainScreen.h"

using namespace glm;

void MainScreen::Initialize(Entity *player){
    
    state.Background = new Entity(BACKGROUND, Util::LoadTexture("background.png"), 1, 1, 1.0f, 1.5f);
    state.Background->xRepeat = 2;
    state.Background->position = vec3(3.0f, 0, 0);
    state.Background->height = 7.5f;
    state.Background->width = 7.5f;
    
    state.music = Mix_LoadMUS("theme.mp3");
    Mix_PlayMusic(state.music, -1);
    
}


void MainScreen::Update(float& lastTick, float deltaTime, float animStep, float& animTime){
    
    state.Background->position.x -= 1.5f * deltaTime;
    if (state.Background->position.x < -6.0f){
        state.Background->position.x += 7.5f*1.5f;
    }
    state.Background->update(NULL, NULL, NULL, 0, deltaTime);
}


void MainScreen::Render(SDL_Window* displayWindow, ShaderProgram* program, GLuint fontTex, GLuint HPTex, bool& gameWon, bool& gameLost){
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    Util::CustomRender(program, state.Background->yRepeat, state.Background->xRepeat, state.Background->height, state.Background->width, state.Background, 1, 1, 1);
    
    Util::DrawText(program, fontTex, "Monster Slayer", 1, -0.6, vec3(-2.6f, 0.5f,0));
    Util::DrawText(program, fontTex, "Press Enter to Continue", 0.3, -0.15, vec3(-1.7f,-0.5f,0));
    
}
