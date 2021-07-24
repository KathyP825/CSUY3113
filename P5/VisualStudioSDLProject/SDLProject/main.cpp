/*
Kathy Pan
Project 5
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

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

//// to display lives
//glm::mat4 uiViewMatrix, uiProjectionMatrix;
//GLuint fontTextureID;

int numLives = 3;
//int playerWin = -1;  // -1 = default, 0 = lose, 1 = win

Scene* currentScene;
Scene* sceneList[4];

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
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

    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);

    // set scene with current lives
    //currentScene->state.player->tempLives = numLives;
}



void ProcessInput() {

    currentScene->state.player->movement = glm::vec3(0);

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
                // Starts game at Level 1
                // only works if in Main Menu
                if (currentScene == sceneList[0]) {
                    //SwitchToScene(sceneList[1]); // need to change back to SwitchToScene(sceneList[1]);
                                                 // was changed for testing
                    SwitchToScene(sceneList[1]);
                }
                
                break;

            case SDLK_SPACE:
                // jump
                // 6.21 -- only jump if colliding on bottom
                if (currentScene->state.player->collidedBottom) {
                    currentScene->state.player->jump = true;  // 6.12
                }

                //state.player->jump = true;  // 6.12
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }


    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

}



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
        //state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);    // 8.8 -- player gets pointer to self
        currentScene->Update(FIXED_TIMESTEP);      // 9.7

        /*
        // 9.5 -- comment out
        // 8.6
        for (size_t i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);     // 8.8 -- enemies gets pointer to player
        }
        */

        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    // 9.13 -- add sidescrolling
    // shows left limit
    // if player position more than 5, follow their position
    // else, keep view at fixed position
    viewMatrix = glm::mat4(1.0f);

    if (currentScene->state.player->position.x > 5) {
        //viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0.0f));     // follow player's X position   // 3.75 = move up into view
        
        // if main menu, do not move view
        // else scroll like normal
        if (currentScene == sceneList[0]) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5.0f, 3.75f, 0.0f));
        }
        else if (currentScene->state.player->position.x > 7.0) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-8.0f, 3.75f, 0.0f));
        }
        else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0.0f));     // follow player's X position   // 3.75 = move up into view
        }
    }
    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5.0f, 3.75, 0.0f));
    }





    // if need to change lives
    //if (currentScene->state.player->injured == true) {
    //    numLives -= 1;
    //    currentScene->state.player->injured == false;
    //}
    //numLives = currentScene->state.player->tempLives;
    numLives -= currentScene->state.player->tempLives;
    if (numLives <= 0) {
        currentScene->state.player->isActive = false;
    }

}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);  // 9.13 -- updates viewMatrix
    
    currentScene->Render(&program);

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

        // switches to next scene
        if (currentScene->state.nextScene >= 0) {
            SwitchToScene(sceneList[currentScene->state.nextScene]);
        }

        Render();
    }

    Shutdown();
    return 0;
}