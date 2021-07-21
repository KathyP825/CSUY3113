#include "Level2.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 1

unsigned int level2_data[] = {
    101,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  99,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,
    100,  84,  84,  84,  84,  84,  84,  84,  84,  84,  84,  84,  84, 100
};


void Level2::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("pixel_platform_02_tileset_final.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 16, 10);

    // Initialize Game Objects

    /*
    -----------------   Initialize Player    -----------------
    */
    state.player = new Entity();
    state.player->entitytype = PLAYER;      // 8.7 -- initilize with PLAYER entity type
    state.player->position = glm::vec3(5.0f, -3.0f, 0.0f);     // 8.5 -- initialize player at bottom left
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);   // 6.6 -- set acceleration and never changing this value
    state.player->speed = 2.0f;     // 6.13 -- increase player speed
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
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemies[0].entitytype = ENEMY;        // 8.7 -- initilize with ENEMY entity type
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4.0f, -2.25f, 0.0f);
    state.enemies[0].speed = 1.0f;

    state.enemies[0].aiType = WAITANDGO;    // 8.8 -- initialize the AIType to 2nd type
    state.enemies[0].aiState = IDLE;        // 8.8 -- initialize the AIState
    state.enemies[0].isActive = false;      // 9.11 -- deactivate 
}


void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    if (state.player->position.x >= 12.0f) {
        state.nextScene = 3;
    }
}


void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}