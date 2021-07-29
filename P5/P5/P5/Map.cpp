#include "Map.h"

using namespace glm;

Map::Map(int width, int height, unsigned int *levelData, GLuint textureID, float tile_size, int tile_count_x, int tile_count_y): width(width), height(height), levelData(levelData), textureID(textureID), tile_size(tile_size), tile_count_x(tile_count_x), tile_count_y(tile_count_y)
{
    Build();
}

void Map::Build(){
    
    for(int y = 0; y < this->height; y++) {
        for(int x = 0; x < this->width; x++) {
            
            int tile = levelData[y * width + x];
            if (tile == 45) continue;
            
            float u = (float)(tile % tile_count_x) / (float)tile_count_x;
            float v = (float)(tile / tile_count_x) / (float)tile_count_y;
            
            float tileWidth = 1.0f/(float)tile_count_x;
            float tileHeight = 1.0f/(float)tile_count_y;
            
            float xoffset = -(tile_size / 2); // From center of tile
            float yoffset = (tile_size / 2); // From center of tile
            
            vertices.insert(vertices.end(), {
                xoffset + (tile_size * x),              yoffset + -tile_size * y,
                xoffset + (tile_size * x),              yoffset + (-tile_size * y) - tile_size,
                xoffset + (tile_size * x) + tile_size,  yoffset + (-tile_size * y) - tile_size,
                
                xoffset + (tile_size * x),              yoffset + -tile_size * y,
                xoffset + (tile_size * x) + tile_size,  yoffset + (-tile_size * y) - tile_size,
                xoffset + (tile_size * x) + tile_size,  yoffset + -tile_size * y
            });
            
            texCoords.insert(texCoords.end(), {
                u, v,
                u, v+(tileHeight),
                u+tileWidth, v+(tileHeight),
                
                u, v,
                u+tileWidth, v+(tileHeight),
                u+tileWidth, v
            });
            
            leftBound = 0 - (tile_size / 2);
            rightBound = (tile_size * width) - (tile_size / 2);
            topBound = 0 + (tile_size / 2);
            botBound = -(tile_size * height) + (tile_size / 2);

            endView = width * tile_size + xoffset - 5.0f;
        }
    }
}


void Map::Render(ShaderProgram *program)
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
     glDrawArrays(GL_TRIANGLES, 0, (int)vertices.size() / 2);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


bool Map::IsSolid(glm::vec3 position, float *penetration_x, float *penetration_y, glm::vec3 &spawn)
{
    *penetration_x = 0;
    *penetration_y = 0;
    
    if (position.x < leftBound || position.x > rightBound) return false;
    if (position.y > topBound || position.y < botBound) return false;
    
    int tile_x = floor((position.x + (tile_size / 2)) / tile_size);
    int tile_y = -ceil((position.y - (tile_size / 2)) / tile_size); // Our array counts up as Y goes down.
    
    if (tile_x < 0 || tile_x >= width) return false;
    if (tile_y < 0 || tile_y >= height) return false;
    
    // Setting spawnpoints
    for (int i = 0; i < spawns.size(); i++){
        if (tile_x == spawns[i].x){
            spawn = vec3(tile_x * tile_size, -7.0f + spawns[i].y * tile_size, 0);
            break;
        }
    }
    
    int tile = levelData[tile_y * width + tile_x];
    if (tile == 45) return false;
        
    float tile_center_x = (tile_x * tile_size);
    float tile_center_y = -(tile_y * tile_size);
    
    *penetration_x = (tile_size / 2) - fabs(position.x - tile_center_x) + 0.01f;
    *penetration_y = (tile_size / 2) - fabs(position.y - tile_center_y);
    
    return true;
}


void Map::setSpawn(std::vector<glm::vec3> spawn){
    spawns = spawn;
}


float Map::getEnd(){
    return endView;
}
