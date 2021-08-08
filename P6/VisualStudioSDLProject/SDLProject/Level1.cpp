#include "Level1.h"

#define OBJECT_COUNT 7
#define ENEMY_COUNT 3


void Level1::Initialize() {
    state.nextScene = -1;

    /*
    -----------------   Initialize Player  -----------------
    */
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 3.0f;

    /*
    -----------------   Initialize Objects  -----------------
    */
    state.objects = new Entity[OBJECT_COUNT];
    
    // ----- FLOOR -----
    GLuint floorTextureID = Util::LoadTexture("diamondFloor.png");
    Mesh* cubeMesh = new Mesh();
    cubeMesh->LoadOBJ("cube.obj", 30);  // 12.3 -- duplicate texture 10 times // 12.6 -- dup 20 times

    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = cubeMesh;
    state.objects[0].position = glm::vec3(0.0f, -0.25f, 0.0f);
    state.objects[0].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[0].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[0].scale = glm::vec3(30.0f, 0.5f, 30.0f);
    state.objects[0].entityType = FLOOR;

    // ----- CEILING -----
    GLuint ceilingTextureID = Util::LoadTexture("bricktext.jpg");
    Mesh* ceilingMesh = new Mesh();
    ceilingMesh->LoadOBJ("cube.obj", 30);

    state.objects[1].textureID = ceilingTextureID;
    state.objects[1].mesh = ceilingMesh;
    state.objects[1].position = glm::vec3(0.0f, 3.75f, 0.0f);
    state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[1].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[1].scale = glm::vec3(30.0f, 0.5f, 30.0f);
    state.objects[1].entityType = CEILING;


    // ----- WALLS ----- 
    // i = [2] to [5]
    GLuint wallTextureID = Util::LoadTexture("sampleWall.jpg");
    Mesh* wallMesh = new Mesh();
    wallMesh->LoadOBJ("cube.obj", 1);  // 12.3 -- duplicate texture 10 times // 12.6 -- dup 20 times

    for (size_t i = 2; i < 6; i++) {
        state.objects[i].textureID = wallTextureID;
        state.objects[i].mesh = wallMesh;

        state.objects[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[i].entityType = WALL;
    }

    state.objects[2].scale = glm::vec3(0.5f, 4.0f, 30.0f);
    state.objects[2].position = glm::vec3(15.0f, 1.5f, 0.0f);
    state.objects[2].width = 0.5f;
    state.objects[2].height = 4.0f;
    state.objects[2].depth = 30.0f;

    state.objects[3].scale = glm::vec3(0.5f, 4.0f, 30.0f);
    state.objects[3].position = glm::vec3(-15.0f, 1.5f, 0.0f);
    state.objects[3].width = 0.5f;
    state.objects[3].height = 4.0f;
    state.objects[3].depth = 30.0f;

    state.objects[4].scale = glm::vec3(30.0f, 4.0f, 0.5f);
    state.objects[4].position = glm::vec3(0.0f, 1.5f, 15.0f);
    state.objects[4].width = 30.0f;
    state.objects[4].height = 4.0f;
    state.objects[4].depth = 0.5f;

    state.objects[5].scale = glm::vec3(30.0f, 4.0f, 0.5f);
    state.objects[5].position = glm::vec3(0.0f, 1.5f, -15.0f);
    state.objects[5].width = 30.0f;
    state.objects[5].height = 4.0f;
    state.objects[5].depth = 0.5f;

    // Door
    //GLuint doorTextureID = Util::LoadTexture("DoorWood.png");
    //Mesh* doorMesh = new Mesh();   // 12.6 -- need a new one b/c floor one is duplicated
    //doorMesh->LoadOBJ("door.obj", 1);  // 12.6 -- 1 = no duplicate

    //state.objects[6].textureID = doorTextureID;
    //state.objects[6].mesh = doorMesh;
    //state.objects[6].position = glm::vec3(0.0f, 1.5f, 0.0f);   // 12.6 -- crate is 1 unit tall, stop from sinking into floor
    //state.objects[6].entityType = CRATE;

    //GLuint crateTextureID = Util::LoadTexture("crate1_diffuse.png");
    //Mesh* crateMesh = new Mesh();   // 12.6 -- need a new one b/c floor one is duplicated
    //crateMesh->LoadOBJ("cube.obj", 1);  // 12.6 -- 1 = no duplicate

    //state.objects[6].textureID = crateTextureID;
    //state.objects[6].mesh = crateMesh;
    //state.objects[6].position = glm::vec3(0.0f, 0.5f, -5.0f);   // 12.6 -- crate is 1 unit tall, stop from sinking into floor
    //state.objects[6].entityType = CRATE;

    // Health 
    GLuint healthTextureID = Util::LoadTexture("potion_blue.png");
    Mesh* healthMesh = new Mesh();   // 12.6 -- need a new one b/c floor one is duplicated
    healthMesh->LoadOBJ("potion_HighPoly.obj", 1);  // 12.6 -- 1 = no duplicate

    state.objects[6].textureID = healthTextureID;
    state.objects[6].mesh = healthMesh;
    state.objects[6].scale = glm::vec3(0.03f, 0.03f, 0.03f);
    state.objects[6].position = glm::vec3(0.0f, 0.5f, -5.0f);   // 12.6 -- crate is 1 unit tall, stop from sinking into floor
    state.objects[6].entityType = HEALTH;
    state.objects[6].isActive = false;

    //state.objects[2].textureID = crateTextureID;
    //state.objects[2].mesh = crateMesh;
    //state.objects[2].position = glm::vec3(-1.0f, 0.5f, -5.0f);   // 12.7 -- next to 1st crate
    //state.objects[2].entityType = CRATE;

    //state.objects[3].textureID = crateTextureID;
    //state.objects[3].mesh = crateMesh;
    //state.objects[3].position = glm::vec3(0.0f, 1.5f, -5.0f);   // 12.7 -- on top of 1st crate
    //state.objects[3].entityType = CRATE;

    /*
    -----------------   Initialize Enemies  -----------------
    */
    // 12.12
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ghost.png");

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].scale = glm::vec3(2.0f, 2.0f, 1.0f);
        state.enemies[i].position = glm::vec3(rand() % 20 - 10, 1.0, rand() % 20 - 10); // 0.5 = edit to have enemies touch ground
        state.enemies[i].rotation = glm::vec3(0, 0, 0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
        state.enemies[i].speed = 1.0f;
        state.enemies[i].isActive = false;
    }
    
}


void Level1::Update(float deltaTime) {
    //
    state.player->Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
    
    // 11.7 -- update cube
    for (size_t i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
    }

    // 12.13 -- update enemies
    for (size_t i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
    }

}


void Level1::Render(ShaderProgram* program) {
    // 11.7 -- render objects
    for (size_t i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Render(program);
    }

    // 12.13 -- render enemies
    for (size_t i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}