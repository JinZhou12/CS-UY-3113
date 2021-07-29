
#include "Entity.h"

class Entity;
using namespace glm;
using namespace std;


Entity::Entity(EntityType obtype, GLuint texture, int cols, int rows, float spriteheight, float spritewidth):
    selftype(obtype), texture(texture), cols(cols), rows(rows), spriteheight(spriteheight), spritewidth(spritewidth){}

void Entity::damaged(){
    life -= 1;
    if (life == 0){
        dead = true;
    }   else{
        hit = true;
    }
}


// Checking whether two objects are colliding
bool Entity::checkCollision(Entity* other){
    if (this == other){
        return false;
    }
    float checky = fabs(position.y - other->position.y) - (contactHeight* height* yRepeat* spriteheight + other->contactHeight* other->height* other->yRepeat* other->spriteheight) / 2;
    float checkx = fabs(position.x - other->position.x) - (contactWidth* width* xRepeat* spritewidth + other->contactWidth* other->width* other->xRepeat* other->spritewidth) / 2;
    if (checkx < 0 and checky < 0){
        if (selftype == PLAYER && other->selftype == GOAL){
            atGoal = true;
        }
        return true;
    }
    return false;
}

// Displacement when there is a collision in the x direction
void Entity::checkCollisionx(Entity *objects, int objectcount){
    
    for (int i=0; i < objectcount; i++){
        if (objects[i].life > 0){
            Entity* object = &objects[i];
            if (checkCollision(object)){
                if (selftype == ENEMY){
                    if (velocity.x>0){
                        if (!object->hit){
                            object->damaged();
                        }
                    }   else if (velocity.x<0){
                        if (!object->hit){
                            object->damaged();
                        }
                    }
                }
            }
        }
    }
    
}
    
// Displacement when there is a collision in the y direction
void Entity::checkCollisiony(Entity *objects, int objectcount){

    for (int i=0; i < objectcount; i++){
        if (objects[i].life > 0){
            Entity* object = &objects[i];
            if (checkCollision(object)){
                
                float checky = fabs(position.y - object->position.y) - (contactHeight* height* yRepeat* spriteheight + object->contactHeight* object->height* object->yRepeat* object->spriteheight) / 2;
                
                if (velocity.y<0){
                    if (selftype == PLAYER){
                        if (checky > -0.1f && object->selftype == ENEMY){
                            object->damaged();
                        }
                    }   else if (selftype == ENEMY){
                        if (!object->hit){
                            object->damaged();
                        }
                    }
                }
            }
        }
    }
}


void Entity::checkCollisionx(Map *map){
    
    vec3 right = vec3(position.x + width*contactWidth*spritewidth/2, position.y, position.z);
    vec3 left = vec3(position.x - width*contactWidth*spritewidth/2, position.y, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    vec3 spawn = spawnpoint;
    
    if (map->IsSolid(right, &penetration_x, &penetration_y, spawn) && velocity.x > 0){
        position.x -= penetration_x;
        collidedRight = true;
    }
    
    if (map->IsSolid(left, &penetration_x, &penetration_y, spawn) && velocity.x < 0){
        position.x += penetration_x;
        collidedLeft = true;
    }
    
    spawnpoint = spawn;
}


void Entity::checkCollisiony(Map *map){
    vec3 top = vec3(position.x, position.y + height*contactHeight*spriteheight/2, position.z);
    vec3 top_left = vec3(position.x - width*contactWidth*spritewidth/2, position.y + height*contactHeight*spriteheight/2, position.z);
    vec3 top_right = vec3(position.x + width*contactWidth*spritewidth/2, position.y + height*contactHeight*spriteheight/2, position.z);
    
    vec3 bot = vec3(position.x, position.y - height*contactHeight*spriteheight/2, position.z);
    vec3 bot_left = vec3(position.x - width*contactWidth*spritewidth/2, position.y - height*contactHeight*spriteheight/2, position.z);
    vec3 bot_right = vec3(position.x + width*contactWidth*spritewidth/2, position.y - height*contactHeight*spriteheight/2, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    vec3 spawn = vec3(0);
    
    if (map->IsSolid(top, &penetration_x, &penetration_y, spawn) && velocity.y > 0){
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }   else if (map->IsSolid(top_left, &penetration_x, &penetration_y, spawn) && velocity.y > 0){
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }   else if (map->IsSolid(top_right, &penetration_x, &penetration_y, spawn) && velocity.y > 0){
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    
    if (map->IsSolid(bot, &penetration_x, &penetration_y, spawn) && velocity.y < 0){
        position.y += penetration_y;
        velocity.y = 0;
        collidedBot = true;
    }   else if (map->IsSolid(bot_left, &penetration_x, &penetration_y, spawn) && velocity.y < 0){
        position.y += penetration_y;
        velocity.y = 0;
        collidedBot = true;
    }   else if (map->IsSolid(bot_right, &penetration_x, &penetration_y, spawn) && velocity.y < 0){
        position.y += penetration_y;
        velocity.y = 0;
        collidedBot = true;
    }
}



// Updating positions of everything accordingly
void Entity::update(Map* map, Entity *goal, Entity *lives, int numOfLives, float deltaTime){
    
    collidedTop = false;
    collidedBot = false;
    collidedRight = false;
    collidedLeft = false;
    
    if (selftype == ENEMY){
        if (active){
            AIMover(map);
        }
    }
    
    // Different effects from movement on velocity depending on acceleration
    if (acceleration.x == 0){
        velocity.x = movement.x * speed;
    }   else{
        velocity.x += movement.x * speed;
    }
    
    if (acceleration.y == 0){
        velocity.y = movement.y * speed;
    }   else{
        velocity.y += movement.y * speed;
    }
    
    velocity += acceleration * deltaTime;
    
    // Collision detection
    
    if (selftype != BACKGROUND){
        if (life > 0){
            position.x += velocity.x * deltaTime;
            checkCollisionx(map);
            checkCollisionx(lives, numOfLives);
            if (selftype == PLAYER){
                checkCollisionx(goal,1);
            }   else if (selftype == ENEMY && (collidedLeft || collidedRight)){
                movement.x = -movement.x;
            }
            
            position.y += velocity.y * deltaTime;
            checkCollisiony(map);
            checkCollisiony(lives, numOfLives);
            if (selftype == PLAYER){
                checkCollisiony(goal,1);
            }
        }
    }
    
    if (velocity.y < 0){
        if (!jump || (!doubleJump && jumpcount == jumpTex.size()-1) || doublecount == doubleTex.size()-1){
            fall = true;
        }
    }
        
    if (position.y <= -8.0f){
        if (!hit){
            damaged();
        }
        position = spawnpoint;
    }
    
    if (dead){
        velocity = vec3(0, -20.0f, 0);
    }   else{
    
        // Determining which side the entity is facing
        if (velocity.x < 0){
            facingRight = false;
        }   else{
            facingRight = true;
        }

        
        // Commiting the displacement change
        matrix = rotate(translate(mat4(1.0), position),angle, vec3(0,0,1));
        
        
        // Status change based on collision
        if (collidedBot){
            onGround = true;
            fall = false;
            jump = false;
            doubleJump = false;
            jumpcount = 0;
            doublecount = 0;
            fallcount = 0;
        }   else{
            onGround = false;
        }
        
    }
}

// Rendering the entity
void Entity::render(ShaderProgram* program){
    program->SetModelMatrix(matrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Entity::AIMover(Map* map){
    vec3 bot_left_sensor = vec3(position.x - width*contactWidth*spritewidth/2 - 0.1f, position.y - height*contactHeight*spriteheight/2, position.z);
    vec3 bot_right_sensor = vec3(position.x + width*contactWidth*spritewidth/2 + 0.1f, position.y - height*contactHeight*spriteheight/2, position.z);
    
    float penetration_x=0;
    float penetration_y=0;
    vec3 spawn = vec3(0);
    
    if (movement.x < 0){
        if (!map->IsSolid(bot_left_sensor, &penetration_x, &penetration_y, spawn)){
            movement.x = 1.0f;
        }   else{
            movement.x = -1.0f;
        }
    }   else{
        if (!map->IsSolid(bot_right_sensor, &penetration_x, &penetration_y, spawn)){
            movement.x = -1.0f;
        }   else{
            movement.x = 1.0f;
        }
    }
}
