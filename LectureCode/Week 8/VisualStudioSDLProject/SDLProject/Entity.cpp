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




// 6.9 -- update parameters for void Entity::Update(float deltaTime, Entity*)
void Entity::Update(float deltaTime, Entity * platforms, int platformCount) {
    // 6.20 -- check if entity is active
    if (isActive == false) {
        return;
    }

    // 6.21
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

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


    /*
    // 6.11 -- delete
    // 6.9  -- check if colliding with platforms
    // if collide with any platform, return and get out because don't need to update anything else
    for (size_t i = 0; i < platformCount; i++) {
        if (CheckCollision(&platforms[i])) {
            return;
        }
    }
    */


    // 6.12 -- do jump
    if (jump) {
        jump = false;   // only jump 1x
        velocity.y += jumpPower;    // does the jump up, acceleration does the back down part
    }



    // 6.7
    velocity.x = movement.x * speed;    // when character starts moving left/right, have instant velocity
    velocity += acceleration * deltaTime;
    //position += velocity * deltaTime;   // new way to calculate position      // 6.16 -- remove


    // 6.16
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(platforms, platformCount);
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(platforms, platformCount);
    

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