#include "WinScreen.h"
#define WinScreen_WIDTH 14
#define WinScreen_HEIGHT 8

unsigned int winscreen_data[] = {
    12,  13,  14,  13,  14,  13,  14,  13,  14,  14,  15,   0,   0,   0,
    28,  29,  30,  29,  30,  29,  30,  29,  30,  30,  31,   0,   0,   0,
    28,  29,  30,  29,  30,  29,  30,  29,  30,  30,  31,   0,   0,   0,
    28,  29,  30,  29,  30,  29,  30,  29,  30,  30,  31,   0,   0,   0,
    28,  29,  30,  29,  30,  29,  30,  29,  30,  30,  31,   0,   0,   0,
    28,  29,  30,  29,  30,  29,  30,  29,  30,  30,  31,   0,   0,   0,
    28,  29,  30,  29,  30,  29,  30,  29,  30,  30,  31,   0,   0,   0,
    44,  45,  46,  45,  46,  45,  46,  45,  46,  46,  47,   0,   0,   0
};


void WinScreen::Initialize() {
    state.nextScene = -1;
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    GLuint mapTextureID = Util::LoadTexture("pixel_platform_03_tileset_final.png");
    state.map = new Map(WinScreen_WIDTH, WinScreen_HEIGHT, winscreen_data, mapTextureID, 1.0f, 16, 10);


    /*
    -----------------   Initialize Player    -----------------
    */
    state.player = new Entity();
    state.player->entitytype = PLAYER;
    state.player->position = glm::vec3(5.0f, -3.0f, 0.0f);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.5f;
    state.player->textureID = Util::LoadTexture("characters.png");

    state.player->animRight = new int[2]{ 4, 5 };
    state.player->animLeft = new int[2]{ 4, 5 };
    state.player->animUp = new int[2]{ 4, 5 };
    state.player->animDown = new int[2]{ 4, 5 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 2;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 9;
    state.player->animRows = 3;

    state.player->height = 0.9f;
    state.player->width = 0.8f;

    state.player->jumpPower = 6.0f;
    state.player->isActive = false;
}


void WinScreen::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);

}


void WinScreen::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("pixel_font.png");

    state.map->Render(program);
    state.player->Render(program);

    Util::DrawText(program, fontTextureID, "You Win!", 0.4f, 0.1f, glm::vec3(3.25f, -3.0f, 0.0f));
    Util::DrawText(program, fontTextureID, "Alien escaped!", 0.2f, 0.01f, glm::vec3(3.55f, -4.0f, 0.0f));
}