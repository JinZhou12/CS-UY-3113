#include "Util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <SDL_image.h>

using namespace std;
using namespace glm;

GLuint Util::LoadTexture(const char *filepath){
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

void Util::CustomRender(ShaderProgram* program, float yRepeat, float xRepeat, float height, float width, Entity* object, int index, int cols, int rows, bool xFlip, bool yFlip){
    
    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;
    
    float wid = 1 / (float)cols;
    float heig = 1 / (float)rows;
    
    float texCoords[12];
    
    if (cols == 1 && rows == 1){
        float inputTexCoords[] = { 0.0, yRepeat, xRepeat, yRepeat, xRepeat, 0.0, 0.0, yRepeat, xRepeat, 0.0, 0.0, 0.0 };
        
        if (xFlip) {
            for (int i=0; i< 12; i++){
                if (i % 2 == 0){
                    inputTexCoords[i] = xRepeat - inputTexCoords[i];
                }
            }
        }
        
        if (yFlip){
            for (int i=0; i< 12; i++){
                if (i % 2 != 0){
                    inputTexCoords[i] = yRepeat - inputTexCoords[i];
                }
            }
        }
        
        for (int i=0; i< 12; i++){
            texCoords[i] = inputTexCoords[i];
        }
    }   else{
        float inputTexCoords[] = {u, v + heig, u + wid, v + heig, u + wid, v, u, v + heig, u + wid, v, u, v};
        
        if (xFlip) {
            for (int i=0; i< 12; i++){
                if (i % 2 == 0){
                    inputTexCoords[i] = (2 * u) + wid - inputTexCoords[i];
                }
            }
        }
        
        if (yFlip){
            for (int i=0; i< 12; i++){
                if (i % 2 != 0){
                    inputTexCoords[i] = (2 * v) + heig - inputTexCoords[i];
                }
            }
        }
        
        for (int i=0; i< 12; i++){
            texCoords[i] = inputTexCoords[i];
        }
    }
    
    float vertices[]  = { -width*xRepeat*object->spritewidth/2, -height*yRepeat*object->spriteheight/2, width*xRepeat*object->spritewidth/2, -height*yRepeat*object->spriteheight/2, width*xRepeat*object->spritewidth/2, height*yRepeat*object->spriteheight/2, -width*xRepeat*object->spritewidth/2, -height*yRepeat*object->spriteheight/2, width*xRepeat*object->spritewidth/2, height*yRepeat*object->spriteheight/2, -width*xRepeat*object->spritewidth/2, height*yRepeat*object->spriteheight/2 };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    object->render(program);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void Util::DrawText(ShaderProgram* program, GLuint fontTextureID, string text, float size, float spacing, vec3 position){
    
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


void Util::AnimUpdate(Entity* entity, float& animTime){
    
    if (entity->hit){
     
         if (entity->hitcount < 5){
             entity->hitcount += 1;
         }   else{
             entity->hitcount = 0;
             entity->hit = false;
         }
     
    }
    
    if (entity->dead){
        
        if (entity->deathcount < entity->deathTex.size()-1){
                entity->deathcount += 1;
        }   else{
            entity->active = false;
        }
        
    }   else if (entity->fall){
        
        if (entity->fallcount < entity->fallTex.size()-1){
            entity->fallcount += 1;
        }   else{
            entity->fallcount = 0;
        }
        
    }   else if (entity->doubleJump){
        
        if (entity->doublecount < entity->doubleTex.size()-1){
            entity->doublecount += 1;
        }
        
    }   else if (entity->jump){
            
            if (entity->jumpcount < entity->jumpTex.size()-1){
                entity->jumpcount += 1;
            }
            
    }   else {
            
        if (entity->movement.x == 0 ){
                    
            if (entity->idlecount < entity->idleTex.size()-1){
                entity->idlecount += 1;
            }   else{
                entity->idlecount = 0;
                }
                   
        }   else{
                entity->idlecount = 0;
        }
            
        if (entity->movement.x != 0){
                
            if (entity->movecount < entity->moveTex.size()-1){
                entity->movecount += 1;
            }   else{
                entity->movecount = 0;
            }
                  
        }   else{
                entity->movecount = 0;
        }
    }
}


void Util::AnimRender(Entity* entity, ShaderProgram* program){
    
    if (entity->dead && !entity->deathTex.empty()){
    
        if (entity->facingRight ){
            Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->deathTex[entity->deathcount], entity->cols, entity->rows);
        }   else{
            Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->deathTex[entity->deathcount], entity->cols, entity->rows, true);
        }
        
    }   else if (!entity->hit || entity->hitcount % 2 == 0){
    
        if(entity->fall && !entity->fallTex.empty()){
            
            if (entity->facingRight){
                Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->fallTex[entity->fallcount], entity->cols, entity->rows);
            }   else{
                Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->fallTex[entity->fallcount], entity->cols, entity->rows, true);
            }
            
        }   else if (entity->doubleJump){
            
            if (entity->facingRight){
                Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->doubleTex[entity->doublecount], entity->cols, entity->rows);
            }   else{
                Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->doubleTex[entity->doublecount], entity->cols, entity->rows, true);
            }
            
        }   else if (entity->jump && !entity->jumpTex.empty()){
            
            if (entity->facingRight){
                Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->jumpTex[entity->jumpcount], entity->cols, entity->rows);
            }   else{
                Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->jumpTex[entity->jumpcount], entity->cols, entity->rows, true);
            }
            
        }   else{
                
            if (entity->velocity.x == 0 && !entity->idleTex.empty()){
                    
                if (entity->facingRight){
                    Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->idleTex[entity->idlecount], entity->cols, entity->rows);
                }   else{
                    Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->idleTex[entity->idlecount], entity->cols, entity->rows, true);
                }
                    
            }
                
            if (entity->movement.x != 0 && !entity->moveTex.empty()){
                    
                if (entity->facingRight){
                    Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->moveTex[entity->movecount], entity->cols, entity->rows);
                }   else{
                    Util::CustomRender(program, entity->yRepeat, entity->xRepeat, entity->height, entity->width, entity, entity->moveTex[entity->movecount], entity->cols, entity->rows, true);
                }
                    
            }
        }
    }
}


void Util::DisplayHealth(ShaderProgram* program, GameState& state, GLuint HPTex, vec3 position){
    
    vector<float> vertices;
    vector<float> texCoords;
    float spacing = 0.2f;
    float size = 0.7f;
    
    for (int i = 0 ; i < state.Player->life; i++){
        float offset = (size + spacing) * i;
        
        vertices.insert(vertices.end(),
        {
            offset +(-0.5f*size), -0.5f* size,
            offset +(0.5f*size), -0.5f* size,
            offset +(0.5f*size), 0.5f* size,
            offset +(-0.5f*size), -0.5f* size,
            offset +(0.5f*size), 0.5f* size,
            offset +(-0.5f*size), 0.5f* size
        });
        
        texCoords.insert(texCoords.end(), {0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0});
    }
    
    mat4 modelMatrix = translate(mat4(1), position);
    
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, HPTex);
    glDrawArrays(GL_TRIANGLES, 0, (int)state.Player->life*6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
