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
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) {
        return true;
    }
    return false;

}


// 6.9 -- update parameters for void Entity::Update(float deltaTime, Entity*)
void Entity::Update(float deltaTime, Entity * platforms, int platformCount) {
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


    // 6.7
    velocity.x = movement.x * speed;    // when character starts moving left/right, have instant velocity
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;   // new way to calculate position
    

    // 9.11 -- after move the position but before update the position, check and fix collision
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