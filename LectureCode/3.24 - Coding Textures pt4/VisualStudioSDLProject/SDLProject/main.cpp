/*
//////// NOTES ////////
-   AVOID odd resolutions for images, and things that aren't multiples of 2 or 16

//////// CHANGES FOR THIS SAVE ////////
-   update Render() function
    -   use the 2 triangles and the texture coordinates
    -   draw the textures onto the 2 triangles
-   replaced old code with code from 3.18 notes
-   uncommented out the translate, then rotate code in Update() function

-   Results = image is tumbling to the right as it rotates

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

// new code
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

// new code
float player_x = 0;     // keeps track of player's x-position
float player_rotate = 0;

// new code -- 3.24
GLuint playerTextureID;

// new code -- taken from 3.16 Notes
GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);   // generate 1 texture ID 
    glBindTexture(GL_TEXTURE_2D, textureID);    // ensure all commanda after uses this texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);       // initialize SDL
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);     // create window: name, centered, window size, use OpenGL
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);        // get OpenGL context
    SDL_GL_MakeCurrent(displayWindow, context);         // tells this is the current windows you want to draw to

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);     // tells OpenGL want camera to draw from (0, 0) to (640, 480)

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");       // sets up Shader program, loads a version of shader that can handle textured polygons

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);      // set modelMatrix to Identity Matrix
                                        // this wasn't changed in this program --> triangle drawn in center of window
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);     // the box of the world you see

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);       // color of triangle = red        // comment out b/c won't do anything

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);           // background color, when clear the window use this color

    playerTextureID = LoadTexture("ctg.png");   // loads texture
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {        // if window is closed, then quit
            gameIsRunning = false;
        }
    }
}

// new code
float lastTicks = 0.0f;   // ticks = amount of time since SDL was last initialized

void Update() {
    //new code
    float ticks = (float)SDL_GetTicks() / 1000.0f;      // SDL_GetTicks() gives the amount of milliseconds since SDL was initialized
    float deltaTime = ticks - lastTicks;    // amount of time that has gone by =  curr amount of time that has gone by - time from the last frame
                                            // if takes a large amount of time, code to check and say 1 second has gone by
    lastTicks = ticks;

    player_x += 1.0f * deltaTime;
    //player_rotate += 90.0f * deltaTime;     // rotate counterclockwise
    player_rotate += -90.0f * deltaTime;    // rotate clockwise

    modelMatrix = glm::mat4(1.0f);  // set modelMatrix to Identity Matrix, reset object to middle each time

    // triangle rotates as its moving to the right
    modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, 0.0f, 0.0f));     // move to the right
    modelMatrix = glm::rotate(modelMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));   // rotate on z-axis

    // triangle rotates in a spiral outwards
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));   // rotate on z-axis
    //modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, 0.0f, 0.0f));     // move to the right


}

/*
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
*/

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);   // clear background
    program.SetModelMatrix(modelMatrix);    // set Model Matrix

    // verticies & texture coordinates for the 2 triangles
    float verticies[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    // tell to use this list of coordinates
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticies);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    // tell to use this texture & draw them
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);   // always do last
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