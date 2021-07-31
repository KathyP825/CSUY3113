#include "Entity.h"

Entity::Entity() {
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
    rotation = glm::vec3(0);    // 11.8

    speed = 0.0f;
}

void Entity::Update(float deltaTime) {
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

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

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

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
    // 11.8 -- remove because unnecessary
    //glm::mat4 modelMatrix = glm::mat4(1.0f);
    //modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    // 11.7 -- delete
    //float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    //float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    //glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glVertexAttribPointer(program->positionAttribute, 3, GL_FLOAT, false, 0, vertices);     // 11.7 -- change to 3 values
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);    // 11.7 -- no change
    glEnableVertexAttribArray(program->texCoordAttribute);

    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);     // 11.7 

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

