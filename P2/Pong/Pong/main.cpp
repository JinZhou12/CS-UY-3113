
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

#include "vector"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

GLuint LoadTexture(const char* filepath);
void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position);

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gamestart = false;
bool gameOver = false;
bool bouncex = false;
bool bouncey = false;
bool speedUp = false;
bool spawned = false;
float spawn = 500;
float countdown = 600;

// One wins when he/she reaches a score of 10
int player1score = 0;
int player2score = 0;

glm::vec3 board1position = glm::vec3(-18.6f,0.0f,0.0f);
glm::vec3 board2position = glm::vec3(18.6f,0.0f,0.0f);
glm::vec3 ballposition = glm::vec3(0.0f,0.0f,0.0f);
glm::vec3 speedposition = glm::vec3(0.0f,0.0f,0.0f);

glm::vec3 board1_movement = glm::vec3(0.0f,0.0f,0.0f);
glm::vec3 board2_movement = glm::vec3(0.0f,0.0f,0.0f);
glm::vec3 ball_movement = glm::vec3(1.0f,0.0f,0.0f);

float boardspeed = 0.0f;
float ballspeed = 0.0f;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, ballMatrix, boardMatrix, board1Matrix, board2Matrix, speedMatrix, resultMatrix;
GLuint ballTex, boardTex, speedUpTex, result1Tex, result2Tex, fontTex;



void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    boardMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(.25f, 1.0f, 1.0f));
    ballMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(.25f, .25f, 1.0f));
    speedMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(.33f, .33f, 1.0f));
    resultMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(10.0f, 8.0f, 1.0f));

    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_BLEND);
    
    ballTex = LoadTexture("ball.png");
    boardTex = LoadTexture("board.png");
    speedUpTex = LoadTexture("speedup.png");
    result1Tex = LoadTexture("player1wins.png");
    result2Tex = LoadTexture("player2wins.png");
    fontTex = LoadTexture("font.png");
    
    //For transparency of the image background
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int x;

void ProcessInput() {
    SDL_Event event;
    board1_movement = glm::vec3(0);
    board2_movement = glm::vec3(0);
    
    // Start/ Restart/ Quit inputs
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                        
                    // Start/Continue the game
                    case SDLK_SPACE:
                        gamestart = true;
                        boardspeed = 5.0f;
                        ballspeed = 17.0f;
                        break;
                        
                    // Pausing the game
                    case SDLK_ESCAPE:
                        gamestart = false;
                        boardspeed = 0;
                        ballspeed = 0;
                        break;
                    
                    // Restart the game only when the entire game is over
                    case SDLK_r:
                        if (gameOver){
                            player1score = 0;
                            player2score = 0;
                            gameOver = false;
                            break;
                        }
                }
        }
    }
    
    // Movement inputs
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W] and board1position.y <= 10){
        board1_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S] and board1position.y >= -10){
        board1_movement.y = -1.0f;
    }
    
    if (keys[SDL_SCANCODE_UP] and board2position.y <= 10){
        board2_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] and board2position.y >= -10){
        board2_movement.y = -1.0f;
    }
}


float lastTick = 0;
float randx1, randx2, randx, randy1, randy2, randy;

void Update() {
    // Calculating deltaTime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTick;
    lastTick = ticks;
    
    // Limiting Board movement range
    if ((board1position.y >= 3.2 and board1_movement.y > 0) or (board1position.y <= -3.2 and board1_movement.y < 0)){
        board1_movement.y = 0;
    }
    if ((board2position.y >= 3.2 and board2_movement.y > 0) or (board2position.y <= -3.2 and board2_movement.y < 0)){
        board2_movement.y = 0;
    }
    
    // Checking if collision on board or edges
    if ((ballposition.y > 14.45 or ballposition.y < -14.45) and not bouncey){
        ball_movement.y = -ball_movement.y;
        bouncey = true;
    }   else{
        bouncey = false;
    }
    
    // if collide with board, flip y movement, and change x movement according to point collided on board
    if ((abs(ballposition.x - board1position.x) <= 1 and abs(ballposition.y - board1position.y*4) <= 2.5)
        or (abs(ballposition.x - board2position.x) <= 1 and abs(ballposition.y - board2position.y*4) <= 2.5)){
        if (not bouncex){
            if (abs(ballposition.x - board1position.x) <= 1){
                float bouncepoint = ballposition.y - board1position.y*4;
                float bounceangle = bouncepoint/4;
                ball_movement.y = bounceangle * 2.5;
            }   else if (abs(ballposition.x - board2position.x) <= 1){
                float bouncepoint = ballposition.y - board2position.y*4;
                float bounceangle = bouncepoint/4;
                ball_movement.y = bounceangle * 2.5;
            }
            ball_movement.x = -ball_movement.x;
        }
        // Limiting bounce on each board to avoid the ball to be stuck on one side
        bouncex = true;
    }
    else{
        bouncex = false;
    }
    
    // keeps track of score for both players and auto resets the board when one round finishes
    if (ballposition.x >= 20.2 or ballposition.x <= -20.2){
        if (ballposition.x >= 20.2){
            player1score += 1;
        } else{
            player2score += 1;
        }
        if (player1score == 10 or player2score == 10){
            gameOver = true;
        }
        bouncex = false;
        bouncey = false;
        speedUp = false;
        spawned = false;
        spawn = rand() % 500 + 500;
        countdown = 600;
        boardspeed = 5.0f;
        ballspeed = 17.0f;
        x = rand() % 3 - 1;
        while (x == 0){
            x = rand() % 3 - 1;
        }
        ball_movement = glm::vec3(x,0.0f,0.0f);
        board1position = glm::vec3(-18.6f,0.0f,0.0f);
        board2position = glm::vec3(18.6f,0.0f,0.0f);
        ballposition = glm::vec3(0.0f,0.0f,0.0f);
    }
    
    // Speed up blocks
    if (gamestart){
        if (not speedUp){
            spawn -= 1;
            // set random positon for the block from (-10,-15) to (10,15)
            if (spawn == 0 and not spawned){
                randx1 = rand() % 100 - 50;
                randx2 = rand() % 10 + 10;
                randx = randx1/randx2;
                randy1 = rand() % 120 - 60;
                randy2 = rand() % 10 + 10;
                randy = randy1/randy2;
                speedMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(.33f, .33f, 1.0f));
                speedposition = glm::vec3(randx,randy,0.0f);
                speedMatrix = glm::translate(speedMatrix, speedposition);
                spawned = true;
            }
        }
        // when ball collide with the block, reset a random spawn time and speed up the ball
        if (spawned and (abs(ballposition.x - speedposition.x*4/3) <= 1.3f and abs(ballposition.y - speedposition.y*4/3) <= 1.3f)){
            speedUp = true;
            spawned = false;
            spawn = rand() % 500 + 500;
        }
        // speeding up the ball, slow down after a while
        if (speedUp){
            ballspeed = 23.0f;
            countdown -= 1;
            if (countdown == 0){
                speedUp = false;
                ballspeed = 17.0f;
                countdown = 600;
            }
        }
    }
        
    // Updating object postions
    board1position += board1_movement * deltaTime * boardspeed;
    board2position += board2_movement * deltaTime * boardspeed;
    ballposition += ball_movement * deltaTime * ballspeed;
    
    // Commiting change
    board1Matrix = glm::translate(boardMatrix, board1position);
    board2Matrix = glm::translate(boardMatrix, board2position);
    ballMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(.25f, .25f, 1.0f));
    ballMatrix = glm::translate(ballMatrix, ballposition);
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
    
    if (player1score == 10){
        program.SetModelMatrix(resultMatrix);
        glBindTexture(GL_TEXTURE_2D, result1Tex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }   else if (player2score == 10){
        program.SetModelMatrix(resultMatrix);
        glBindTexture(GL_TEXTURE_2D, result2Tex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }   else{
    
        // Board1 render
        program.SetModelMatrix(board1Matrix);
        glBindTexture(GL_TEXTURE_2D, boardTex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    
        // Board2 render
        program.SetModelMatrix(board2Matrix);
        glBindTexture(GL_TEXTURE_2D, boardTex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    
        // Ball render
        program.SetModelMatrix(ballMatrix);
        glBindTexture(GL_TEXTURE_2D, ballTex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Speed Up render
        if (spawned){
            program.SetModelMatrix(speedMatrix);
            glBindTexture(GL_TEXTURE_2D, speedUpTex);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    
    DrawText(&program, fontTex, "Score: " + std::to_string(player1score), .3, -.1, glm::vec3(-4.7f, 3.45f, 0));
    DrawText(&program, fontTex, "Score: " + std::to_string(player2score), .3, -.1, glm::vec3(3.3f, 3.45f, 0));
    
    
    glDisableVertexAttribArray(program.positionAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}


int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        // Pause updating/rendering window until game restarted or continued
        if (not gameOver){
            Update();
            Render();
        }
    }
    
    Shutdown();
    return 0;
}


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



void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position){
    
    float width = 1.0f/ 16.0f;
    float height = 1.0f/ 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> textCoords;
    
    for (int i = 0; i < text.size(); i++){
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(),{
            offset +(-0.5f*size), 0.5f* size,
            offset +(-0.5f*size), -0.5f* size,
            offset +(0.5f*size), 0.5f* size,
            offset +(0.5f*size), -0.5f* size,
            offset +(0.5f*size), 0.5f* size,
            offset +(-0.5f*size), -0.5f* size
        });
        
        textCoords.insert(textCoords.end(),{
            u, v,
            u, v+height,
            u + width, v,
            u + width, v+height,
            u + width, v,
            u, v + height
        });
    }
    
    glm::mat4 modelMatrix = translate(glm::mat4(1),position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)text.size()*6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
