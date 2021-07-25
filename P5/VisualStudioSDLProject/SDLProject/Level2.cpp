#include "Level2.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 1

unsigned int level2_data[] = {
     36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
      2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 141,   0,
     85,   0,   0,   0,   0,   0,   0,   0,   0,  35,  36,  36,  36,  85,
    101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  99,
    101,   0,   0,  20,   0,   0,   0,  83,  85,   0,   0,   0,   0,  99,
    101,   0,   0,   0,   0,   0,  83, 100, 100,  85,   0,   0,   0, 133,
    101,   0,   0,   0,   0,  83, 100, 100, 100, 101,   0,   0, 141,   0,
    100,  84,  84,  84,  84, 100, 100, 100, 100, 100,  84,  84,  84,  84
};


void Level2::Initialize() {
    state.nextScene = -1;

    glClearColor(0.529f, 0.808f, 0.980f, 1.0f);

    GLuint mapTextureID = Util::LoadTexture("pixel_platform_02_tileset_final.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 16, 10);


    /*
    -----------------   Initialize Player    -----------------
    */
    state.player = new Entity();
    state.player->entitytype = PLAYER;
    state.player->position = glm::vec3(1.0f, -1.0f, 0.0f);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
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
    GLuint enemyTextureID = Util::LoadTexture("characters.png");

    state.enemies[0].entitytype = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(9.0f, -0.1f, 0.0f);
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

    state.enemies[0].height = 0.8f;
    state.enemies[0].width = 0.8f;

    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
}


void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    for (size_t i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }

    if (state.player->position.x >= 11.0f && state.player->position.y <= -5.5f) {
        state.nextScene = 3;
    }
}


void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);

    state.enemies[0].Render(program);
}