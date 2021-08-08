#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <string>
using namespace std;

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

#include "Menu.h"
#include "Level1.h"
#include "LoseScreen.h"
#include "WinScreen.h"



SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

// UI display
glm::mat4 uiViewMatrix, uiProjectionMatrix;
GLuint fontTextureID;

int numLives = 3;


Scene* currentScene;
Scene* sceneList[4];    // only handle game level for now

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

    // for displaying UI, lives
    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    fontTextureID = Util::LoadTexture("font1.png");

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
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new LoseScreen();
    sceneList[3] = new WinScreen();
    SwitchToScene(sceneList[0]);    // starting scene

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
            case SDLK_RETURN:
                // starts game at Level 1
                // only works if in Main Menu
                if (currentScene == sceneList[0]) {
                    SwitchToScene(sceneList[1]);
                }
                break;

            case SDLK_r:    // maybe delete
                // only works if at Lose/Win screens
                // Press R to return to Main Menu
                if (currentScene == sceneList[2]) {
                    SwitchToScene(sceneList[0]);
                }
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
        currentScene->state.player->rotation.y += 0.3f;   // make slower
    }
    else if (keys[SDL_SCANCODE_D]) {
        currentScene->state.player->rotation.y -= 0.3f;   // make slower
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
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    // 12.5 -- update viewMatrix
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(currentScene->state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -currentScene->state.player->position);

    /*
    -----------------   Life Check    -----------------
    */
    // if player is injured, lose 1 life
    if (currentScene->state.player->injured == true) {
        numLives -= 1;
        currentScene->state.player->injured = false;
        //Mix_PlayChannel(-1, squish, 0);
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // 11.6 -- also clear depth buffer

    program.SetProjectionMatrix(projectionMatrix);  // include for UI display
    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);     // need to render scene

    // if no lives left, switch to Lose screen
    if (numLives <= 0) {
        SwitchToScene(sceneList[2]);
    }

    // display UI
    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);

    if (currentScene == sceneList[0]) {     // Main Menu
        Util::DrawText(&program, fontTextureID, "Game Name", 1.0, -0.3f, glm::vec3(-3, 1.8, 0));
        Util::DrawText(&program, fontTextureID, "Press Enter to Start", 0.4, -0.1f, glm::vec3(-3, 0.3, 0));
        Util::DrawText(&program, fontTextureID, "Use WASD to control your player", 0.25, -0.1f, glm::vec3(-2.5, -0.3, 0));
    }
    else if (currentScene == sceneList[2]) {    // Lose Screen
        Util::DrawText(&program, fontTextureID, "You Lose!", 1.0, -0.3f, glm::vec3(-2.9, 1.0, 0));
        //Util::DrawText(&program, fontTextureID, "Press R to return to Main Menu", 0.3, -0.1f, glm::vec3(-3, 0.0, 0));   // maybe delete
    }
    else if (currentScene == sceneList[3]) {    // Win Screen
        Util::DrawText(&program, fontTextureID, "You Win!", 1.0, -0.3f, glm::vec3(-2.8, 1.0, 0));
        //Util::DrawText(&program, fontTextureID, "Press R to return to Main Menu", 0.3, -0.1f, glm::vec3(-3, 0.0, 0));   // maybe delete
    }
    else {
        Util::DrawText(&program, fontTextureID, "Lives: " + to_string(numLives), 0.5, -0.2f, glm::vec3(-6, 3.2, 0));
        Util::DrawText(&program, fontTextureID, "Time: ", 0.5, -0.2f, glm::vec3(-6, 2.8, 0));
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
