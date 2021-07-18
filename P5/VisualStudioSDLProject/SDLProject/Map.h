#pragma once
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Map {
    // number of tiles wide and number of tiles tall
    // NOT width/height in pixels
    int width;
    int height;
    unsigned int* levelData;    // array with map data in it

    GLuint textureID;   // ID of texture with the tiles on it
    float tile_size;    // how big on texture map the tile is

    // how many tiles are there on the texture map
    // related to that texture
    int tile_count_x;
    int tile_count_y;

    std::vector<float> vertices;
    std::vector<float> texCoords;
    float left_bound, right_bound, top_bound, bottom_bound;

public:
    Map(int width, int height, unsigned int* levelData, GLuint textureID, float tile_size, int
        tile_count_x, int tile_count_y);
    void Build();
    void Render(ShaderProgram* program);
    bool IsSolid(glm::vec3 position, float* penetration_x, float* penetration_y);
};