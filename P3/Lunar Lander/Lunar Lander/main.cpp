

#include "Entity.h"
#include "vector"
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#define NumOfObjects 8
#define FIXEDTIMESTEP 0.016666666

struct GameState{
    Entity* rocket;
    Entity* dead;
    Entity* thrust;
    Entity* objects;
};

GameState state;
ShaderProgram program;
SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameOver = false;
bool gameWon = false;
bool pause = false;
bool thrust = false;
int fuel = 200000;
float thruststate = 0;
GLuint fontTex, thrustTex;


float lastTick = 0;
float deltaTime = 0;

using namespace glm;
using namespace std;


GLuint LoadTexture(const char* filepath);
void CustomRender(ShaderProgram* program, float yRepeat, float xRepeat, float height, float width, Entity* object);
void DrawText(ShaderProgram* program, GLuint fontTextureID, string text, float size, float spacing, vec3 position);


// Initialize the game
void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    mat4 viewMatrix = mat4(1.0f);
    mat4 projectionMatrix = ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_BLEND);

    //For transparency of the image background
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTex = LoadTexture("font.png");
    
    //player
    state.rocket = new Entity("Rocket",LoadTexture("rocket.png"));
    state.rocket->position = vec3(0,3.5f,0);
    state.rocket->width = 0.5;
    state.rocket->height = 0.5;
    state.rocket->contactWidth = 0.6;
    state.rocket->acceleration = vec3(0,-.5f,0);
    
    
    //visualization of boosting upwards
    state.thrust = new Entity("Thrust",LoadTexture("thrust.png"));
    state.thrust->width = 0.3;
    state.thrust->height = 0.3;
    
    
    //When the rocket dies
    state.dead = new Entity("Dead",LoadTexture("explosion.png"));
    state.dead->width = 0.7;
    state.dead->height = 0.7;
    
    
    //obstacles
    state.objects = new Entity[NumOfObjects];
    GLuint objectTex = LoadTexture("block.png");
    
    for (int i=0; i< NumOfObjects; i++){
        state.objects[i].selftype.type = "Block";
    }
    
    
        // stationary platforms
    state.objects[0].texture = objectTex;
    state.objects[0].height = 0.4f;
    state.objects[0].xRepeat = 2;
    state.objects[0].position = vec3(-3.5f,2.3f,0);
    
    
    state.objects[1].texture = objectTex;
    state.objects[1].height = 0.4f;
    state.objects[1].xRepeat = 3;
    state.objects[1].position = vec3(3.0f,-1.3f,0);
    
        // borders
    state.objects[2].position = vec3(0.0f,-3.7f,0);
    state.objects[2].xRepeat = 10;
    state.objects[2].texture = objectTex;
    
    state.objects[3].position = vec3(-5.0f,0.3f,0);
    state.objects[3].yRepeat = 8;
    state.objects[3].texture = objectTex;
    
    state.objects[4].position = vec3(5.0f,0.3f,0);
    state.objects[4].yRepeat = 8;
    state.objects[4].texture = objectTex;
    
        // moving platforms
    state.objects[5].selftype.type = "Mover";
    state.objects[5].height = 0.29f;
    state.objects[5].width = 1.4f;
    state.objects[5].speed = 1.0f;
    state.objects[5].bounce = true;
    state.objects[5].velocity.x = 1.5f;
    state.objects[5].texture = objectTex;
    state.objects[5].position = vec3(0,1.1f,0);
    
    state.objects[6].selftype.type = "Mover";
    state.objects[6].height = 0.29f;
    state.objects[6].width = 1.4f;
    state.objects[6].speed = 1.0f;
    state.objects[6].bounce = true;
    state.objects[6].velocity.x = -1.5f;
    state.objects[6].texture = objectTex;
    state.objects[6].position = vec3(0,-0.2f,0);
    
    
    //goal/landing platform
    state.objects[7].texture = LoadTexture("platform.png");
    state.objects[7].height = 0.19f;
    state.objects[7].width = 1.1f;
    state.objects[7].selftype.type = "Platform";
    state.objects[7].position = vec3(-3.95f,-3.1f,0);
}



void ProcessInput() {
    SDL_Event event;
    
    // Start/ Restart/ Quit inputs
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                        
                    // Pausing the game
                    case SDLK_ESCAPE:
                        if (pause){
                            pause = false;
                        }   else{
                            pause = true;
                        }
                    
                    // restart the game with the platform at a random spot on the floor when a game ended
                    case SDLK_r:
                        if (gameOver or gameWon){
                            gameOver = false;
                            gameWon = false;
                            state.rocket->position = vec3(0,3.5f,0);
                            state.rocket->velocity = vec3(0);
                            fuel = 2000;
                            state.objects[7].position = vec3((float)(rand() % 78 - 39) / (rand() % 10 + 10),-3.1f,0);
                        }
                }
        }
    }
    
    // Movement inputs
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (!gameOver and !gameWon and !pause){
        //thrust when space is held down
        if (keys[SDL_SCANCODE_SPACE] and fuel > 0){
            thrust = true;
            state.rocket->acceleration.y = 0.5f;
            fuel -= 1;
        }   else{
            thrust = false;
            state.rocket->acceleration.y = -.5f;
        }
        
        //give the rocket a side acceleration for A/D hold
        if (keys[SDL_SCANCODE_A] and fuel > 0){
            state.rocket->acceleration.x = -1.0f;
            fuel -= 1;
        }   else if (keys[SDL_SCANCODE_D] and fuel > 0){
            state.rocket->acceleration.x = 1.0f;
            fuel -= 1;
        }   else{
            state.rocket->acceleration.x = 0;
        }
    }
}



void Update() {
    
    // Calculating deltaTime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    deltaTime += ticks - lastTick;
    lastTick = ticks;
    
    while(deltaTime >= FIXEDTIMESTEP){
        
        // updating obstacles is not paused or game ended
        for (int i = 0; i < NumOfObjects; i++){
            if(state.objects[i].velocity != vec3(0)){
                if (!gameWon and !gameOver and !pause){
                    state.objects[i].update(state.objects,state.rocket,NumOfObjects,FIXEDTIMESTEP,gameOver,gameWon);
                }
            }   else{
                state.objects[i].update(NULL,NULL,0,FIXEDTIMESTEP,gameOver,gameWon);
            }
        }
        
        // updating rocket/player is not paused or game ended
        if (!gameWon && !gameOver && !pause){
            state.rocket->update(state.objects,NULL,NumOfObjects,FIXEDTIMESTEP,gameOver,gameWon);
            mat4 projectionMatrix = ortho(-5.0f+state.rocket->position.x,5.0f+state.rocket->position.x,-3.75f+state.rocket->position.y,3.75f + state.rocket->position.y,-1.0f,1.0f);
            program.SetProjectionMatrix(projectionMatrix);
        }
        
        // updating thrust
        if (thrust){
            
            if (thruststate < 1){
                thruststate += 1.0f * FIXEDTIMESTEP;
            }
            state.thrust->position = state.rocket->position;
            state.thrust->position.y -= 0.25;
            state.thrust->update(NULL, NULL, 0, FIXEDTIMESTEP, gameOver, gameWon);
            state.thrust->matrix = scale(rotate(state.thrust->matrix,radians(-90.0f),vec3(0,0,1.0f)),vec3(thruststate,thruststate,0));
            
        }   else{
            
            if(thruststate > 0){
                thruststate -= 1.3* FIXEDTIMESTEP;
            }   else{
                thruststate = 0;
            }
            state.thrust->position = state.rocket->position;
            state.thrust->position.y -= 0.25;
            state.thrust->update(NULL, NULL, 0, FIXEDTIMESTEP, gameOver, gameWon);
            state.thrust->matrix = scale(rotate(state.thrust->matrix,radians(-90.0f),vec3(0,0,1.0f)),vec3(thruststate,thruststate,0));
            
        }

        deltaTime -= FIXEDTIMESTEP;
        
        // updating needed when game is won or is lost
        if (gameOver or gameWon){
            thruststate = 0;
            if (gameOver){
                state.dead->position = state.rocket->position;
                state.dead->update(NULL, NULL, 0, FIXEDTIMESTEP, gameOver, gameWon);
            }
        }
    }
}


void Render() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    // rendering obstacles
    for (int i=0; i<NumOfObjects; i++){
        CustomRender(&program, state.objects[i].yRepeat , state.objects[i].xRepeat, state.objects[i].height , state.objects[i].width, &state.objects[i]);
    }
    
    // rendering thrust before rocket so that it appears behind the rocket
    if (thruststate > 0){
        CustomRender(&program, 1, 1, state.thrust->height, state.thrust->width, state.thrust);
    }
    
    // rendering of the player being either the rocket or the explostion depending on whether the game is lost
    if (gameOver){
        CustomRender(&program, 1, 1, state.dead->height, state.dead->width , state.dead);
    }   else{
        CustomRender(&program, 1, 1, state.rocket->height, state.rocket->width , state.rocket);
    }
    
    // rendering visible stats for the player to observe
    DrawText(&program, fontTex, "y velocity: " + to_string((int)round(state.rocket->velocity.y*100)), .3, -.1, vec3(state.rocket->position.x-4.7f, state.rocket->position.y+3.15f, 0));
    DrawText(&program, fontTex, "x velocity: " + to_string((int)round(state.rocket->velocity.x*100)), .3, -.1, vec3(state.rocket->position.x-4.7f, state.rocket->position.y+2.85f, 0));
    DrawText(&program, fontTex, "Fuel: " + to_string(fuel), .3, -.1, vec3(state.rocket->position.x-4.7f, state.rocket->position.y+3.45f, 0));
    
    // ending screen if game is won
    if (gameWon){
        DrawText(&program, fontTex, "You Won!", 1, -.5, vec3(state.rocket->position.x-1.7f,state.rocket->position.y+0.5f,0));
        DrawText(&program, fontTex, "Press R to restart", .3, -.1, vec3(state.rocket->position.x-1.7f,state.rocket->position.y-.15f,0));
    }
    
    // ending screen if game is lost
    if (gameOver){
        DrawText(&program, fontTex, "Game Over", 1, -.5, vec3(state.rocket->position.x-2.0f,state.rocket->position.y+0.5f,0));
        DrawText(&program, fontTex, "Press R to restart", .3, -.1, vec3(state.rocket->position.x-1.7f,state.rocket->position.y-0.15f,0));
    }

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



// Additional Functions
    // Function for importing texture
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
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    
    stbi_image_free(image);
    return textureID;
}

// Customized Render with repeats and size changes
void CustomRender(ShaderProgram* program, float yRepeat, float xRepeat, float height, float width, Entity* object){
    
    float vertices[]  = { -width*xRepeat/2, -height*yRepeat/2, width*xRepeat/2, -height*yRepeat/2, width*xRepeat/2, height*yRepeat/2, -width*xRepeat/2, -height*yRepeat/2, width*xRepeat/2, height*yRepeat/2, -width*xRepeat/2, height*yRepeat/2 };
    float texCoords[] = { 0.0, yRepeat, xRepeat, yRepeat, xRepeat, 0.0, 0.0, yRepeat, xRepeat, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    object->render(program);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


//Rendering Text
void DrawText(ShaderProgram* program, GLuint fontTextureID, string text, float size, float spacing, vec3 position){
    
    float width = 1.0f/ 16.0f;
    float height = 1.0f/ 16.0f;
    
    vector<float> vertices;
    vector<float> textCoords;
    
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
    
    mat4 modelMatrix = translate(mat4(1),position);
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
