/*
//////// NOTES ////////
-   AVOID odd resolutions for images, and things that aren't multiples of 2 or 16
-   SDL_SCANCODE_A can use arrow keys

//////// CHANGES FOR THIS SAVE ////////
-   add code to draw sprites from a list of the indices
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


// 4.9 notes
glm::vec3 player_position = glm::vec3(0, 0, 0);     // start at 0,0,0   // player_position = player's postion in our world
glm::vec3 player_movement = glm::vec3(0, 0, 0);     // don't go anywhere (yet)   // player_movement = where the player wants to move / how much they want to move in a direction
float player_speed = 1.0f;


GLuint playerTextureID;

// 5.10 notes
int *animIndices = new int[4]{ 3, 7, 11, 15 };        // list of indicies of the sprites we need
int animFrames = 4;     // 4 frames
int animIndex = 0;      // index for which sprite on animIndices[] is being used


// 3.16 notes
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



// 5.5 notes
void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index) {
    // can be made into parameters for function
    // in this example, will be just hardcoding them
    int cols = 4;
    int rows = 4;

    // get U,V coordinates
    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;

    // get width & height of the texture
    float width = 1.0f / (float)cols;
    float height = 1.0f / (float)rows;

    // texture coordinates and verticies you are mapping to
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v, u, v + height, u + width, v, u, v };
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    // everything below this is the same as before
    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
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

    playerTextureID = LoadTexture("george_0.png");   // loads texture
}



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




void Render() {
    glClear(GL_COLOR_BUFFER_BIT);   // clear background
    program.SetModelMatrix(modelMatrix);    // set Model Matrix

    /*
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
    */

    // 5.10 notes
    DrawSpriteFromTextureAtlas(&program, playerTextureID, animIndices[animIndex]);
    //DrawSpriteFromTextureAtlas(&program, playerTextureID, 7);     // displays just the 7th index sprite


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