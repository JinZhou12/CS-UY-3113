
#include "Entity.h"

class Entity;
using namespace glm;
using namespace std;

Entity::Entitytype::Entitytype(string obtype):type(obtype){}

Entity::Entity(string obtype, GLuint texture): selftype(Entitytype{obtype}), texture(texture){}

// Checking whether two objects are colliding
bool Entity::checkCollision(Entity* other){
    if (this == other){
        return false;
    }
    float checky = fabs(position.y - other->position.y) - (contactHeight* height* yRepeat + other->contactHeight* other->height* other->yRepeat) / 2;
    float checkx = fabs(position.x - other->position.x) - (contactWidth* width* xRepeat + other->contactWidth* other->width* other->xRepeat) / 2;
    if (checkx < 0 and checky < 0){
        return true;
    }
    return false;
}

// Displacement when there is a collision in the x direction
void Entity::checkCollisionx(Entity *objects, int numobjects, bool& gameOver, bool& gameWon, float deltaTime){
    for (int i=0; i < numobjects; i++){
        Entity* object = &objects[i];
        if (checkCollision(object)){
            
            float xPene = fabs(fabs(position.x - object->position.x) - (contactWidth* width* xRepeat + object->contactWidth* object->width* object->xRepeat) / 2);
            
            // Some displacement to make sure the rocket doesn't teleport to the top of the platform everytime when makeing contact with it in the direction it's moving
            if (velocity.x>0){
                if (selftype.type == "Mover" and object->selftype.type == "Rocket" and object->position.x > position.x){
                    object->position.x += (velocity.x) * deltaTime;
                    //gameOver = true;
                }   else{
                    position.x -= xPene;
                }
            }   else if (velocity.x<0){
                if (selftype.type == "Mover" and object->selftype.type == "Rocket" and object->position.x < position.x){
                    //gameOver = true;
                    object->position.x += (velocity.x) * deltaTime;
                }   else{
                    position.x += xPene;
                }
            }
                
                
            // for the moving platform to move back and forth
            if (bounce){
                if (object->selftype.type != "Rocket"){
                    velocity.x = -velocity.x;
                }
            }   else{
                velocity.x = 0;
            }
            
            // The rocket explodes if it comes in comtact with anything from sides
            if (selftype.type == "Rocket"){
                //gameOver = true;
            }
        }
    }
}
    
// Displacement when there is a collision in the y direction
void Entity::checkCollisiony(Entity *objects, int numobjects, bool& gameOver, bool& gameWon){
    for (int i=0; i < numobjects; i++){
        Entity* object = &objects[i];
        if (checkCollision(object)){
            float yPene = fabs(fabs(position.y - object->position.y) - (contactHeight* height* yRepeat + object->contactHeight* object->height* object->yRepeat) / 2);
            
            if (velocity.y>0){
                position.y -= yPene;
            }   else if (velocity.y<0){
                // The player wins if rocket collides with the platform from the top
                if (selftype.type == "Rocket"){
                    if (object->selftype.type == "Platform"){
                        //gameWon = true;
                    }
                }
                position.y += yPene;
            }
            velocity.y = 0;
            
            // otherwise any colllision loses the game
            if(selftype.type == "Rocket" and !gameWon){
                //gameOver = true;
            }
        }
    }
}

// Updating positions of everything accordingly
void Entity::update(Entity* other, Entity* player, int numOfEntities, float deltaTime, bool& gameOver, bool& gameWon){
    
    velocity.x += movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    if (player){
        checkCollisiony(player, 1, gameOver, gameWon);
    }
    checkCollisiony(other, numOfEntities, gameOver, gameWon);
    
    position.x += velocity.x * deltaTime;
    if(player){
        checkCollisionx(player, 1, gameOver, gameWon, deltaTime);
    }
    checkCollisionx(other, numOfEntities, gameOver, gameWon, deltaTime);
    
    matrix = translate(mat4(1.0), position);
}

// Rendering the entity
void Entity::render(ShaderProgram* program){
    program->SetModelMatrix(matrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
