#include "Level1.h"

#define OBJECT_COUNT 31
#define ENEMY_COUNT 3


void Level1::Initialize() {
    state.nextScene = -1;

    /*
    -----------------   Initialize Player  -----------------
    */
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);    // return to 0.75
    state.player->width = 0.5;
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 3.0f;

    /*
    -----------------   Initialize Objects  -----------------
    */
    state.objects = new Entity[OBJECT_COUNT];
    
    // ----- FLOOR -----
    GLuint floorTextureID = Util::LoadTexture("diamondFloor.png");
    Mesh* cubeMesh = new Mesh();
    cubeMesh->LoadOBJ("cube.obj", 50);  // 12.3 -- duplicate texture 50 times

    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = cubeMesh;
    state.objects[0].position = glm::vec3(0.0f, -0.25f, 0.0f);
    state.objects[0].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[0].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[0].scale = glm::vec3(50.0f, 0.5f, 50.0f);     // floor is 50 x 50
    state.objects[0].entityType = FLOOR;

    // ----- CEILING -----
    GLuint ceilingTextureID = Util::LoadTexture("bricktext.jpg");
    Mesh* ceilingMesh = new Mesh();
    ceilingMesh->LoadOBJ("cube.obj", 30);

    state.objects[1].textureID = ceilingTextureID;
    state.objects[1].mesh = ceilingMesh;
    state.objects[1].position = glm::vec3(0.0f, 8.0f, 0.0f);
    state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[1].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[1].scale = glm::vec3(50.0f, 0.5f, 50.0f);
    state.objects[1].entityType = CEILING;


    // ----- Outer Walls ----- 
    // i = [2] to [5]
    GLuint wallTextureID = Util::LoadTexture("ch01_island_iron.png");
    Mesh* wallMesh = new Mesh();
    wallMesh->LoadOBJ("cube.obj", 1);  // 12.3 -- duplicate texture 10 times // 12.6 -- dup 20 times

    for (size_t i = 2; i < 6; i++) {
        state.objects[i].textureID = wallTextureID;
        state.objects[i].mesh = wallMesh;

        state.objects[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[i].entityType = WALL;
    }

    state.objects[2].scale = glm::vec3(0.5f, 10.0f, 50.0f);
    state.objects[2].position = glm::vec3(25.0f, 3.0f, 0.0f);
    state.objects[2].width = 0.5f;
    state.objects[2].height = 10.0f;
    state.objects[2].depth = 50.0f;

    state.objects[3].scale = glm::vec3(0.5f, 10.0f, 50.0f);
    state.objects[3].position = glm::vec3(-25.0f, 3.0f, 0.0f);
    state.objects[3].width = 0.5f;
    state.objects[3].height = 10.0f;
    state.objects[3].depth = 50.0f;

    state.objects[4].scale = glm::vec3(50.0f, 10.0f, 0.5f);
    state.objects[4].position = glm::vec3(0.0f, 3.0f, 25.0f);
    state.objects[4].width = 50.0f;
    state.objects[4].height = 10.0f;
    state.objects[4].depth = 0.5f;

    state.objects[5].scale = glm::vec3(50.0f, 10.0f, 0.5f);
    state.objects[5].position = glm::vec3(0.0f, 3.0f, -25.0f);
    state.objects[5].width = 50.0f;
    state.objects[5].height = 10.0f;
    state.objects[5].depth = 0.5f;

    // ----- EXIT (Indicator Flag & Door) ----- 
    GLuint flagTextureID = Util::LoadTexture("flagArrow.png");
    Mesh* flagMesh = new Mesh();
    flagMesh->LoadOBJ("cube.obj", 1);

    state.objects[6].textureID = flagTextureID;
    state.objects[6].mesh = flagMesh;
    state.objects[6].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[6].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[6].entityType = CRATE;

    state.objects[6].scale = glm::vec3(1.0f, 2.0f, 1.0f);
    state.objects[6].position = glm::vec3(0.0f, 6.5f, -24.0f);
    state.objects[6].width = 1.0f;
    state.objects[6].height = 2.0f;
    state.objects[6].depth = 1.0f;

    GLuint doorTextureID = Util::LoadTexture("castledoors.png");
    Mesh* doorMesh = new Mesh();
    doorMesh->LoadOBJ("cube.obj", 1);

    state.objects[7].textureID = doorTextureID;
    state.objects[7].mesh = doorMesh;
    state.objects[7].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[7].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[7].entityType = GOAL;

    state.objects[7].scale = glm::vec3(2.0f, 4.0f, 0.2f);
    state.objects[7].position = glm::vec3(0.0f, 2.0f, -24.8f);
    state.objects[7].width = 2.0f;
    state.objects[7].height = 4.0f;
    state.objects[7].depth = 0.2f;

    // ----- Maze Walls ----- 
    GLuint crateTextureID = Util::LoadTexture("crate1_diffuse.png");
    Mesh* crateMesh = new Mesh();   // 12.6 -- need a new one b/c floor one is duplicated
    crateMesh->LoadOBJ("cube.obj", 1);  // 12.6 -- 1 = no duplicate

    for (size_t i = 8; i < OBJECT_COUNT; i++) {     // change obj count to num of walls later9
        state.objects[i].textureID = crateTextureID;
        state.objects[i].mesh = crateMesh;

        state.objects[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[i].entityType = WALL;
    }

    state.objects[8].scale = glm::vec3(5.0f, 1.0f, 0.2f);
    state.objects[8].position = glm::vec3(0.0f, 0.5f, 2.5f);   // behind player
    state.objects[8].width = 5.0f;
    state.objects[8].height = 1.0f;
    state.objects[8].depth = 0.2f;

    state.objects[9].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[9].position = glm::vec3(2.5f, 0.5f, 0.0f);    // player right
    state.objects[9].width = 0.2f;
    state.objects[9].height = 1.0f;
    state.objects[9].depth = 5.0f;

    state.objects[10].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[10].position = glm::vec3(-2.5f, 0.5f, 0.0f);    // player left
    state.objects[10].width = 0.2f;
    state.objects[10].height = 1.0f;
    state.objects[10].depth = 5.0f;

    state.objects[11].scale = glm::vec3(0.2f, 1.0f, 4.0f);
    state.objects[11].position = glm::vec3(1.5f, 0.5f, -23.0f);    // door right
    state.objects[11].width = 0.2f;
    state.objects[11].height = 1.0f;
    state.objects[11].depth = 4.0f;

    state.objects[12].scale = glm::vec3(0.2f, 1.0f, 8.0f);
    state.objects[12].position = glm::vec3(-1.5f, 0.5f, -21.0f);    // door left
    state.objects[12].width = 0.2f;
    state.objects[12].height = 1.0f;
    state.objects[12].depth = 8.0f;

    state.objects[13].scale = glm::vec3(8.0f, 1.0f, 0.2f);
    state.objects[13].position = glm::vec3(2.5f, 0.5f, -17.0f);   // covers door
    state.objects[13].width = 8.0f;
    state.objects[13].height = 1.0f;
    state.objects[13].depth = 0.2f;

    // Upper Right corner
    state.objects[14].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[14].position = glm::vec3(15.0f, 0.5f, -22.5f);   // outer wall
    state.objects[14].width = 0.2f;
    state.objects[14].height = 1.0f;
    state.objects[14].depth = 5.0f;

    state.objects[15].scale = glm::vec3(14.0f, 1.0f, 0.2f);
    state.objects[15].position = glm::vec3(19.0f, 0.5f, -15.0f);   // horizontal wall
    state.objects[15].width = 14.0f;
    state.objects[15].height = 1.0f;
    state.objects[15].depth = 0.2f;

    // connect upper right corner to exit
    state.objects[16].scale = glm::vec3(0.2f, 1.0f, 11.0f);
    state.objects[16].position = glm::vec3(9.0f, 0.5f, -15.5f);   // outer wall
    state.objects[16].width = 0.2f;
    state.objects[16].height = 1.0f;
    state.objects[16].depth = 11.0f;

    state.objects[17].scale = glm::vec3(2.5f, 1.0f, 0.2f);
    state.objects[17].position = glm::vec3(7.75f, 0.5f, -21.0f);   // horizontal wall
    state.objects[17].width = 2.5f;
    state.objects[17].height = 1.0f;
    state.objects[17].depth = 0.2f;

    state.objects[18].scale = glm::vec3(0.2f, 1.0f, 6.0f);
    state.objects[18].position = glm::vec3(6.6f, 0.5f, -18.0f);   // outer wall
    state.objects[18].width = 0.2f;
    state.objects[18].height = 1.0f;
    state.objects[18].depth = 6.0f;

    // Bottom Right corner
    state.objects[19].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[19].position = glm::vec3(15.0f, 0.5f, 22.5f);   // outer wall
    state.objects[19].width = 0.2f;
    state.objects[19].height = 1.0f;
    state.objects[19].depth = 5.0f;

    state.objects[20].scale = glm::vec3(14.0f, 1.0f, 0.2f);
    state.objects[20].position = glm::vec3(19.0f, 0.5f, 15.0f);   // horizontal wall
    state.objects[20].width = 14.0f;
    state.objects[20].height = 1.0f;
    state.objects[20].depth = 0.2f;

    // Top Right corner
    state.objects[21].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[21].position = glm::vec3(-15.0f, 0.5f, -22.5f);   // outer wall
    state.objects[21].width = 0.2f;
    state.objects[21].height = 1.0f;
    state.objects[21].depth = 5.0f;

    state.objects[22].scale = glm::vec3(14.0f, 1.0f, 0.2f);
    state.objects[22].position = glm::vec3(-19.0f, 0.5f, -15.0f);   // horizontal wall
    state.objects[22].width = 14.0f;
    state.objects[22].height = 1.0f;
    state.objects[22].depth = 0.2f;
    
    // Bottom Right corner
    state.objects[23].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[23].position = glm::vec3(-15.0f, 0.5f, 22.5f);   // outer wall
    state.objects[23].width = 0.2f;
    state.objects[23].height = 1.0f;
    state.objects[23].depth = 5.0f;

    state.objects[24].scale = glm::vec3(14.0f, 1.0f, 0.2f);
    state.objects[24].position = glm::vec3(-19.0f, 0.5f, 15.0f);   // horizontal wall
    state.objects[24].width = 14.0f;
    state.objects[24].height = 1.0f;
    state.objects[24].depth = 0.2f;

    // Midpoint, right wall between Top Right and Bottom Right
    state.objects[25].scale = glm::vec3(6.0f, 1.0f, 0.2f);
    state.objects[25].position = glm::vec3(22.0f, 0.5f, -4.0f);   // horizontal front wall
    state.objects[25].width = 6.0f;
    state.objects[25].height = 1.0f;
    state.objects[25].depth = 0.2f;

    state.objects[26].scale = glm::vec3(6.0f, 1.0f, 0.2f);
    state.objects[26].position = glm::vec3(18.0f, 0.5f, 4.0f);   // horizontal back wall
    state.objects[26].width = 6.0f;
    state.objects[26].height = 1.0f;
    state.objects[26].depth = 0.2f;

    // Midpoint, left wall between Top Left and Bottom Left
    state.objects[27].scale = glm::vec3(6.0f, 1.0f, 0.2f);
    state.objects[27].position = glm::vec3(-18.0f, 0.5f, -4.0f);   // horizontal front wall
    state.objects[27].width = 6.0f;
    state.objects[27].height = 1.0f;
    state.objects[27].depth = 0.2f;

    state.objects[28].scale = glm::vec3(6.0f, 1.0f, 0.2f);
    state.objects[28].position = glm::vec3(-22.0f, 0.5f, 4.0f);   // horizontal back wall
    state.objects[28].width = 6.0f;
    state.objects[28].height = 1.0f;
    state.objects[28].depth = 0.2f;

    // Midpoint, back walls between Bottom Left and Bottom Right
    state.objects[29].scale = glm::vec3(0.2f, 1.0f, 6.0f);
    state.objects[29].position = glm::vec3(-4.0f, 0.5f, 18.0f);   // vertical wall
    state.objects[29].width = 0.2f;
    state.objects[29].height = 1.0f;
    state.objects[29].depth = 6.0f;

    state.objects[30].scale = glm::vec3(0.2f, 1.0f, 6.0f);
    state.objects[30].position = glm::vec3(4.0f, 0.5f, 22.0f);   // vertical wall
    state.objects[30].width = 0.2f;
    state.objects[30].height = 1.0f;
    state.objects[30].depth = 6.0f;


    //// ----- HEALTH ----- 
    //GLuint healthTextureID = Util::LoadTexture("potion_blue.png");
    //Mesh* healthMesh = new Mesh();   // 12.6 -- need a new one b/c floor one is duplicated
    //healthMesh->LoadOBJ("potion_HighPoly.obj", 1);  // 12.6 -- 1 = no duplicate

    //state.objects[6].textureID = healthTextureID;
    //state.objects[6].mesh = healthMesh;
    //state.objects[6].scale = glm::vec3(0.03f, 0.03f, 0.03f);
    //state.objects[6].position = glm::vec3(0.0f, 0.5f, -5.0f);   // 12.6 -- crate is 1 unit tall, stop from sinking into floor
    //state.objects[6].entityType = HEALTH;
    //state.objects[6].isActive = false;

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

    // if win, switch to Win screen
    if (state.player->reachedExit == true) {
        state.nextScene = 3;
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