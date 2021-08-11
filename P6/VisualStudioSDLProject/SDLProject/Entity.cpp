#include "Entity.h"

Entity::Entity() {
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
    rotation = glm::vec3(0);    // 11.8
    scale = glm::vec3(1);   // default scale = 1

    speed = 0.0f;

    billboard = false;
    width = 1.0f;
    height = 1.0f;
    depth = 1.0f;

    isActive = true;
    //gainHealth = false;
    reachedExit = false;
}


bool Entity::CheckCollision(Entity* other) {
    // ensure enemy isn't colliding with player when isActive = false
    if (other->isActive == false) {
        return false;
    }

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);
    if (xdist < 0 && ydist < 0 && zdist < 0) return true;
    return false;
}



void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Entity* enemies, int enemyCount) {
    if (isActive == false) return;
    
    glm::vec3 previousPosition = position;

    // ensures 2d enemy always face player
    if (billboard) {
        float directionX = position.x - player->position.x;
        float directionZ = position.z - player->position.z;
        rotation.y = glm::degrees(atan2f(directionX, directionZ));

        // billboard/enemies moves towards player
        velocity.z = cos(glm::radians(rotation.y)) * -1.0f;
        velocity.x = sin(glm::radians(rotation.y)) * -1.0f;
    }

    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    // only care about collisions if player is colliding
    if (entityType == PLAYER) {
        for (int i = 0; i < objectCount; i++) {
            if (objects[i].entityType == FLOOR) continue;   // ignore collisions with the floor

            reachedExit = false;    // need or else will auto jump to Win Screen
            if (objects[i].entityType == GOAL) {
                reachedExit = true;
            }
            
            // removed because for some reason HEALTH was always true
            //gainHealth = false;
            //if (objects[i].entityType == HEALTH) {
            //    gainHealth = true;
            //    objects[i].isActive = false;
            //}

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

    // enemy rotation speed to face player
    if (entityType == ENEMY){
        rotation.y += 30 * deltaTime;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

}

void Entity::Render(ShaderProgram* program) {
    if (isActive == false) return;

    program->SetModelMatrix(modelMatrix);
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