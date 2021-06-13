/*
//////// NOTES ////////
-   AVOID odd resolutions for images, and things that aren't multiples of 2 or 16
-   SDL_SCANCODE_A can use arrow keys

//////// CHANGES FOR THIS SAVE ////////
-   object moves when button is held down for a long time
-   can move in diagonals / with multiple buttons pressed
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

/*
// 4.9 -- can remove
float player_x = 0;     // keeps track of player's x-position
float player_rotate = 0;
*/

// new code -- 4.9
glm::vec3 player_position = glm::vec3(0, 0, 0);     // start at 0,0,0   // player_position = player's postion in our world
glm::vec3 player_movement = glm::vec3(0, 0, 0);     // don't go anywhere (yet)   // player_movement = where the player wants to move / how much they want to move in a direction
float player_speed = 1.0f;


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

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);           // background color, when clear the window use this color

    // new code -- 3.25
    // enables blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // good setting for transparency

    playerTextureID = LoadTexture("ctg.png");   // loads texture
}

/*
void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {        // if window is closed, then quit
            gameIsRunning = false;
        }
    }
}
*/


// if don't handle anything, nothing happens
void ProcessInput() {
    player_movement = glm::vec3(0);     // if nothing is pressed, don't want to go anywhere

    SDL_Event event;
    while (SDL_PollEvent(&event)) {     // while there are events to process
        switch (event.type) {   // checks types of events 
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
                //player_movement.x = 1.0f;     // takes forever to get anywhwere
                break;

            case SDLK_SPACE:
                // Some sort of action
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    // check if continuously holding down key
    // can't do left and right key at the same time so use else if
    const Uint8* keys = SDL_GetKeyboardState(NULL);     // gets the pointer
    if (keys[SDL_SCANCODE_LEFT]) {      // if left key is down right now
        player_movement.x = -1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        player_movement.x = 1.0f;
    }

    if (keys[SDL_SCANCODE_UP]) {      // if left key is down right now
        player_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        player_movement.y = -1.0f;
    }

    // check if length of how much we're trying to move is greater than 1
    // if yes, normalize
    // make sure you can't move faster if you're holding down 2 keys
    if (glm::length(player_movement) > 1.0f) {
        player_movement = glm::normalize(player_movement);
    }
}



// new code
float lastTicks = 0.0f;   // ticks = amount of time since SDL was last initialized

/*
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
*/

// 4.9 -- replace previous Update()
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    // add (direction * units per second *elapsed time)
    // add how much player wants to move / direction
    player_position += player_movement * player_speed * deltaTime;

    modelMatrix = glm::mat4(1.0f);      // set to Identity Matrix
    modelMatrix = glm::translate(modelMatrix, player_position);
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