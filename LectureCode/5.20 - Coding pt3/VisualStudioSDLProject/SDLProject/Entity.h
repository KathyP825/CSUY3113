// Enitity.h
#pragma once

/*
#ifndef Entity_h
#define Entity_h

#include<stdio.h>


#endif // !Entity_h
*/

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

class Entity {
public:
    glm::vec3 position;
    glm::vec3 movement;
    float speed;

    GLuint textureID;   // don't need to load the same texture multiple times

    glm::mat4 modelMatrix;

    Entity();

    void Update(float deltaTime);
    void Render(ShaderProgram* program);
};
#pragma once

