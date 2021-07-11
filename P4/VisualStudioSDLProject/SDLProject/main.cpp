/*
//////// CHANGES FOR THIS SAVE ////////
-   create a floor at bottom, place player on left
-   place an AI on right
-   give the AI some basic behavior
-   2nd AI has different behavior
-   AI change between states and waht they do when they are in a certain state

-   main.cpp
    -   8.5
        -   PLATFORM_COUNT = 10
        -   Initialize()
            -   for loop to initialize floor instead of making each tile 1 by 1
            -   change player initial position, small drop to starting place
    -   8.6
        -   ENEMY_COUNT = 1
        -   GameState struct
            -   add enemy Entity
        -   Initialize()
            -   initialize enemy Entity
        -   Update()
            -   tell all enemies to update
        -   Render()
            -   render enemies
    -   8.7
        -   Initialize()
            -   initilize entities with EntityType
    -   8.8
        -   Initialize()
            -   initilize AIs with AIType and AIState
        -   Update()
            -   player and enemies recieve pointer to player

-   Entity.h
    -   8.7
        -   create EntityType enum
        -   create AIType enum
        -   create AIState enum
        -   add function prototypes for AI
    -   8.8
        -   add 2nd AIType
        -   create function prototype for new AI type, AIWaitAndGo()
        -   update prototypes to include parameter Entity* player 

-   Entity.cpp
    -   8.7
        -   create AI() and AIWalker() functions
    -   8.8
        -   update AI() to have AIWaitAndGo()
        -   include Entity* player parameter in Update(), AI()
    -   8.9
        -   update AIWaitAndGo() to move AI to player if player is close enough

-   Notes:
    -   still need acceleration/gravity for AI
    -   need to check collisions for player with things other than platforms
        -   collision with AI
    -   
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 11    // 8.5 -- 11 platforms
#define ENEMY_COUNT 1   // 8.6 -- 1 enemy

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;        // 8.6
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}



void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entitytype = PLAYER;      // 8.7 -- initilize with PLAYER entity type
    state.player->position = glm::vec3(-4.0f, -1.0f, 0.0f);     // 8.5 -- initialize player at bottom left
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);   // 6.6 -- set acceleration and never changing this value
    //state.player->speed = 1.0f;
    state.player->speed = 1.5f;     // 6.13 -- increase player speed
    state.player->textureID = LoadTexture("george_0.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    //6.11 -- fix character hovering
    state.player->height = 0.8f;    // num can vary
    state.player->width = 0.8f;     // not necessary, but can if you want


    // 6.12 -- set up for jump
    state.player->jumpPower = 5.0f;


    // 6.8 -- set up platforms
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");

    // 8.5 -- creates the bottom floor
    for (size_t i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].entitytype = PLATFORM;       // 8.7 -- initilize with PLATFORM entity type
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-5.0f + i, -3.25f, 0);
    }
    
    /*
    // 8.5 -- delete because will be initilized in for loop above

    state.platforms[0].textureID = platformTextureID;
    state.platforms[0].position = glm::vec3(-1, -3.25f, 0);

    state.platforms[1].textureID = platformTextureID;
    state.platforms[1].position = glm::vec3(0, -3.25f, 0);
    //state.platforms[1].isActive = false;      // 6.20 -- test for platform's isActive is false

    state.platforms[2].textureID = platformTextureID;
    state.platforms[2].position = glm::vec3(1, -3.25f, 0);

    state.platforms[3].textureID = platformTextureID;       // 6.13 -- add 4th platform
    state.platforms[3].position = glm::vec3(-3, -3.25f, 0);

    state.platforms[4].textureID = platformTextureID;       // 6.14 -- add 5th platform
    state.platforms[4].position = glm::vec3(1.2, -2.25f, 0);
    */

    for (size_t i = 0; i < PLATFORM_COUNT; i++) {   // only updates platforms 1x
        //state.platforms[i].Update(0);     // 6.9 -- update to version with all parameters
        //state.platforms[i].Update(0, NULL, 0);
        state.platforms[i].Update(0, state.player, NULL, 0);    // 8.8 -- update to include player pointer
    }


    // 8.6 -- initialize Enemies
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("ctg.png");

    state.enemies[0].entitytype = ENEMY;        // 8.7 -- initilize with ENEMY entity type
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4.0f, -2.25f, 0.0f);
    state.enemies[0].speed = 1.0f;

    //state.enemies[0].aiType = WALKER;       // 8.7 -- initialize the AIType
    //state.enemies[0].aiState = WALKING;     // 8.7 -- initialize the AIState
    state.enemies[0].aiType = WAITANDGO;    // 8.8 -- initialize the AIType to 2nd type
    state.enemies[0].aiState = IDLE;        // 8.8 -- initialize the AIState
}



void ProcessInput() {

    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                // jump
                // 6.21 -- only jump if colliding on bottom
                if (state.player->collidedBottom){
                    state.player->jump = true;  // 6.12
                }
                
                //state.player->jump = true;  // 6.12
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}



/*
float lastTicks = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    state.player->Update(deltaTime);
}
*/

// 6.5 -- replace Update() to fixed timestate version
#define FIXED_TIMESTEP 0.0166666f   // 60 times per second
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        // if not enough time has passed, store into accumulator and don't do anything
        accumulator = deltaTime;
        return;
    }

    // when enough time has passed, update player with FIXED_TIMESTEP
    // if have extra time (accumulate 2x), update 2x
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        //state.player->Update(FIXED_TIMESTEP);     // 6.9 -- update to include all parameters
        //state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);    // 8.8 -- player gets pointer to self

        // 8.6
        for (size_t i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);     // 8.8 -- enemies gets pointer to player
        }

        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}




void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // render and display all platforms
    for (size_t i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    // 8.6 -- render and display all enemies
    for (size_t i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }


    state.player->Render(&program);

    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}