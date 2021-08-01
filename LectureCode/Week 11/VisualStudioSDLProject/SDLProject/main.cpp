#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Entity.h"

/*
// 11.11 -- remove cube hardcode
// 11.6
float cubeVertices[] = {
-0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
-0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5,
0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5,
0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5,
-0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5,
-0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5,
0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5,
0.5, 0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5,
-0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5,
-0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5,
0.5, 0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5,
0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5
};

// 11.6
float cubeTexCoords[] = {
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f
};
*/


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

// 11.7
#define OBJECT_COUNT 2


struct GameState {
    Entity* player;
    Entity* objects;    // 11.7
};

GameState state;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("3D!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);  // 11.5 -- change window size
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1280, 720);    // 11.5 -- changed window size view

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    //projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 100.0f);     // change from orthographic to perspective

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 11.6 -- include Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /*
    -----------------   Initialize Player  -----------------
    */

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 1.5f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;


    /*
    -----------------   Initialize Objects  -----------------
    */
    state.objects = new Entity[OBJECT_COUNT];
    
    // 11.14 -- comment out cube, Mario, and Pikachu -- replace with ship
    GLuint shipTextureID = Util::LoadTexture("ship.png");
    Mesh* shipMesh = new Mesh();
    shipMesh->LoadOBJ("ship.obj");

    state.objects[0].textureID = shipTextureID;
    state.objects[0].mesh = shipMesh;
    //state.objects[0].position = glm::vec3(0.0f, 0.0f, -5.0f);     // ship starts infront of us
    state.objects[0].position = glm::vec3(-5.0f, 0.0f, 10.0f);   // ship starts behind us and flies past us
    state.objects[0].rotation = glm::vec3(0.0f, 180.0f, 0.0f);  // initially facing us, change to see back of ship
    state.objects[0].acceleration = glm::vec3(0.0f, 0.0f, -10.0f);  // ship move forward away from us
    state.objects[0].entityType = SHIP;

    state.objects[1].textureID = shipTextureID;
    state.objects[1].mesh = shipMesh;
    state.objects[1].position = glm::vec3(5.0f, 0.0f, 10.0f);   // ship starts behind us and flies past us
    state.objects[1].rotation = glm::vec3(0.0f, 180.0f, 0.0f);  // initially facing us, change to see back of ship
    state.objects[1].acceleration = glm::vec3(0.0f, 0.0f, -10.0f);  // ship move forward away from us
    state.objects[1].entityType = SHIP;
    
    //// Cube
    //GLuint cubeTextureID = Util::LoadTexture("crate1_diffuse.png");

    //// 11.11 -- replacement code for cube
    //Mesh* cubeMesh = new Mesh();
    //cubeMesh->LoadOBJ("cube.obj");

    //state.objects[0].textureID = cubeTextureID;
    //state.objects[0].mesh = cubeMesh;   // 11.11
    //state.objects[0].position = glm::vec3(0.0f, 0.0f, -5.0f);   // 11.7 -- center of screen, 1 unit up, 5 units back into the screen
    //state.objects[0].entityType = CUBE;     // 11.8

    ///*
    //// 11.11 -- remove cube code b/c its hardcode
    //state.objects[0].vertices = cubeVertices;
    //state.objects[0].texCoords = cubeTexCoords;
    //state.objects[0].numVertices = 36;
    ////state.objects[0].rotation = glm::vec3(45.0f, 0.0f, 0.0f);   // 11.8 -- rotate box 45 degrees on X-axis
    //*/

    //// 11.12 -- Mario
    //GLuint marioTextureID = Util::LoadTexture("mario_body.png");
    //Mesh* marioMesh = new Mesh();
    //marioMesh->LoadOBJ("mario.obj");

    //state.objects[1].textureID = marioTextureID;
    //state.objects[1].mesh = marioMesh;
    //state.objects[1].position = glm::vec3(-10.0f, 0.0f, -40.0f);   // center for Mario = at its feet
    //state.objects[1].scale = glm::vec3(0.25f, 0.25f, 0.25f);    // 11.13 -- scale Mario to 1/4 size
    //state.objects[1].entityType = ENEMY;

    //// 11.12 -- Pikachu
    //GLuint pikachuTextureID = Util::LoadTexture("pikachu.png");
    //Mesh* pikachuMesh = new Mesh();
    //pikachuMesh->LoadOBJ("pikachu.obj");

    //state.objects[2].textureID = pikachuTextureID;
    //state.objects[2].mesh = pikachuMesh;
    //state.objects[2].position = glm::vec3(2.0f, 0.0f, -4.0f);   // pikachu is much smaller than Mario
    //state.objects[2].entityType = ENEMY;


}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                // Some sort of action
                break;

            }
            break;
        }
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP);

        // 11.7 -- update cube
        for (size_t i = 0; i < OBJECT_COUNT; i++) {
            state.objects[i].Update(FIXED_TIMESTEP);
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
}


void Render() {
    //glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // 11.6 -- also clear depth buffer

    //state.player->Render(&program);   // 11.7 -- don't render b/c is 1st person game
    
    // 11.7 -- render objects
    for (size_t i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Render(&program);
    }

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
