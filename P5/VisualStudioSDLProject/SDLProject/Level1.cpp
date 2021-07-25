#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] = {
    101,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 141,   0,
     17,   0,   0,   0,  35,  37,   0,   0,   0,  35,  36,  36,  36,  52,
     49,  85,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,
    100, 101,   0,   0,   0,   0,  23,  52,   0,   0,   0,   0,   0,  99,
    100, 100,  85,   0,   0,  83,  84, 100,  85,   0,   0,   0,  35,  99,
    101,   0,   0,   0,  83, 100, 100, 100, 100,  85,   0,   0, 141, 133,
    100,  84,  84,  84,  84,  84,  84,  84,  84,  84,  84,  84,  84,  84
};


void Level1::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("pixel_platform_03_tileset_final.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 16, 10);


    /*
    -----------------   Initialize Player    -----------------
    */
    state.player = new Entity();
    state.player->entitytype = PLAYER;
    state.player->position = glm::vec3(1.0f, -2.0f, 0.0f);

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


    /*
    -----------------   Initialize Enemies    -----------------
    */
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("characters.png");

    state.enemies[0].entitytype = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(5.0f, -1.0f, 0.0f);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].speed = 1.0f;

    state.enemies[0].animRight = new int[2]{ 9, 10 };
    state.enemies[0].animLeft = new int[2]{ 9, 10 };
    state.enemies[0].animUp = new int[2]{ 9, 10 };
    state.enemies[0].animDown = new int[2]{ 9, 10 };

    state.enemies[0].animIndices = state.enemies[0].animRight;
    state.enemies[0].animFrames = 2;
    state.enemies[0].animIndex = 0;
    state.enemies[0].animCols = 9;
    state.enemies[0].animRows = 3;

    state.enemies[0].height = 0.9f;
    state.enemies[0].width = 0.8f;

    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
}


void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

    for (size_t i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }

    // player must reach the signboard to advance
    // next to signboard triggers
    if (state.player->position.x >= 11.0f && state.player->position.y >= -1.2f) {
        state.nextScene = 2;
    }
}


void Level1::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);

    state.enemies[0].Render(program);
}