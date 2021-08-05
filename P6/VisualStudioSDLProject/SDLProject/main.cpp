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
#include "Scene.h"

//#include "Menu.h"
#include "Level1.h"


#define OBJECT_COUNT 6
#define ENEMY_COUNT 1


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[1];    // only handle game level for now

// for audio
//Mix_Music* music;
//Mix_Chunk* sound;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);   // need to add audio pipe
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

    // scenes
    sceneList[0] = new Level1();
    SwitchToScene(sceneList[0]);

}

void ProcessInput() {
    //currentScene->state.player->velocity = glm::vec3(0);
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                // starts game at Level 1
                // only works if in Main Menu
                //if (currentScene == sceneList[0]) {
                //    SwitchToScene(sceneList[1]);
                //}
                break;

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
        //currentScene->state.player->rotation.y += 1.0f;
        currentScene->state.player->rotation.y += 0.1f;   // make slower
    }
    else if (keys[SDL_SCANCODE_D]) {
        currentScene->state.player->rotation.y -= 0.1f;   // make slower
    }

    currentScene->state.player->velocity.x = 0;
    currentScene->state.player->velocity.z = 0;
    if (keys[SDL_SCANCODE_W]) {
        currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
        currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
        currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
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

    // time accumulator
    while (deltaTime >= FIXED_TIMESTEP) {
        //currentScene->state.player->Update(FIXED_TIMESTEP, currentScene->state.player, currentScene->state.objects, OBJECT_COUNT);
        currentScene->Update(FIXED_TIMESTEP);

        //// 11.7 -- update cube
        //for (size_t i = 0; i < OBJECT_COUNT; i++) {
        //    currentScene->state.objects[i].Update(FIXED_TIMESTEP, currentScene->state.player, currentScene->state.objects, OBJECT_COUNT);
        //}

        //// 12.13 -- update enemies
        //for (size_t i = 0; i < ENEMY_COUNT; i++) {
        //    currentScene->state.enemies[i].Update(FIXED_TIMESTEP, currentScene->state.player, currentScene->state.objects, OBJECT_COUNT);
        //}

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    // 12.5 -- update viewMatrix
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(currentScene->state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -currentScene->state.player->position);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // 11.6 -- also clear depth buffer
    
    program.SetViewMatrix(viewMatrix);  // 12.5

    currentScene->Render(&program);     // need to render scene

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
