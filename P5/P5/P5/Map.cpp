#include "Map.h"

using namespace glm;

Map::Map(int width, int height, unsigned int *levelData, GLuint textureID, float tile_size, int tile_count_x, int tile_count_y): width(width), height(height), levelData(levelData), textureID(textureID), tile_size(tile_size), tile_count_x(tile_count_x), tile_count_y(tile_count_y)
{
    Build();
}

void Map::Build(){
    
    for(int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
            
            int tile = levelData[y*width + x];
            if (tile == 45) continue;
            
            float u = (float)(tile % tile_count_x)/ (float)tile_count_x;
            float v = (float)(tile / tile_count_x)/ (float)tile_count_y;
            
            float tileWidth = 1.0f/ (float)tile_count_x;
            float tileHeight = 1.0f/ (float)tile_count_y;
            
            float xOffset = -(tile_size / 2);
            float yOffset = tile_size / 2;
            
            vertices.insert(vertices.end(),
            {xOffset + tile_size * x,               yOffset - tile_size * y,
             xOffset + tile_size * x,               yOffset - tile_size * y - tile_size,
             xOffset + tile_size * x + tile_size,   yOffset - tile_size * y - tile_size,
                
             xOffset + tile_size * x,               yOffset - tile_size * y,
             xOffset + tile_size * x + tile_size,   yOffset - tile_size * y - tile_size,
             xOffset + tile_size * x + tile_size,   yOffset - tile_size * y
            });
            
            texCoords.insert(texCoords.end(),
            {u, v,
             u, v + tileHeight,
             u + tileWidth, v + tileHeight,
                
             u, v,
             u + tileWidth, v+ tileHeight,
             u + tileWidth, v
            });
        }
    }
    
    leftBound = -tile_size / 2;
    rightBound = (tile_size * width) - tile_size / 2;
    
    topBound = tile_size / 2;
    botBound = -(tile_size * height) + tile_size / 2;
}


void Map::Render(ShaderProgram *program){
    mat4 modelMatrix = mat4(1.0f);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)vertices.size());
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

