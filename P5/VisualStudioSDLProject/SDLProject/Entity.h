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
#include "Map.h"    // 9.9

enum EntityType { PLAYER, PLATFORM, ENEMY };      // 8.7 -- list of entity types
enum AIType { WALKER, WAITANDGO };    // 8.7, 8.8 -- list of AI types, what they do
enum AIState { IDLE, WALKING, ATTACKING };      // 8.7 -- list of states AI can be in

class Entity {
public:
    EntityType entitytype;
    AIType aiType;
    AIState aiState;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;

    // assume everything in game world has a width and height of 1 unit
    // change if object is scaled
    float width = 1.0f;
    float height = 1.0f;

    bool jump = false;
    float jumpPower = 0.0f;

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

    bool isActive = true;
    //bool injured = false;
    int tempLives = 0;

    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;

    Entity();

    // 6.9 -- if collided with object, return TRUE
    bool CheckCollision(Entity* other);

    // 6.16 -- new collision detection prototype
    // objects = platforms, objectCount = platformCount
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);

    // 9.11 -- collisions for Map
    // check if collide with the map, not entities
    void CheckCollisionsX(Map* map);
    void CheckCollisionsY(Map* map);

    void Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);

    // prototypes for AI
    void AI(Entity* player);
    void AIWalker();
    void AIWaitAndGo(Entity* player);
};