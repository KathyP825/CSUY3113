#include "Entity.h"

Entity::Entity() {
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
    rotation = glm::vec3(0);    // 11.8
    scale = glm::vec3(1);   // default scale = 1

    speed = 0.0f;

    // 12.9
    billboard = false;
    width = 1.0f;
    height = 1.0f;
    depth = 1.0f;

    isActive = true;
    reachedExit = false;
}


// 12.9 -- collision check function
bool Entity::CheckCollision(Entity* other) {
    // makes sure enemy isn't colliding with player when isActive is false
    if (other->isActive == false) {
        return false;
    }

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);
    if (xdist < 0 && ydist < 0 && zdist < 0) return true;
    return false;
}


// 12.9 -- new Update with parameters for collision check
//void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount) {
void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Entity* enemies, int enemyCount) {
    if (isActive == false) return;
    
    glm::vec3 previousPosition = position;

    // 12.12 -- ensure 2d enemy always face player
    if (billboard) {
        float directionX = position.x - player->position.x;
        float directionZ = position.z - player->position.z;
        rotation.y = glm::degrees(atan2f(directionX, directionZ));

        // 12.13 -- billboard walks towards us
        velocity.z = cos(glm::radians(rotation.y)) * -1.0f;
        velocity.x = sin(glm::radians(rotation.y)) * -1.0f;
    }

    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    // 12.9 -- only care about collisions if it's player colliding
    if (entityType == PLAYER) {
        for (int i = 0; i < objectCount; i++) {
            // Ignore collisions with the floor
            if (objects[i].entityType == FLOOR) continue;

            reachedExit = false;    // need or else will auto jump to Win Screen
            if (objects[i].entityType == GOAL) {
                reachedExit = true;
            }

            if (CheckCollision(&objects[i])) {
                position = previousPosition;
                break;
            }
        }

        for (int i = 0; i < enemyCount; i++) {
            if (CheckCollision(&enemies[i])) {
                injured = true;
                enemies[i].isActive = false;
            }
        }

    }

    // 11.8 -- rotate every frame, causes continuous rotation
    if (entityType == CUBE) {
        /*
        -----------------   One Axis  -----------------
        */
        // X-axis
        //rotation.x += 45 * deltaTime;     // rotate in towards screen
        //rotation.x += -45 * deltaTime;   // rotate out away from screen

        // Y-axis
        //rotation.y += 45 * deltaTime;     // rotate right
        //rotation.y += -45 * deltaTime;      // rotate left

        // Z-axis
        //rotation.z += 45 * deltaTime;     // rotate counter clockwise from top view
        //rotation.z += -45 * deltaTime;      // rotate clockwise from top view

        /*
        -----------------   Two Axis  -----------------
        */
        rotation.y += 45 * deltaTime;
        rotation.z += 45 * deltaTime;

    }
    else if (entityType == ENEMY){
        rotation.y += 30 * deltaTime;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);    // 11.13 -- order = translate, scale, rotate

    /*
    -----------------   One Axis  -----------------
    */
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // 11.8 -- rotate X
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // 11.8 -- rotate Y
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // 11.8 -- rotate Z

    /*
    -----------------   Two Axis  -----------------
    */
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));



}

void Entity::Render(ShaderProgram* program) {
    if (isActive == false) return;
    // 11.8 -- remove because unnecessary
    //glm::mat4 modelMatrix = glm::mat4(1.0f);
    //modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    // 12.12 -- replace previous code
    glBindTexture(GL_TEXTURE_2D, textureID);
    if (billboard) {
        DrawBillboard(program);
    }
    else {
        mesh->Render(program);
    }
}

void Entity::DrawBillboard(ShaderProgram* program) {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}