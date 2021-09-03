
#include "Entity.h"

class Entity;
using namespace glm;
using namespace std;


Entity::Entity(EntityType obtype, GLuint texture, int cols, int rows, float spriteheight, float spritewidth):
    selftype(obtype), texture(texture), cols(cols), rows(rows), spriteheight(spriteheight), spritewidth(spritewidth){}

void Entity::damaged(){
    life -= 1;
    if (life <= 0){
        active = false;
    }   else{
        hit = true;
        if (selftype == CHASER || selftype == SHOOTER){
            speed -= 0.3f;
        }
    }
}


// Checking whether two objects are colliding
bool Entity::checkCollision(Entity* other){
    if (this == other){
        return false;
    }
    
    float myPositiony;
    float otherPositiony;
    
    if (!texCentered){
        myPositiony = (position.y - (1 - contactHeight)* height* yRepeat* spriteheight / 2);
    }   else{
        myPositiony = position.y;
    }
    
    if (!other->texCentered){
        otherPositiony = (other->position.y - (1 - other->contactHeight) * other->height* other->yRepeat* other->spriteheight / 2);
    }   else{
        otherPositiony = other->position.y;
    }
    
    float checky = fabs(myPositiony - otherPositiony) - (contactHeight* height* yRepeat* spriteheight + other->contactHeight* other->height* other->yRepeat* other->spriteheight) / 2;
    float checkx = fabs(position.x - other->position.x) - (contactWidth* width* xRepeat* spritewidth + other->contactWidth* other->width* other->xRepeat* other->spritewidth) / 2;
    if (checkx < 0 and checky < 0){
        if ((selftype == MYSHOT && (other->selftype == CHASER || other->selftype == SHOOTER || other->selftype == BOSS)) || (selftype == BADSHOT && other->selftype == PLAYER)){
            active = false;
            if (!other->hit){
                other->damaged();
            }
        }   else if ((selftype == CHASER || selftype == SHOOTER || selftype == BOSS) && other->selftype == PLAYER && !other->hit){
            other->damaged();
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
                
                if (selftype == CHASER || selftype == SHOOTER){
                    speed -= 0.25f * speed;
                }
                
                float penex = fabs(position.x - object->position.x) - (contactWidth* width* xRepeat* spritewidth + object->contactWidth* object->width* object->xRepeat* object->spritewidth) / 2 + 0.11f;
                
                if (velocity.x > 0){
                    position.x -= penex;
                } else if (velocity.x < 0){
                    position.x += penex;
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
                if (selftype == CHASER || selftype == SHOOTER){
                    speed -= 0.3f;
                }
                
                float myPositiony;
                float otherPositiony;
                
                if (!texCentered){
                    myPositiony = (position.y - (1 - contactHeight)* height* yRepeat* spriteheight / 2);
                }   else{
                    myPositiony = position.y;
                }
                
                if (!object->texCentered){
                    otherPositiony = (object->position.y - (1 - object->contactHeight) * object->height* object->yRepeat* object->spriteheight / 2);
                }   else{
                    otherPositiony = object->position.y;
                }
                
                float peney = fabs(myPositiony - otherPositiony) - (contactHeight* height* yRepeat* spriteheight + object->contactHeight* object->height* object->yRepeat* object->spriteheight) / 2 + 0.11f;
                
                if (velocity.y > 0){
                    position.y -= peney;
                } else if (velocity.y < 0){
                    position.y += peney;
                }
            }
        }
    }
}


void Entity::checkCollisionx(Map *map){
        
    float myPositiony;
        
    if (!texCentered){
        myPositiony = (position.y - (1 - contactHeight)* height* yRepeat* spriteheight / 2);
    }   else{
        myPositiony = position.y;
    }
    
    vec3 right = vec3(position.x + width*contactWidth*spritewidth/2, myPositiony, position.z);
    vec3 left = vec3(position.x - width*contactWidth*spritewidth/2, myPositiony, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    bool bullet = false;
    if (selftype == MYSHOT || selftype == BADSHOT){
        bullet = true;
    }
    
    if (map->IsSolid(right, bullet, &penetration_x, &penetration_y) && velocity.x > 0){
        position.x -= penetration_x;
        collidedRight = true;
    }
    
    if (map->IsSolid(left, bullet, &penetration_x, &penetration_y) && velocity.x < 0){
        position.x += penetration_x;
        collidedLeft = true;
    }
}


void Entity::checkCollisiony(Map *map){
        
    float myPositiony;
        
    if (!texCentered){
        myPositiony = (position.y - (1 - contactHeight)* height* yRepeat* spriteheight / 2);
    }   else{
        myPositiony = position.y;
    }
        
    vec3 top = vec3(position.x, myPositiony + contactHeight* height* yRepeat* spriteheight / 2, position.z);
    vec3 top_left = vec3(position.x - width*contactWidth*spritewidth/2, myPositiony + contactHeight* height* yRepeat* spriteheight / 2, position.z);
    vec3 top_right = vec3(position.x + width*contactWidth*spritewidth/2, myPositiony + contactHeight* height* yRepeat* spriteheight / 2, position.z);
    
    vec3 bot = vec3(position.x, myPositiony - contactHeight* height* yRepeat* spriteheight / 2, position.z);
    vec3 bot_left = vec3(position.x - width*contactWidth*spritewidth/2, myPositiony - contactHeight* height* yRepeat* spriteheight / 2, position.z);
    vec3 bot_right = vec3(position.x + width*contactWidth*spritewidth/2, myPositiony - contactHeight* height* yRepeat* spriteheight / 2, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    bool bullet = false;
    if (selftype == MYSHOT || selftype == BADSHOT){
        bullet = true;
    }
    
    if (map->IsSolid(top, bullet, &penetration_x, &penetration_y) && velocity.y > 0){
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }   else if (map->IsSolid(top_left, bullet, &penetration_x, &penetration_y) && velocity.y > 0){
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }   else if (map->IsSolid(top_right, bullet, &penetration_x, &penetration_y) && velocity.y > 0){
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    
    if (map->IsSolid(bot, bullet, &penetration_x, &penetration_y) && velocity.y < 0){
        position.y += penetration_y;
        velocity.y = 0;
        collidedBot = true;
    }   else if (map->IsSolid(bot_left, bullet, &penetration_x, &penetration_y) && velocity.y < 0){
        position.y += penetration_y;
        velocity.y = 0;
        collidedBot = true;
    }   else if (map->IsSolid(bot_right, bullet, &penetration_x, &penetration_y) && velocity.y < 0){
        position.y += penetration_y;
        velocity.y = 0;
        collidedBot = true;
    }
}



// Updating positions of everything accordingly
void Entity::update(Map* map, Entity *lives, int numOfLives, Entity *friends, int numFriends, Entity *bullets, int bulletcount, float deltaTime){
    
    collidedTop = false;
    collidedBot = false;
    collidedRight = false;
    collidedLeft = false;
    
    if (cooldownCounter < attackCooldown){
        cooldownCounter += deltaTime;
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
    if (selftype != WEAPON && active){
        if (life > 0){
            position.x += velocity.x * deltaTime;
            checkCollisionx(map);
            checkCollisionx(lives, numOfLives);
            checkCollisionx(friends, numFriends);
                
            position.y += velocity.y * deltaTime;
            checkCollisiony(map);
            checkCollisiony(lives, numOfLives);
            checkCollisiony(friends, numFriends);
        }
    }
    
    if (lives != NULL){
        if (selftype == CHASER){
            if (speed < maxSpeed){
                speed += deltaTime;
            }
            AIChaser(lives);
        }   else if (selftype == SHOOTER){
            if (speed < maxSpeed){
                speed += deltaTime;
            }
            AIShooter(lives, bullets, bulletcount);
        }   else if (selftype == BOSS){
            if (speed < maxSpeed){
                speed += deltaTime;
            }
            AIBoss(lives, bullets, bulletcount);
        }
    }
    
    if (selftype == MYSHOT || selftype == BADSHOT){
        if (collidedBot || collidedTop || collidedLeft || collidedRight){
            active = false;
        }
    }
        

    // Determining which side the entity is facing
    if (selftype != SHOOTER){
        if (velocity.x < 0){
            facingRight = false;
        }   else if (velocity.x > 0){
            facingRight = true;
        }
    }

    // Commiting the displacement change
    matrix = rotate(translate(mat4(1.0), position),angle, vec3(0,0,1));
        
}

// Rendering the entity
void Entity::render(ShaderProgram* program){
    program->SetModelMatrix(matrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Entity::AIChaser(Entity *player){
    movement = normalize(player->position - position);
}


void Entity::AIShooter(Entity *player, Entity *bullets, int maxbullets){
    
    if (player->position.x > position.x){
        facingRight = true;
    }   else if(player->position.x < position.x){
        facingRight = false;
    }
    
    if (distance(player->position, position) < 10.0f){
        
        if (cooldownCounter >= attackCooldown){
            cooldownCounter = 0;
            for (int i=0; i< maxbullets; i++){
                if (!bullets[i].active){
                    bullets[i].active = true;
                    vec3 myPosition = position;
                    if (!texCentered){
                        myPosition.y = (position.y - (1 - contactHeight)* height* yRepeat* spriteheight / 2);
                    }
                    bullets[i].position = myPosition;
                    bullets[i].movement = normalize(player->position - position);
                    break;
                }
            }
        }
        if (distance(player->position, position) < 3.0f){
            movement = -normalize(player->position - position);
        }   else if (distance(player->position, position) > 4.5f) {
            speed = 0;
            movement = vec3(0);
        }
    }
}


void Entity::AIBoss(Entity *player, Entity *bullets, int maxbullets){
    
    movement = normalize(player->position - position);
    
    if (cooldownCounter >= attackCooldown){
        cooldownCounter = 0;
        int bulletCounter = rand() % 6 + 15;
        for (int i=0; i< maxbullets; i++){
            if (!bullets[i].active){
                bullets[i].active = true;
                bullets[i].position = position;
                bullets[i].speed = (float)(rand() % 26 + 25) / (float)(rand() % 6 + 10);
                bullets[i].movement = normalize(player->position - position);
                bullets[i].movement.y += (float)(pow(-1,bulletCounter) * bulletCounter) / (float)(rand ()% 10 + 20);
                bullets[i].movement = normalize(bullets[i].movement);
                bulletCounter -= 1;
                if (bulletCounter <= 0){
                    break;
                }
            }
        }
    }
    
}
