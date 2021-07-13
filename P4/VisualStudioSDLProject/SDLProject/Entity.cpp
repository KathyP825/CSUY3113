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

/*
--------------- Collision Check ---------------
*/

bool Entity::CheckCollision(Entity* other) {
    if (other == this) return false;

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


void Entity::CheckCollisionsY(Entity* objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity* object = &objects[i];

        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));

            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;

                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;

                collidedBottom = true;
            }

            // player kills enemy if player's top/bottom collides with enemy
            if ((entitytype == PLAYER && object->entitytype == ENEMY) && collidedBottom == true) {
                numEnemiesKilled++;
                object->isAlive = false;
                object->isActive = false;
            }
            else if ((entitytype == PLAYER && object->entitytype == ENEMY) && (collidedTop == true)) {
                numEnemiesKilled++;
                object->isAlive = false;
                object->isActive = false;
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
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }

            // player dies if collide from left/right
            if ((entitytype == PLAYER && object->entitytype == ENEMY) && (collidedLeft == true || collidedRight == true)) {
                isAlive = false;
            }
        }
    }
}


/*
--------------- AI Controls ---------------
*/

void Entity::AIWalker() {
    movement = glm::vec3(-1.0f, 0.0f, 0.0f);
}

// if player is close, AI wakes up ands starts walking
void Entity::AIWaitAndGo(Entity* player) {
    // uses multiple states
    // need pointer to player to get player's position
    switch (aiState) {
        case IDLE:
            // if player close enough, switch to walking
            if (glm::distance(position, player->position) < 3.0f) {
                aiState = WALKING;
            }

            if (player->isAlive == false) {
                speed = 0.0f;
            }

            break;

        case WALKING:
            // is player is to the left of AI, move to left, else move to right
            if (player->position.x < position.x) {
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


// patrols back and forth on platform
// for Enemy 3 only
void Entity::AIPatrol() {
    //if ((position.x <= -3.5f)) {    // needs to be same as starting position or won't work
    //    movement = glm::vec3(1.0f, 0.0f, 0.0f);
    //}
    //else if (position.x >= -0.5f) {
    //    movement = glm::vec3(-1.0f, 0.0f, 0.0f);
    //}

    switch (aiState) {
        case IDLE:
            speed = 0.0f;
        case WALKING:
            if (position.x <= -3.5f) {    // needs to be same as starting position or won't work
                movement = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            else if (position.x >= -0.5f) {
                movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            }
    }
}


// Enemy 2 jumps up and down continuously
void Entity::AIHopper() {
    switch (aiState) {
        case IDLE:
            velocity.y = 0.0f;
            acceleration.y = 0.0f;      // stops enemy from sliding down from gravity
            break;

        case FALL:
            if (position.y <= -0.15f) {      //if on floor
                aiState = RISE;
            }
            else {
                velocity.y = -4.0f;
            }
            break;

        case RISE:
            velocity.y = 2.5f;

            if (position.y >= 2.5f) {   // jump to height limit, fall back down
                aiState = FALL;
            }

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

        case PATROL:
            AIPatrol();
            break;

        case HOPPER:
            AIHopper();
            break;
    }
}


/*
--------------- General Controls ---------------
*/

void Entity::Update(float deltaTime, Entity* player, Entity * platforms, int platformCount, Entity* enemies, int enemyCount) {
    if (isActive == false) return;

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

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


    if (jump) {
        jump = false;   // only jump 1x
        velocity.y += jumpPower;    // does the jump up, acceleration does the back down part
    }


    velocity.x = movement.x * speed;    // when character starts moving left/right, have instant velocity
    velocity += acceleration * deltaTime;


    // 6.16
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(platforms, platformCount);
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(platforms, platformCount);


    if (entitytype == PLAYER) {
        CheckCollisionsX(enemies, enemyCount);
        CheckCollisionsY(enemies, enemyCount);
    }

    // if player dies, remaining enemies stop moving
    // else if player wins, player stops moving left/right/jump
    if ((entitytype == PLAYER) && (isAlive == false)) {
        speed = 0.0f;
        jumpPower = 0.0f;

        for (size_t i = 0; i < enemyCount; i++) {
            if (enemies[i].isActive == true) {
                enemies[i].aiState = IDLE;
            }
        }
    }
    else if ((entitytype == PLAYER) && (numEnemiesKilled == 3)) {
        speed = 0.0f;
        jumpPower = 0.0f;
    }
    
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