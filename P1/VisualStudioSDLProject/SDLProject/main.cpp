/*
Kathy Pan
Project 1: Simple 2D Scene
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

// load image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, cloudMatrix, cloud2Matrix, sunMatrix, groundMatrix, projectionMatrix, ballMatrix, catMatrix, dogMatrix;

// new code
float cloud_x = 0;
float cloud2_x = 0;
float sun_rotate = 0.0f;
float ground = 0.0f;
float ball_x = 0;
float ball_speed = 2.0f;
float ball_rotate = 0.0f;
float ball_rotate_dir = 1;      // takes care of the direction change
float cat = 0;
float dog = 0;
float lastTicks = 0.0f;

GLuint cloudTextureID, cloud2TextureID, sunTextureID, groundTextureID, ballTextureID, catTextureID, dogTextureID;

// don't need to change
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
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Simple 2D Scene", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    cloudMatrix = glm::mat4(1.0f);      // cloud image
    cloud2Matrix = glm::mat4(1.0f);     // cloud2 image
    sunMatrix = glm::mat4(1.0f);        // sun image
    groundMatrix = glm::mat4(1.0f);     // ground image
    ballMatrix = glm::mat4(1.0f);       // ball image
    catMatrix = glm::mat4(1.0f);        // cat image
    dogMatrix = glm::mat4(1.0f);        // dog image

    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);     // the box of the world you see

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.8f, 1.0f, 1.0f);   // background color

    // enables blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // load textures, get texture IDs
    cloudTextureID = LoadTexture("cloud.png");
    cloud2TextureID = LoadTexture("cloud.png");
    sunTextureID = LoadTexture("sun.png");
    groundTextureID = LoadTexture("dirt_grass_flowers.png");
    ballTextureID = LoadTexture("ball.png");
    catTextureID = LoadTexture("cat.png");
    dogTextureID = LoadTexture("dog.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}


void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    cloud_x += 0.7f * deltaTime;
    cloud2_x += 1.0f * deltaTime;
    sun_rotate += -90.0f * deltaTime;
    ball_x += ball_speed * deltaTime;
    ball_rotate += (-360.0f * deltaTime * ball_rotate_dir);     // ball direction

    // set matrixes to Identity matrix
    cloudMatrix = glm::mat4(1.0f);
    cloud2Matrix = glm::mat4(1.0f);
    sunMatrix = glm::mat4(1.0f);
    groundMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    catMatrix = glm::mat4(1.0f);
    dogMatrix = glm::mat4(1.0f);


    ///// SUN /////
    sunMatrix = glm::scale(sunMatrix, glm::vec3(1.5f, 1.5f, 1.0f));
    sunMatrix = glm::translate(sunMatrix, glm::vec3(-2.8f, 2.0f, 0.0f));
    sunMatrix = glm::rotate(sunMatrix, glm::radians(sun_rotate), glm::vec3(0.0f, 0.0f, 1.0f));


    ///// CLOUDS /////
    cloudMatrix = glm::scale(cloudMatrix, glm::vec3(1.5f, 1.5f, 1.0f));
    cloudMatrix = glm::translate(cloudMatrix, glm::vec3(0.0f, 1.7f, 0.0f));
    cloudMatrix = glm::translate(cloudMatrix, glm::vec3(cloud_x, 0.0f, 0.0f));     // cloud moves

    //cloud2Matrix = glm::scale(cloud2Matrix, glm::vec3(1.0f, 1.0f, 1.0f));
    cloud2Matrix = glm::translate(cloud2Matrix, glm::vec3(0.0f, 2.0f, 0.0f));
    cloud2Matrix = glm::translate(cloud2Matrix, glm::vec3(cloud2_x, 0.0f, 0.0f));     // cloud moves

    // clouds leave window on right side, re-enter window on left side
    if ((cloud_x <= -5.0f) || (cloud_x >= 5.0f)) {
        cloud_x = cloud_x * -1;
    }
    if ((cloud2_x <= -6.0f) || (cloud2_x >= 6.0f)) {
        cloud2_x = cloud2_x * -1;
    }


    ///// GROUND /////
    groundMatrix = glm::scale(groundMatrix, glm::vec3(10.0f, 2.0f, 1.0f));
    groundMatrix = glm::translate(groundMatrix, glm::vec3(0.0f, -1.375f, 0.0f));


    ///// BALL /////
    ballMatrix = glm::translate(ballMatrix, glm::vec3(0.0f, -2.1f, 0.0f));
    ballMatrix = glm::translate(ballMatrix, glm::vec3(ball_x, 0.0f, 0.0f));     // ball moves
    ballMatrix = glm::rotate(ballMatrix, glm::radians(ball_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

    // ball stays in window between cat and dog
    // changes the current direction the ball is translating in 
    // changes the current direction the ball is rotating in
    if ((ball_x <= -2.3f) || (ball_x >= 2.5f)) {
        ball_speed = ball_speed * -1;
        ball_rotate_dir = ball_rotate_dir * -1;
    }


    ///// CAT /////
    catMatrix = glm::scale(catMatrix, glm::vec3(1.5f, 1.5f, 1.0f));
    catMatrix = glm::translate(catMatrix, glm::vec3(-2.2f, -1.2f, 0.0f));


    ///// DOG /////
    dogMatrix = glm::scale(dogMatrix, glm::vec3(1.5f, 1.5f, 1.0f));
    dogMatrix = glm::translate(dogMatrix, glm::vec3(2.2f, -1.2f, 0.0f));

}


void displaySun() {
    program.SetModelMatrix(sunMatrix);
    glBindTexture(GL_TEXTURE_2D, sunTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void displayCloud() {
    program.SetModelMatrix(cloudMatrix);
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void displayCloud2() {
    program.SetModelMatrix(cloud2Matrix);
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void displayGround() {
    program.SetModelMatrix(groundMatrix);
    glBindTexture(GL_TEXTURE_2D, groundTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void displayBall() {
    program.SetModelMatrix(ballMatrix);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void displayCat() {
    program.SetModelMatrix(catMatrix);
    glBindTexture(GL_TEXTURE_2D, catTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void displayDog() {
    program.SetModelMatrix(dogMatrix);
    glBindTexture(GL_TEXTURE_2D, dogTextureID);
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

    displaySun();
    displayCloud();
    displayCloud2();
    displayGround();
    displayBall();
    displayCat();
    displayDog();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

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