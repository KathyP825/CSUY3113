/*
Kathy Pan
Project 3: Lunar Lander
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
#include <vector>   // need for the DrawText to work

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 25    // 25 platforms
//#define LANDINGGOAL_COUNT 2


struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* landingGoal;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

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



// looping through each character in the string
// create 2 triangles for each character
// gets the coordinates that goes with the 2 triangles
// size is not font size  -->  size = size of letter relative to the player
// spacing = amount of space between characters
// position = where in the game world should the strip of triangles be drawn

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    // calculate U,V coordinates
    // 16 rows and columns
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    // vector = an array you can add to on the fly instead of a fixed length array
    std::vector<float> vertices;
    std::vector<float> texCoords;

    // for each character
    // get index
    // get U,V coordinates 
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;

        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;

        //-----  Set up the vertices and texCoords  -----
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
            });

    } // end of for loop

    // draw using familiar code after vertices and texCoords are set up
    // the string is a model like everything else
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    // vertices.data() makes the vector into a normal array so it can be used
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));  // (int)(text.size() * 6) == num of verticies we're trying to render

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);


}




void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);       // background color
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //---------- Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = EntityType::PLAYER;
    state.player->position = glm::vec3(0.0f, 3.25f, 0.0f);       // player starting position
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.01f, 0);   // 6.6 -- set acceleration and never changing this value
    state.player->speed = 2.0f;     // 6.13 -- increase player speed
    state.player->textureID = LoadTexture("comets.png");

    state.player->animRight = new int[1]{ 1 };
    state.player->animLeft = new int[1]{ 2 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 1;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 2;
    state.player->animRows = 1;

    //6.11 -- fix character hovering
    state.player->height = 0.8f;    // num can vary
    state.player->width = 0.8f;     // not necessary, but can if you want


    // 6.8 -- set up platforms
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("lava.png");
    GLuint goalTextureID = LoadTexture("goal_tile.png");

    // ----------- left wall
    state.platforms[0].textureID = platformTextureID;
    state.platforms[0].entityType = EntityType::PLATFORM;
    state.platforms[0].position = glm::vec3(-5.0f, 3.75f, 0);

    state.platforms[1].textureID = platformTextureID;
    state.platforms[1].entityType = EntityType::PLATFORM;
    state.platforms[1].position = glm::vec3(-5.0f, 2.75f, 0);

    state.platforms[2].textureID = platformTextureID;
    state.platforms[2].entityType = EntityType::PLATFORM;
    state.platforms[2].position = glm::vec3(-5.0f, 1.75f, 0);

    state.platforms[3].textureID = platformTextureID;
    state.platforms[3].entityType = EntityType::PLATFORM;
    state.platforms[3].position = glm::vec3(-5.0f, 0.75f, 0);

    state.platforms[4].textureID = platformTextureID;
    state.platforms[4].entityType = EntityType::PLATFORM;
    state.platforms[4].position = glm::vec3(-5.0f, -0.25f, 0);

    state.platforms[5].textureID = platformTextureID;
    state.platforms[5].entityType = EntityType::PLATFORM;
    state.platforms[5].position = glm::vec3(-5.0f, -1.25f, 0);

    state.platforms[6].textureID = platformTextureID;
    state.platforms[6].entityType = EntityType::PLATFORM;
    state.platforms[6].position = glm::vec3(-5.0f, -2.25f, 0);

    // ------------- right wall
    state.platforms[7].textureID = platformTextureID;
    state.platforms[7].entityType = EntityType::PLATFORM;
    state.platforms[7].position = glm::vec3(5.0f, 3.75f, 0);

    state.platforms[8].textureID = platformTextureID;
    state.platforms[8].entityType = EntityType::PLATFORM;
    state.platforms[8].position = glm::vec3(5.0f, 2.75f, 0);

    state.platforms[9].textureID = platformTextureID;
    state.platforms[9].entityType = EntityType::PLATFORM;
    state.platforms[9].position = glm::vec3(5.0f, 1.75f, 0);

    state.platforms[10].textureID = platformTextureID;
    state.platforms[10].entityType = EntityType::PLATFORM;
    state.platforms[10].position = glm::vec3(5.0f, 0.75f, 0);

    state.platforms[11].textureID = platformTextureID;
    state.platforms[11].entityType = EntityType::PLATFORM;
    state.platforms[11].position = glm::vec3(5.0f, -0.25f, 0);

    state.platforms[12].textureID = platformTextureID;
    state.platforms[12].entityType = EntityType::PLATFORM;
    state.platforms[12].position = glm::vec3(5.0f, -1.25f, 0);

    state.platforms[13].textureID = platformTextureID;
    state.platforms[13].entityType = EntityType::PLATFORM;
    state.platforms[13].position = glm::vec3(5.0f, -2.25f, 0);

    //--------- bottom platform
    state.platforms[14].textureID = platformTextureID;
    state.platforms[14].entityType = EntityType::PLATFORM;
    state.platforms[14].position = glm::vec3(5.0f, -3.25f, 0);

    state.platforms[15].textureID = platformTextureID;
    state.platforms[15].entityType = EntityType::PLATFORM;
    state.platforms[15].position = glm::vec3(-5.0f, -3.25f, 0);

    state.platforms[16].textureID = platformTextureID;
    state.platforms[16].entityType = EntityType::PLATFORM;
    state.platforms[16].position = glm::vec3(4.0f, -3.25f, 0);

    state.platforms[17].textureID = platformTextureID;
    state.platforms[17].entityType = EntityType::PLATFORM;
    state.platforms[17].position = glm::vec3(-4.0f, -3.25f, 0);

    state.platforms[18].textureID = platformTextureID;
    state.platforms[18].entityType = EntityType::PLATFORM;
    state.platforms[18].position = glm::vec3(-3.0f, -3.25f, 0);

    state.platforms[19].textureID = platformTextureID;      // originally next to goal
    state.platforms[19].entityType = EntityType::PLATFORM;
    state.platforms[19].position = glm::vec3(3.0f, -3.25f, 0);

    state.platforms[20].textureID = platformTextureID;
    state.platforms[20].entityType = EntityType::PLATFORM;
    state.platforms[20].position = glm::vec3(-2.0f, -3.25f, 0);

    state.platforms[21].textureID = goalTextureID;  // MISSION GOAL
    state.platforms[21].entityType = EntityType::LANDINGGOAL;
    state.platforms[21].position = glm::vec3(2.0f, -3.25f, 0);

    state.platforms[22].textureID = platformTextureID;
    state.platforms[22].entityType = EntityType::PLATFORM;
    state.platforms[22].position = glm::vec3(-1.0f, -3.25f, 0);

    state.platforms[23].textureID = platformTextureID;      // originally next to goal
    state.platforms[23].entityType = EntityType::PLATFORM;
    state.platforms[23].position = glm::vec3(1.0f, -3.25f, 0);

    state.platforms[24].textureID = platformTextureID;
    state.platforms[24].entityType = EntityType::PLATFORM;
    state.platforms[24].position = glm::vec3(0.0f, -3.25f, 0);  // center


    for (size_t i = 0; i < PLATFORM_COUNT; i++) {   // only updates platforms 1x
        state.platforms[i].Update(0, NULL, 0);
    
    }
}



void ProcessInput() {

    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
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
                break;

            case SDLK_SPACE:
                // jump
                // 6.21 -- only jump if colliding on bottom
                //if (state.player->collidedBottom){
                //    state.player->jump = true;  // 6.12
                //}
                
                //state.player->jump = true;  // 6.12
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        //state.player->movement.x = -1.0f;
        state.player->acceleration.x -= 1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        //state.player->movement.x = 1.0f;
        state.player->acceleration.x += 1.0f;
        state.player->animIndices = state.player->animRight;
    }


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}



// 6.5 -- replace Update() to fixed timestate version
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

    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}




void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint fontTextureID = LoadTexture("font.png");

    // render and display all platforms
    for (size_t i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    if (state.player->isWinner == 1) {
        DrawText(&program, fontTextureID, "Mission Success!",0.5f, -0.25f, glm::vec3(-1.8f, 1.0f, 0));
    }
    else if (state.player->isWinner == 2) {
        DrawText(&program, fontTextureID, "Mission Failed! ", 0.5f, -0.25f, glm::vec3(-1.8f, 1.0f, 0));
    }


    state.player->Render(&program);     // display player

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