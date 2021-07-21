
#include "Entity.h"

class Entity;
using namespace glm;
using namespace std;


Entity::Entity(EntityType obtype, GLuint texture, int cols, int rows, float spriteheight, float spritewidth):
    selftype(obtype), texture(texture), cols(cols), rows(rows), spriteheight(spriteheight), spritewidth(spritewidth){}

void Entity::damaged(){
    hit = true;
    life -= 1;
    cast = false;
    attack = false;
    attacked = false;
    attackcount = 0;
}


// Checking whether two objects are colliding
bool Entity::checkCollision(Entity* other){
    if (this == other){
        return false;
    }
    float checky = fabs(position.y - other->position.y) - (contactHeight* height* yRepeat* spriteheight + other->contactHeight* other->height* other->yRepeat* other->spriteheight) / 2;
    float checkx = fabs(position.x - other->position.x) - (contactWidth* width* xRepeat* spritewidth + other->contactWidth* other->width* other->xRepeat* other->spritewidth) / 2;
    if (checkx < 0 and checky < 0){
        //
        if ((selftype == ENEMYBULLET && (other->selftype == PLAYER || other->selftype == PLATFORM || other->selftype == BORDER)) ||
            (selftype == HEROBULLET && (other->selftype == ENEMY || other->selftype == FLYENEMY || other->selftype == PLATFORM || other->selftype == BORDER))){
            if (selftype == HEROBULLET){
                dead = true;
            }   else{
                if (other->selftype != PLATFORM){
                    dead = true;
                }
            }
            
            if (other->selftype != PLATFORM || other->selftype != BORDER){
                other->damaged();
            }
        }
        return true;
    }
    return false;
}

// Displacement when there is a collision in the x direction
void Entity::checkCollisionx(Entity *objects, int numobjects){
    
    lastcollideleft = NONE;
    lastcollideright = NONE;
    
    for (int i=0; i < numobjects; i++){
        Entity* object = &objects[i];
        if (checkCollision(object)){
            
            float xPene = fabs(fabs(position.x - object->position.x) - (contactWidth* width* xRepeat* spritewidth + object->contactWidth* object->width* object->xRepeat*object->spritewidth) / 2);
            
            if (!(selftype == ENEMYBULLET && object->selftype == PLATFORM)){
                if (velocity.x>0){
                    lastcollideright = object->selftype;
                    position.x -= xPene;
                }   else if (velocity.x<0){
                    lastcollideleft = object->selftype;
                    position.x += xPene;
                }
            }
        }  
    }
}
    
// Displacement when there is a collision in the y direction
void Entity::checkCollisiony(Entity *objects, int numobjects){
    
    lastcollidebot = NONE;
    lastcollidetop = NONE;
    
    for (int i=0; i < numobjects; i++){
        Entity* object = &objects[i];
        if (checkCollision(object)){
            float yPene = fabs(fabs(position.y - object->position.y) - (contactHeight* height* yRepeat* spriteheight + object->contactHeight* object->height* object->yRepeat* object-> spriteheight) / 2);
            
            if (!(selftype == ENEMYBULLET && object->selftype == PLATFORM)){
                if (velocity.y>0){
                    lastcollidetop = object->selftype;
                    position.y -= yPene;
                }   else if (velocity.y<0){
                    lastcollidebot = object->selftype;
                    position.y += yPene;
                }
                velocity.y = 0;
            }
        }
    }
}


void Entity::checkAttack(Entity *other, int numobjects){
    for (int i = 0; i < numobjects; i++){
        if (airAttack){
            
            float checkx = fabs(position.x - other[i].position.x) - (contactWidth * width* xRepeat* spritewidth + other[i].contactWidth* other[i].width* other[i].xRepeat* other[i].spritewidth) / 2;
            float checky = fabs(position.y - other[i].position.y) - (contactHeight * height* yRepeat* spriteheight + other[i].contactHeight* other[i].height* other[i].yRepeat* other[i].spriteheight) / 2;
            
            if (checkx <= 0 && checky <= 0){
                attacked = true;
                other[i].damaged();
            }

        }   else if ((facingRight && other[i].position.x > position.x) || (!facingRight && other[i].position.x < position.x)){
            
            float checkx = fabs(position.x - other[i].position.x) - (contactWidth * attackdisx * width* xRepeat* spritewidth + other[i].contactWidth* other[i].width* other[i].xRepeat* other[i].spritewidth) / 2;
            float checky = fabs(position.y - other[i].position.y) - (contactHeight * attackdisy * height* yRepeat* spriteheight + other[i].contactHeight* other[i].height* other[i].yRepeat* other[i].spriteheight) / 2;
            
            if (checkx <= 0 && checky <= 0){
                attacked = true;
                other[i].damaged();
            }
            
        }
    }
}


// Updating positions of everything accordingly
void Entity::update(Entity* platform, Entity *life, int numOfEntities, int numOfLives, float deltaTime){
    
    if (airAttack){
        if (selftype == PLAYER){
            acceleration.y = -30.0f;
        }
    }   else {
        if (selftype == PLAYER){
            acceleration.y = -8.0f;
        }
    }
    
    if (attack || airAttack || hit || cast){
        movement = vec3(0);
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
    
    if (dead){
        velocity = vec3(0, -15.0f, 0);
    }
    
    
    // Determining which side the entity is facing
    if (velocity.x < 0 && !attack){
        facingRight = false;
    }   else if (velocity.x > 0 && !attack){
        facingRight = true;
    }
    
    // Determining whether the entity is falling
    if(!jump && velocity.y < 0){
        if (selftype != FLYENEMY){
            fall = true;
        }
    }

    // Collision detection
    position.y += velocity.y * deltaTime;
    if (selftype == HEROBULLET || selftype == ENEMYBULLET){
        checkCollisiony(life, numOfLives);
    }
    checkCollisiony(platform, numOfEntities);
    
    
    position.x += velocity.x * deltaTime;
    if (selftype == HEROBULLET || selftype == ENEMYBULLET){
        checkCollisiony(life, numOfLives);
    }
    checkCollisionx(platform, numOfEntities);
    
    
    // Check attack hitbox
    if (!attacked && (((attack && (attackcount >= validattack)) || airAttack))){
        if (selftype == PLAYER || selftype == ENEMY || selftype == FLYENEMY){
            checkAttack(life, numOfLives);
        }
    }
    
    // Commiting the displacement change
    matrix = rotate(translate(mat4(1.0), position),angle, vec3(0,0,1));
    
    
    // Status change based on collision
    if (lastcollidebot == PLATFORM || lastcollidebot == BORDER){
        onGround = true;
        fall = false;
        wallSlide = false;
        jump = false;
        if (airAttack){
            airAttack = false;
            attacked = false;
        }
        jumpcount = 0;
        fallcount = 0;
        airAttackcount = 0;
        wallSlidecount = 0;
    }   else{
        onGround = false;
    }
    
    
    if (lastcollideright == NONE && lastcollideleft == NONE){
        if (selftype == PLAYER){
            acceleration.y = -8.0f;
            wallSlide = false;
        }
        
    }   else if( !onGround && (lastcollideright != NONE || lastcollideleft != NONE)){
        if (selftype == PLAYER){
            
            fall = false;
            wallSlide = true;
            
            if (velocity.y > 0){
                acceleration.y = -12.0f;
            }   else{
                acceleration.y = -8.0f;
                velocity.y = -1.0f;
            }
            
        }
    }
    
    
}

// Rendering the entity
void Entity::render(ShaderProgram* program){
    program->SetModelMatrix(matrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
