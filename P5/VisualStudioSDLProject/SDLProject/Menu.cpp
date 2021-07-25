#include "Menu.h"
#define MENU_WIDTH 14
#define MENU_HEIGHT 8

#define MENU_ENEMY_COUNT 1

unsigned int menu_data[] = {
    101,  36,  36,  36,  36,  36,  36,  36,  36,  36,  99,   0,   0,   0,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,   0,   0,   0,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,   0,   0,   0,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,   0,   0,   0,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,   0,   0,   0,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,   0,   0,   0,
    100,  84,  84,  84,  84,  84,  84,  84,  84,  84, 100,   0,   0,   0,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,   0,   0,   0
};


void Menu::Initialize() {
    state.nextScene = -1;
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    GLuint mapTextureID = Util::LoadTexture("pixel_platform_01_tileset_final.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 16, 10);


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

}


void Menu::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);

}


void Menu::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("pixel_font.png");

    state.map->Render(program);
    state.player->Render(program);

    Util::DrawText(program, fontTextureID, "Lost Alien", 0.4f, 0.1f, glm::vec3(2.75f, -1.5f, 0.0f));
    Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.2f, 0.01f, glm::vec3(3.0f, -3.0f, 0.0f));
}