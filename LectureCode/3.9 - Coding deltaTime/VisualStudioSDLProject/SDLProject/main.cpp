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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

// new code
float player_x = 0;     // keeps track of player's x-position


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);       // initialize SDL
    displayWindow = SDL_CreateWindow("Triangle!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);     // create window: name, centered, window size, use OpenGL
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);        // get OpenGL context
    SDL_GL_MakeCurrent(displayWindow, context);         // tells this is the current windows you want to draw to

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);     // tells OpenGL want camera to draw from (0, 0) to (640, 480)

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");       // sets up Shader program

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);      // set modelMatrix to Identity Matrix
                                        // this wasn't changed in this program --> triangle drawn in center of window
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);     // the box of the world you see

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);       // color of triangle = red

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);           // background color, when clear the window use this color
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {        // if window is closed, then quit
            gameIsRunning = false;
        }
    }
}

void Update() {
    player_x += 1.0f;
    modelMatrix = glm::mat4(1.0f);  // set modelMatrix to Identity Matrix, reset object to middle each time
    modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, 0.0f, 0.0f));

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);   // uses glClearColor, clear background

    program.SetModelMatrix(modelMatrix);    // tells shader prog which matrix to use when drawing the next thing

    float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };   // verticies of the triangle { X1, Y1, X2, Y2, X3, Y3 }
                                                                    // apply modelMatrix to all 3 of these verticies
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);  // gives list of verticies
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 3);   // tells prog that you want it to draw triangles, have 3 verticies to draw
    glDisableVertexAttribArray(program.positionAttribute);

    SDL_GL_SwapWindow(displayWindow);   // push drawing to display on screen
}

void Shutdown() {
    SDL_Quit();
}


// main loop
int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();       // happens as fast as computer can do it
    }

    Shutdown();
    return 0;
}