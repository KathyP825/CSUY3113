#include "Menu.h"
#define MENU_WIDTH 14
#define MENU_HEIGHT 8

#define MENU_ENEMY_COUNT 1

unsigned int menu_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0
};


void Menu::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 4, 1);


    /*
    -----------------   Initialize Player    -----------------
    */
    state.player = new Entity();
    state.player->entitytype = PLAYER;      // 8.7 -- initilize with PLAYER entity type
    state.player->position = glm::vec3(5.0f, -3.0f, 0.0f);     // 8.5 -- initialize player at bottom left
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);   // 6.6 -- set acceleration and never changing this value
    state.player->speed = 2.0f;     // 6.13 -- increase player speed
    state.player->textureID = Util::LoadTexture("george_0.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    //6.11 -- fix character hovering
    state.player->height = 0.8f;    // num can vary
    state.player->width = 0.8f;     // not necessary, but can if you want

    // 6.12 -- set up for jump
    state.player->jumpPower = 6.0f;

}


void Menu::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);

    if (state.player->position.x >= 10.0f) {
        state.nextScene = 1;
    }
}


void Menu::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("pixel_font.png");

    Util::DrawText(program, fontTextureID, "Game Name", 0.4f, 0.1f, glm::vec3(2.5f, -1.5f, 0.0f));
    Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.2f, 0.01f, glm::vec3(3.0f, -3.0f, 0.0f));

    state.map->Render(program);
    state.player->Render(program);
}