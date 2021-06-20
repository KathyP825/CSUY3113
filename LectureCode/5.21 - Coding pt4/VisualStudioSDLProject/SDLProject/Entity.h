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

    // 5.21 -- copy code from main.cpp, set to NULL and 0

    /*
    // list of indicies of the sprites for each of the animations
    int* animRight = new int[4]{ 3, 7, 11, 15 };        // walk to the right
    int* animLeft = new int[4]{ 1, 5, 9, 13 };         // walk to the left
    int* animUp = new int[4]{ 2, 6, 10, 14 };           // walk up
    int* animDown = new int[4]{ 0, 4, 8, 12 };         // walk down

    // keeps trackof current animation
    int* animIndices = animRight;   // current animation to run
    int animFrames = 4;             // 4 frames
    int animIndex = 0;              // index for which sprite on animIndices[] is being used
    float animTime = 0.0f;
    */


    // list of indicies of the sprites for each of the animations
    int* animRight = NULL;        // walk to the right
    int* animLeft = NULL;         // walk to the left
    int* animUp = NULL;           // walk up
    int* animDown = NULL;         // walk down

    // keeps trackof current animation
    int* animIndices = NULL;   // current animation to run
    int animFrames = 0;             // 4 frames
    int animIndex = 0;              // index for which sprite on animIndices[] is being used
    float animTime = 0.0f;

    int animCols = 0;
    int animRows = 0;

    Entity();

    void Update(float deltaTime);
    void Render(ShaderProgram* program);
};
#pragma once


