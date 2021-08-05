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


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

// 11.7
#define OBJECT_COUNT 6

// 12.12 -- enemies
#define ENEMY_COUNT 1


struct GameState {
    Entity* player;
    Entity* objects;    // 11.7
    Entity* enemies;    // 12.12
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
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;


    /*
    -----------------   Initialize Objects  -----------------
    */
    state.objects = new Entity[OBJECT_COUNT];
    
    // 12.2 -- remove commented out code, delete ships, replace with floor
    // 11.14 -- comment out cube, Mario, and Pikachu -- replace with ship
    GLuint floorTextureID = Util::LoadTexture("grasstext.jpg");
    Mesh* cubeMesh = new Mesh();
    //cubeMesh->LoadOBJ("cube.obj");
    cubeMesh->LoadOBJ("cube.obj", 30);  // 12.3 -- duplicate texture 10 times // 12.6 -- dup 20 times

    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = cubeMesh;
    state.objects[0].position = glm::vec3(0.0f, -0.25f, 0.0f);
    state.objects[0].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[0].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[0].scale = glm::vec3(30.0f, 0.5f, 30.0f);
    state.objects[0].entityType = FLOOR;


    // ceiling
    GLuint ceilingTextureID = Util::LoadTexture("bricktext.jpg");
    Mesh* ceilingMesh = new Mesh();
    //cubeMesh->LoadOBJ("cube.obj");
    ceilingMesh->LoadOBJ("cube.obj", 30);  // 12.3 -- duplicate texture 10 times // 12.6 -- dup 20 times

    state.objects[1].textureID = ceilingTextureID;
    state.objects[1].mesh = ceilingMesh;
    state.objects[1].position = glm::vec3(0.0f, 3.75f, 0.0f);
    state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[1].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[1].scale = glm::vec3(30.0f, 0.5f, 30.0f);
    state.objects[1].entityType = CEILING;



    // CRATES
    //GLuint crateTextureID = Util::LoadTexture("crate1_diffuse.png");
    //Mesh* crateMesh = new Mesh();   // 12.6 -- need a new one b/c floor one is duplicated
    //crateMesh->LoadOBJ("cube.obj", 1);  // 12.6 -- 1 = no duplicate

    //state.objects[1].textureID = crateTextureID;
    //state.objects[1].mesh = crateMesh;
    //state.objects[1].position = glm::vec3(0.0f, 0.5f, -5.0f);   // 12.6 -- crate is 1 unit tall, stop from sinking into floor
    //state.objects[1].entityType = CRATE;

    //state.objects[2].textureID = crateTextureID;
    //state.objects[2].mesh = crateMesh;
    //state.objects[2].position = glm::vec3(-1.0f, 0.5f, -5.0f);   // 12.7 -- next to 1st crate
    //state.objects[2].entityType = CRATE;

    //state.objects[3].textureID = crateTextureID;
    //state.objects[3].mesh = crateMesh;
    //state.objects[3].position = glm::vec3(0.0f, 1.5f, -5.0f);   // 12.7 -- on top of 1st crate
    //state.objects[3].entityType = CRATE;

    // Walls, i = [1] to i = [4]
    GLuint wallTextureID = Util::LoadTexture("sampleWall.jpg");
    Mesh* wallMesh = new Mesh();
    wallMesh->LoadOBJ("cube.obj", 1);  // 12.3 -- duplicate texture 10 times // 12.6 -- dup 20 times

    for (size_t i = 2; i < OBJECT_COUNT; i++) {
        state.objects[i].textureID = wallTextureID;
        state.objects[i].mesh = wallMesh;

        state.objects[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[i].entityType = WALL;
    }

    state.objects[2].scale = glm::vec3(0.5f, 4.0f, 30.0f);
    state.objects[2].position = glm::vec3(15.0f, 1.5f, 0.0f);
    state.objects[2].width = 0.5f;
    state.objects[2].height = 4.0f;
    state.objects[2].depth = 30.0f;

    state.objects[3].scale = glm::vec3(0.5f, 4.0f, 30.0f);
    state.objects[3].position = glm::vec3(-15.0f, 1.5f, 0.0f);
    state.objects[3].width = 0.5f;
    state.objects[3].height = 4.0f;
    state.objects[3].depth = 30.0f;

    state.objects[4].scale = glm::vec3(30.0f, 4.0f, 0.5f);
    state.objects[4].position = glm::vec3(0.0f, 1.5f, 15.0f);
    state.objects[4].width = 30.0f;
    state.objects[4].height = 4.0f;
    state.objects[4].depth = 0.5f;

    state.objects[5].scale = glm::vec3(30.0f, 4.0f, 0.5f);
    state.objects[5].position = glm::vec3(0.0f, 1.5f, -15.0f);
    state.objects[5].width = 30.0f;
    state.objects[5].height = 4.0f;
    state.objects[5].depth = 0.5f;





    /*
    -----------------   Initialize Enemies  -----------------
    */
    // 12.12
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].position = glm::vec3(rand() % 20 - 10, 0.5, rand() % 20 - 10); // 0.5 = edit to have enemies touch ground
        state.enemies[i].rotation = glm::vec3(0, 0, 0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
        state.enemies[i].isActive = false;
    }

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

    // 12.5 -- A and D turns left/right
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) {
        //state.player->rotation.y += 1.0f;
        state.player->rotation.y += 0.1f;   // make slower
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player->rotation.y -= 0.1f;   // make slower
    }

    state.player->velocity.x = 0;
    state.player->velocity.z = 0;
    if (keys[SDL_SCANCODE_W]) {
        state.player->velocity.z = cos(glm::radians(state.player->rotation.y)) * -2.0f;
        state.player->velocity.x = sin(glm::radians(state.player->rotation.y)) * -2.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        state.player->velocity.z = cos(glm::radians(state.player->rotation.y)) * 2.0f;
        state.player->velocity.x = sin(glm::radians(state.player->rotation.y)) * 2.0f;
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
        state.player->Update(FIXED_TIMESTEP, state.player, state.objects, OBJECT_COUNT);

        // 11.7 -- update cube
        for (size_t i = 0; i < OBJECT_COUNT; i++) {
            state.objects[i].Update(FIXED_TIMESTEP, state.player, state.objects, OBJECT_COUNT);
        }

        // 12.13 -- update enemies
        for (size_t i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.objects, OBJECT_COUNT);
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    // 12.5 -- update viewMatrix
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -state.player->position);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // 11.6 -- also clear depth buffer
    
    program.SetViewMatrix(viewMatrix);  // 12.5

    // 11.7 -- render objects
    for (size_t i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Render(&program);
    }

    // 12.13 -- render enemies
    for (size_t i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
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
