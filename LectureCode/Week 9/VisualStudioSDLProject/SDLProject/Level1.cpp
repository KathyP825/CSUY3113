#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};


void Level1::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    
    // Initialize Game Objects

    /*
    -----------------   Initialize Player    -----------------
    */
    state.player = new Entity();
    state.player->entitytype = PLAYER;      // 8.7 -- initilize with PLAYER entity type
    state.player->position = glm::vec3(5.0f, 0.0f, 0.0f);     // 8.5 -- initialize player at bottom left
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


    /*
    -----------------   Initialize Enemies    -----------------
    */
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemies[0].entitytype = ENEMY;        // 8.7 -- initilize with ENEMY entity type
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4.0f, -2.25f, 0.0f);
    state.enemies[0].speed = 1.0f;

    state.enemies[0].aiType = WAITANDGO;    // 8.8 -- initialize the AIType to 2nd type
    state.enemies[0].aiState = IDLE;        // 8.8 -- initialize the AIState
    state.enemies[0].isActive = false;      // 9.11 -- deactivate 
}


void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

    if (state.player->position.x >= 12.0f) {
        state.nextScene = 1;
    }
}


void Level1::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}