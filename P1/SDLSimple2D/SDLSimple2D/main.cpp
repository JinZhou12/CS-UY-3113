// Project 1: simple 2D
// Jin Zhou
// CSUY 3113

// it's not perfect, not sure how to compute the exact ball retrieve speed to make the pattern consistent,
// but it's ok for the first few minutes :)

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

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, player1Matrix, player2Matrix, ballMatrix, hoopMatrix, projectionMatrix;

GLuint player1Tex, player2Tex, ballTex, hoopTex;

GLuint LoadTexture(const char* filepath){
    int w, h, n;
    unsigned char* image = stbi_load(filepath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL){
        std::cout << "Unable to load image, check the filepath!\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Simple 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    
    player1Matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-3.0f, -2.0f, 0.0f));
    player2Matrix = glm::translate(glm::mat4(1.0f),glm::vec3(3.0f, -2.0f, 0.0f));
    ballMatrix = glm::translate(glm::scale(glm::mat4(1.0f),glm::vec3(0.5f, 0.5f, 1.0f)),glm::vec3(-3.5f, -2.0f, 0.0f));
    hoopMatrix = glm::translate(glm::scale(glm::mat4(1.0f),glm::vec3(2.0f,2.0f,1.0f)), glm::vec3(2.3f, 0.7f, 0.0f));
    
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(.79f, 0.56f, 0.26f, 1.0f);
    glEnable(GL_BLEND);
    
    //For transparency of the image background
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    player1Tex = LoadTexture("ctg.png");
    player2Tex = player1Tex;
    ballTex = LoadTexture("basketball.png");
    hoopTex = LoadTexture("hoop.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float player2_y = 0;
int sign = 1;
float ball_x = 0;
float ball_y = 0;
float ball_rotate = 0;
float rotate_angle = -90.0f;
float deltaBall_x = 7.2/3.3;
float deltaBall_y = 1.0f;
float ballRetrieveSpeed = 4.4333;

float lastTick = 0;

void Update() {
    // Calculating deltaTime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTick;
    lastTick = ticks;
    
    if (player2_y >= 3.3 and sign == 1){
        sign = -1;
    }
    else if(player2_y <= 0 and sign == -1){
        sign = 1;
    }
    // Player2 update
    player2_y += 1.0f * sign * deltaTime;
    
    player2Matrix = glm::translate(glm::mat4(1.0f),glm::vec3(3.0f, -2.0f, 0.0f));
    player2Matrix = glm::translate(player2Matrix, glm::vec3(0.0f, player2_y, 0.0f));
    
    // Ball update
    if (ball_y <= -1.5 and ball_x > 0){
        deltaBall_x = -ballRetrieveSpeed;
        deltaBall_y = 0;
        rotate_angle = 180.0f;
    }else if (ball_y <= -1.5 and ball_x <=0){
        deltaBall_x = 0;
        deltaBall_y = ballRetrieveSpeed;
        rotate_angle = 0;
    }else if (ball_x >= 7.2 and deltaBall_x != 0){
        deltaBall_x = 0;
        deltaBall_y = -ballRetrieveSpeed;
        rotate_angle = 0;
    }else if (ball_y >=0 and deltaBall_y != 1.3f and deltaBall_y > 0){
        deltaBall_x = 7.2/3.3;
        deltaBall_y = 1.3f;
        rotate_angle = -90.0f;
    }
    ball_x += deltaBall_x * deltaTime;
    ball_y += deltaBall_y * deltaTime;
    ball_rotate += rotate_angle * deltaTime;

    ballMatrix = glm::translate(glm::scale(glm::mat4(1.0f),glm::vec3(0.6f, 0.6f, 1.0f)),glm::vec3(-3.5f, -2.0f, 0.0f));
    ballMatrix = glm::translate(ballMatrix,glm::vec3(ball_x, ball_y, 0.0f));
    ballMatrix = glm::rotate(ballMatrix, glm::radians(ball_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Render() {
    // Initiating render
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    // Player1 render
    program.SetModelMatrix(player1Matrix);
    
    glBindTexture(GL_TEXTURE_2D, player1Tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Player1 render
    program.SetModelMatrix(player2Matrix);
    
    glBindTexture(GL_TEXTURE_2D, player2Tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Ball render
    program.SetModelMatrix(ballMatrix);
    
    glBindTexture(GL_TEXTURE_2D, ballTex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Hoop render
    program.SetModelMatrix(hoopMatrix);
    
    glBindTexture(GL_TEXTURE_2D, hoopTex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
