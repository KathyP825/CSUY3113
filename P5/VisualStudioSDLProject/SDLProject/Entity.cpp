#include "Entity.h"

Entity::Entity() {
    position = glm::vec3(0);

    // 6.6 -- initialize movement, acceleration, and velocity
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);

    speed = 0;

    modelMatrix = glm::mat4(1.0f);
}


// 6.9 -- check if curr Entity will collide with another Entity
bool Entity::CheckCollision(Entity* other) {
    if (isActive == false || other->isActive == false) {
        return false;
    }

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) {
        return true;
    }
    return false;

}


// 6.16 -- new collision check
void Entity::CheckCollisionsY(Entity* objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity* object = &objects[i];

        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));

            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;

                collidedTop = true;     //6.21
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;

                collidedBottom = true;  // 6.21
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity* object = &objects[i];

        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));

            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;

                collidedRight = true;   // 6.21
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;

                collidedLeft = true;    // 6.21
            }
        }
    }
}



// 9.11 -- new collision Y and X check
void Entity::CheckCollisionsY(Map* map) {
    // 6 probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // checks to see if any 1 of the sensors is touching something solid
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map* map) {
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}



// 8.7
void Entity::AIWalker() {
    movement = glm::vec3(-1.0f, 0.0f, 0.0f);
}

// 8.8, 8.9
// if player is close, AI wakes up ands starts walking
void Entity::AIWaitAndGo(Entity* player) {
    // uses multiple states
    // need pointer to player to get player's position
    switch (aiState) {
    case IDLE:
        // 8.9 -- want to know if player is close enough
        // if player close enough, switch to walking
        if (glm::distance(position, player->position) < 4.0f) {
            aiState = WALKING;
        }
        else {
            movement = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        break;

    case WALKING:
        // stops moving if too player is too far away
        if (glm::distance(position, player->position) >= 4.0f) {
            aiState = IDLE;
        }

        // is player is to the left of AI, move to left, else move to right
        else if (player->position.x < position.x) {
            movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        else {
            movement = glm::vec3(1.0f, 0.0f, 0.0f);
        }

        break;

    case ATTACKING:
        break;
    }
}

// 8.7 -- process input for an enemy
// can set movement
void Entity::AI(Entity* player) {
    switch (aiType) {
    case WALKER:
        AIWalker();
        break;

    case WAITANDGO:
        AIWaitAndGo(player);
        break;
    }
}



// 6.9 -- update parameters for void Entity::Update(float deltaTime, Entity*)
void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map) {
    // 6.20 -- check if entity is active
    if (isActive == false) {
        return;
    }

    // 6.21
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;


    // 8.7 -- if is enemy, call AI func
    if (entitytype == ENEMY) {
        AI(player);
    }


    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }


    // 6.12 -- do jump
    if (jump) {
        jump = false;   // only jump 1x
        velocity.y += jumpPower;    // does the jump up, acceleration does the back down part
    }



    // 6.7
    velocity.x = movement.x * speed;    // when character starts moving left/right, have instant velocity
    velocity += acceleration * deltaTime;

    //// 9.11 -- replace previous CollsionCheck calls
    //position.y += velocity.y * deltaTime; // Move on Y
    //CheckCollisionsY(map);
    //CheckCollisionsY(objects, objectCount); // Fix if needed

    //position.x += velocity.x * deltaTime; // Move on X
    //CheckCollisionsX(map);
    //CheckCollisionsX(objects, objectCount); // Fix if needed

    // 9.11 -- replace previous CollsionCheck calls
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount); // Fix if needed
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount); // Fix if needed


    /*
    // 6.16 -- replace and delete
    // 6.11 -- after move the position but before update the position, check and fix collision
    for (int i = 0; i < platformCount; i++) {
        Entity* platform = &platforms[i];

        // if collide with something
        if (CheckCollision(platform)) {
            float ydist = fabs(position.y - platform->position.y);      // get distance from centers
            float penetrationY = fabs(ydist - (height / 2.0f) - (platform->height / 2.0f));     // how too far we went

            if (velocity.y > 0) {   // if going up and hit a platform, move self down
                position.y -= penetrationY;
                velocity.y = 0;     // if hit platform, stop moving
            }
            else if (velocity.y < 0) {      // if falling down and hit a platform, move self up
                position.y += penetrationY;
                velocity.y = 0;     // if hit platform, stop moving
            }
        }
    }
    */


    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index) {
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {
    if (isActive == false) {
        return;
    }

    program->SetModelMatrix(modelMatrix);

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}