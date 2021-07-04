/*
Kathy Pan
Project 2: Pong!
Updated to add sound
*/


#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h>  // 7.10 main.cpp - need SDL2_mixer.dll and smpeg2.dll
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program;
glm::mat4 viewMatrix, ballMatrix, leftPaddleMatrix, rightPaddleMatrix, projectionMatrix;


//----------- Initialize Object Settings
glm::vec3 leftPaddle_position = glm::vec3(-9.5f, 0.0f, 0.0f);
glm::vec3 leftPaddle_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rightPaddle_position = glm::vec3(9.5f, 0.0f, 0.0f);
glm::vec3 rightPaddle_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 paddle_scaled = glm::vec3(0.5f, 1.5f, 1.0f);      // resize paddle, has new height & width
float paddle_width = paddle_scaled.x;
float paddle_height = paddle_scaled.y;
float paddle_speed = 2.0f;

glm::vec3 ball_position = glm::vec3(0.0f, 0.0f, 0.0f);     // can get ballX and ballY
glm::vec3 ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 ball_scaled = glm::vec3(0.5f, 0.5f, 1.0f);    // resize ball, has new height & width
float ball_width = ball_scaled.x;
float ball_height = ball_scaled.y;
float ball_speed = 1.0f;

bool playing = false;   // game status

GLuint ballTextureID, leftPaddleTextureID, rightPaddleTextureID;


Mix_Music* music;   // 7.12 main.cpp - pointer for music

Mix_Chunk* bounce;  // 7.13 main.cpp - pointer for sound effects


//----------- DO NOT TOUCH
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
    //SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);  // 7.10 main.cpp - allows both video and sound

    displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 640, 480);
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");


    // can initilialize audio first so if game takes long to load, can start music playing
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);  // 7.10 main.cpp - initialize audio
    music = Mix_LoadMUS("dooblydoo.mp3");               // 7.12 main.cpp - load music and start playing
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);                // 7.13 main.cpp - change music volume

    bounce = Mix_LoadWAV("bounce.wav");     // 7.12 main.cpp - initialize sound effects


    viewMatrix = glm::mat4(1.0f);
    leftPaddleMatrix = glm::mat4(1.0f);
    rightPaddleMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);     // the box of the world you see

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    glUseProgram(program.programID);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);   // background color, when clear the window use this color

    // enables blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load Textures
    leftPaddleTextureID = LoadTexture("paddle.png");
    rightPaddleTextureID = LoadTexture("paddle.png");
    ballTextureID = LoadTexture("earth.png");

    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);  // 7.10 main.cpp - can initialize audio after other initilizations
}



// if don't handle anything, nothing happens
void ProcessInput() {
    //---- Stops paddles from moving when nothing is pressed
    leftPaddle_movement = glm::vec3(0);
    rightPaddle_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {     // while there are events to process
        switch (event.type) {           // checks types of events 
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:    // pressing Space key starts the game, enables paddles
                playing = true;
                ball_movement = glm::vec3(-2.0f, 3.0f, 0.0f);
                break;
            }
            break; // SDL_KEYDOWN
        }
    }


    // 1. check if continuously holding down key
    // 2. checks to keep paddles inside window
    if (playing == true) {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        // -2.75f < paddle_position < 2.75  ---->   stop paddle from leaving screen

        // W,S keys
        if (keys[SDL_SCANCODE_W]) {
            if ((leftPaddle_position.y + (paddle_height / 2.0f)) < 2.75f) {
                leftPaddle_movement.y = 1.0f;
            }
        }
        else if (keys[SDL_SCANCODE_S]) {
            if ((leftPaddle_position.y - (paddle_height / 2.0f)) > -2.75f) {
                leftPaddle_movement.y = -1.0f;
            }
        }

        // Arrow keys
        if (keys[SDL_SCANCODE_UP]) {
            if ((rightPaddle_position.y + (paddle_height / 2.0f)) < 2.75f) {
                rightPaddle_movement.y = 1.0f;
            }
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            if ((rightPaddle_position.y - (paddle_height / 2.0f)) > -2.75f) {
                rightPaddle_movement.y = -1.0f;
            }
        }


        //------  Normalize  ------
        if (glm::length(leftPaddle_movement) > 1.0f) {
            leftPaddle_movement = glm::normalize(leftPaddle_movement);
        }

        if (glm::length(rightPaddle_movement) > 1.0f) {
            rightPaddle_movement = glm::normalize(rightPaddle_movement);
        }
    }

}



float lastTicks = 0.0f;   // ticks = amount of time since SDL was last initialized

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    leftPaddle_position += leftPaddle_movement * paddle_speed * deltaTime;
    rightPaddle_position += rightPaddle_movement * paddle_speed * deltaTime;
    ball_position += ball_movement * ball_speed * deltaTime;

    //------  Left Paddle  ------
    leftPaddleMatrix = glm::mat4(1.0f);
    leftPaddleMatrix = glm::scale(leftPaddleMatrix, paddle_scaled);
    leftPaddleMatrix = glm::translate(leftPaddleMatrix, leftPaddle_position);

    //------  Right Paddle  ------
    rightPaddleMatrix = glm::mat4(1.0f);
    rightPaddleMatrix = glm::scale(rightPaddleMatrix, paddle_scaled);
    rightPaddleMatrix = glm::translate(rightPaddleMatrix, rightPaddle_position);

    //------  Ball  ------
    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::scale(ballMatrix, ball_scaled);
    ballMatrix = glm::translate(ballMatrix, ball_position);


    //------  COLLISION CHECK  ------
    // check if hit left/right walls, if not then continue playing
    if ((ball_position.x < -10.0f || ball_position.x > 10.0f) == false) {     // checks for LEFT / RIGHT walls
        /*
        1 = paddle
        2 = ball
        subtract 0.5 & 0.75 ----- stop ball from going through paddle
        */

        if (ball_position.x < 0.0f) {   //------  LEFT PADDLE  ------
            float xdist = fabs(ball_position.x - leftPaddle_position.x) - ((paddle_width + ball_width) / 2.0f) - 0.5f;
            float ydist = fabs(ball_position.y - leftPaddle_position.y) - ((paddle_height + ball_height) / 2.0f) - 0.75f;

            if (xdist < 0 && ydist < 0) {
                // ball_movement.x == ball_movement[0]
                ball_movement.x *= -1.0f;
                // ball_position.x == ball_position[0]
                ball_position[0] += 0.1f;

                Mix_PlayChannel(-1, bounce, 0);     // 7.13 main.cpp - if hit left paddle, play bounce
            }
        }
        else if (ball_position.x > 0.0f) {  //------  RIGHT PADDLE  ------
            float xdist = fabs(ball_position.x - rightPaddle_position.x) - ((paddle_width + ball_width) / 2.0f) - 0.5f;
            float ydist = fabs(ball_position.y - rightPaddle_position.y) - ((paddle_height + ball_height) / 2.0f) - 0.75f;

            if (xdist < 0 && ydist < 0) {
                // ball_movement.x == ball_movement[0]
                ball_movement[0] *= -1.0f;
                // ball_position.x == ball_position[0]
                ball_position[0] -= 0.1f;

                Mix_PlayChannel(-1, bounce, 0);     // 7.13 main.cpp - if hit right paddle, play sound
            }
        }


        // checks for TOP / BOTTOM of window
        // if touch, bounce back
        if ((ball_position.y <= -7.0f) || (ball_position.y >= 7.0f)) {
            ball_movement.y *= -1.0f;
            ball_position.y += ball_movement.y * 0.1f;

            Mix_PlayChannel(-1, bounce, 0);     // 7.13 main.cpp - if hit top/bottom walls, play sound
        }

    }
    else {
        // hit left/right walls
        // stop game
        playing = false;
        ball_speed = 0.0f;
    }

}




void displayLeftPaddle() {
    program.SetModelMatrix(leftPaddleMatrix);
    glBindTexture(GL_TEXTURE_2D, leftPaddleTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void displayRightPaddle() {
    program.SetModelMatrix(rightPaddleMatrix);
    glBindTexture(GL_TEXTURE_2D, rightPaddleTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void displayBall() {
    program.SetModelMatrix(ballMatrix);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);   // clear background

    // verticies & texture coordinates for the 2 triangles
    float verticies[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    // tell to use this list of coordinates
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticies);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    displayLeftPaddle();
    displayRightPaddle();
    displayBall();

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