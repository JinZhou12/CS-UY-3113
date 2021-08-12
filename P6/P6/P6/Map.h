#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


class Map{
    int width;
    int height;
    unsigned int *levelData;
    unsigned int *levelOverlay;
    
    GLuint textureID;
    float tile_size;
    int tile_count_x;
    int tile_count_y;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<float> poletopVertices;
    std::vector<float> poletopCoords;
    std::vector<int> Solids;
    
    float endViewX;
    float endViewY;
    
    bool finish = false;
    
    float leftBound, rightBound, topBound, botBound;
    
public:
    Map(int width, int height, unsigned int *levelData, unsigned int *levelOverlay, GLuint textureID, float tile_size, int tile_count_x, int tile_count_y);
    void Build();
    float getEndX();
    float getEndY();
    bool finished();
    void rebuild();
    void adjustMap(int target, int replacement);
    void setSolids(std::vector<int> solids);
    void Render(ShaderProgram *program);
    void RenderPoleTops(ShaderProgram *program);
    bool IsSolid(glm::vec3 position, bool bullet, float* penetration_x, float* penetration_y);
    
};
