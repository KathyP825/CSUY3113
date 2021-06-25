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

    // 6.6 -- add variables to keep track of acceleration and velocity
    glm::vec3 acceleration;
    glm::vec3 velocity;

    // 6.9 -- assume everything in game world has a width and height of 1 unit
    // change if object is scaled
    float width = 1.0f;
    float height = 1.0f;

    // 6.12 -- track if jump
    bool jump = false;
    float jumpPower = 0.0f;     // can jump higher sometimes

    float speed;

    GLuint textureID;

    glm::mat4 modelMatrix;

    int* animRight = NULL;
    int* animLeft = NULL;
    int* animUp = NULL;
    int* animDown = NULL;

    int* animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;

    // 6.20
    bool isActive = true;

    Entity();

    // 6.9 -- if collided with object, return TRUE
    bool CheckCollision(Entity* other);

    // 6.16 -- new collision detection prototype
    // objects = platforms, objectCount = platformCount
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);

    //6.9 -- change prototype void Update(float deltaTime);
    void Update(float deltaTime, Entity* platforms, int platformCount);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
};