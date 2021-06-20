// Entity.cpp

#include "Entity.h"

// does the initilizing
Entity::Entity() {
    position = glm::vec3(0);
    speed = 0;

    modelMatrix = glm::mat4(1.0f);
}

// basic Update() because it might be the same for all entities
void Entity::Update(float deltaTime) {

    // 5.21 -- if don't have animations, don't animate
    if (animIndices != NULL) {
        // 5.21 -- copied from main.cpp,  replace with local variables
        // only animate if moving
        if (glm::length(movement) != 0) {
            animTime += deltaTime;
            if (animTime >= 0.25f) {       // when reach 1/4 of a second, flip to next frame
                                           // if 0.1f, moves faster / is running
                animTime = 0.0f;    // reset timer
                animIndex++;        // move to next index on list of frames

                if (animIndex >= animFrames) {      // checks if at last, does the looping
                    animIndex = 0;
                }
            }

        }
        else {
            animIndex = 0;  // if not moving, go to first frame of animation
        }
    }

    


    position += movement * speed * deltaTime;

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::Render(ShaderProgram* program) {
    program->SetModelMatrix(modelMatrix);

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


