#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Mesh.h"
enum EntityType { PLAYER, PLATFORM, ENEMY, CUBE, SHIP };

class Entity {
public:
    EntityType entityType;

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 rotation;     // 11.8
    glm::vec3 scale;    // 11.13

    float speed;

    GLuint textureID;

    /*
    // 11.11 -- delete, replace with Mesh class
    // 11.7
    float* vertices;
    float* texCoords;
    int numVertices;
    */

    Mesh* mesh;     // 11.11
    
    glm::mat4 modelMatrix;

    Entity();

    void Update(float deltaTime);
    void Render(ShaderProgram* program);
};



