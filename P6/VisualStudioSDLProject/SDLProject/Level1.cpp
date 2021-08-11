#include "Level1.h"

#define OBJECT_COUNT 62
#define ENEMY_COUNT 3


void Level1::Initialize() {
    state.nextScene = -1;

    /*
    -----------------   Initialize Player  -----------------
    */
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);  // return to this 

    //state.player->position = glm::vec3(-5.0, 0.75f, 0.0);    // for testing

    state.player->width = 0.5;
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 3.0f;

    /*
    -----------------   Initialize Objects  -----------------
    */
    state.objects = new Entity[OBJECT_COUNT];
    
    // ----- FLOOR -----
    GLuint floorTextureID = Util::LoadTexture("ch01_island_ground_soil_dif.jpg");
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
    GLuint ceilingTextureID = Util::LoadTexture("ch01_island_ground_stone_dif.png");
    Mesh* ceilingMesh = new Mesh();
    ceilingMesh->LoadOBJ("cube.obj", 10);

    state.objects[1].textureID = ceilingTextureID;
    state.objects[1].mesh = ceilingMesh;
    state.objects[1].position = glm::vec3(0.0f, 8.0f, 0.0f);
    state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[1].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    state.objects[1].scale = glm::vec3(50.0f, 0.5f, 50.0f);
    state.objects[1].entityType = CEILING;


    // ----- Outer Walls ----- 
    // i = [2] to [5]
    GLuint wallTextureID = Util::LoadTexture("ch01_island_ground_stonepath_dif.png");
    Mesh* wallMesh = new Mesh();
    wallMesh->LoadOBJ("cube.obj", 2);  // 12.3 -- duplicate texture 10 times // 12.6 -- dup 20 times

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
    state.objects[7].depth = 0.5f;

    // ----- Maze Walls ----- 
    GLuint mazeTextureID = Util::LoadTexture("Grass_Texture.png");
    //GLuint mazeTextureID = Util::LoadTexture("Grass_Texture.png");
    Mesh* mazeMesh = new Mesh();
    mazeMesh->LoadOBJ("cube.obj", 1);

    for (size_t i = 8; i < OBJECT_COUNT; i++) {
        state.objects[i].textureID = mazeTextureID;
        state.objects[i].mesh = mazeMesh;

        state.objects[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        state.objects[i].entityType = WALL;
    }

    state.objects[8].scale = glm::vec3(5.0f, 1.0f, 0.2f);
    state.objects[8].position = glm::vec3(0.0f, 0.5f, 2.5f);   // behind player
    state.objects[8].width = 5.0f;
    state.objects[8].height = 1.0f;
    state.objects[8].depth = 0.2f;

    state.objects[9].scale = glm::vec3(0.2f, 1.0f, 10.0f);
    state.objects[9].position = glm::vec3(2.5f, 0.5f, -2.5f);    // player right
    state.objects[9].width = 0.2f;
    state.objects[9].height = 1.0f;
    state.objects[9].depth = 10.0f;

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

    state.objects[18].scale = glm::vec3(0.2f, 1.0f, 4.0f);
    state.objects[18].position = glm::vec3(6.6f, 0.5f, -19.05f);   // outer wall, was too long
    state.objects[18].width = 0.2f;
    state.objects[18].height = 1.0f;
    state.objects[18].depth = 4.0f;

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

    // Top Left corner
    state.objects[21].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[21].position = glm::vec3(-15.0f, 0.5f, -22.5f);   // outer wall
    state.objects[21].width = 0.2f;
    state.objects[21].height = 1.0f;
    state.objects[21].depth = 5.0f;

    state.objects[22].scale = glm::vec3(16.0f, 1.0f, 0.2f);
    state.objects[22].position = glm::vec3(-17.0f, 0.5f, -15.0f);   // horizontal wall
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
    state.objects[27].scale = glm::vec3(7.0f, 1.0f, 0.2f);
    state.objects[27].position = glm::vec3(-14.0f, 0.5f, -10.0f);   // horizontal front wall
    state.objects[27].width = 7.0f;
    state.objects[27].height = 1.0f;
    state.objects[27].depth = 0.2f;

    state.objects[28].scale = glm::vec3(4.0f, 1.0f, 0.2f);
    state.objects[28].position = glm::vec3(-23.0f, 0.5f, 4.0f);   // horizontal back wall, dead end wall
    state.objects[28].width = 4.0f;
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

    // connect Upper Right to Right midpoint
    state.objects[31].scale = glm::vec3(12.0f, 1.0f, 0.2f);
    state.objects[31].position = glm::vec3(15.0f, 0.5f, -10.2f);   // horizontal wall
    state.objects[31].width = 12.0f;
    state.objects[31].height = 1.0f;
    state.objects[31].depth = 0.2f;

    state.objects[32].scale = glm::vec3(0.2f, 1.0f, 10.0f);
    state.objects[32].position = glm::vec3(15.0f, 0.5f, -1.0f);   // inner vertical wall
    state.objects[32].width = 0.2f;
    state.objects[32].height = 1.0f;
    state.objects[32].depth = 10.0f;

    state.objects[33].scale = glm::vec3(0.2f, 1.0f, 9.0f);
    state.objects[33].position = glm::vec3(9.0f, 0.5f, -5.5f);   // outer vertical wall
    state.objects[33].width = 0.2f;
    state.objects[33].height = 1.0f;
    state.objects[33].depth = 9.0f;

    state.objects[34].scale = glm::vec3(6.0f, 1.0f, 0.2f);
    state.objects[34].position = glm::vec3(12.0f, 0.5f, -1.0f);   // horizontal wall, dead end, put health near here
    state.objects[34].width = 6.0f;
    state.objects[34].height = 1.0f;
    state.objects[34].depth = 0.2f;

    state.objects[35].scale = glm::vec3(6.0f, 1.0f, 0.2f);
    state.objects[35].position = glm::vec3(12.0f, 0.5f, 20.0f);   // horizontal wall, connecting lower right
    state.objects[35].width = 6.0f;
    state.objects[35].height = 1.0f;
    state.objects[35].depth = 0.2f;

    state.objects[36].scale = glm::vec3(0.2f, 1.0f, 7.0f);
    state.objects[36].position = glm::vec3(9.0f, 0.5f, 16.5f);   // outer vertical wall, path to lower right and exit
    state.objects[36].width = 0.2f;
    state.objects[36].height = 1.0f;
    state.objects[36].depth = 7.0f;

    state.objects[37].scale = glm::vec3(0.2f, 1.0f, 6.0f);
    state.objects[37].position = glm::vec3(21.0f, 0.5f, 7.0f);   // connect vertical wall
    state.objects[37].width = 0.2f;
    state.objects[37].height = 1.0f;
    state.objects[37].depth = 6.0f;

    state.objects[38].scale = glm::vec3(12.0f, 1.0f, 0.2f);
    state.objects[38].position = glm::vec3(15.0f, 0.5f, 10.0f);   // horizontal wall, to crossroad
    state.objects[38].width = 12.0f;
    state.objects[38].height = 1.0f;
    state.objects[38].depth = 0.2f;

    // replicate on left side
    state.objects[39].scale = glm::vec3(6.0f, 1.0f, 0.2f);
    state.objects[39].position = glm::vec3(-12.0f, 0.5f, 20.0f);   // horizontal wall, connecting lower left
    state.objects[39].width = 6.0f;
    state.objects[39].height = 1.0f;
    state.objects[39].depth = 0.2f;

    state.objects[40].scale = glm::vec3(0.2f, 1.0f, 7.0f);
    state.objects[40].position = glm::vec3(-9.0f, 0.5f, 16.5f);   // outer vertical wall, path to lower left 
    state.objects[40].width = 0.2f;
    state.objects[40].height = 1.0f;
    state.objects[40].depth = 7.0f;

    state.objects[41].scale = glm::vec3(0.2f, 1.0f, 6.0f);
    state.objects[41].position = glm::vec3(-21.0f, 0.5f, 7.0f);   // connect vertical wall
    state.objects[41].width = 0.2f;
    state.objects[41].height = 1.0f;
    state.objects[41].depth = 6.0f;

    state.objects[42].scale = glm::vec3(12.0f, 1.0f, 0.2f);
    state.objects[42].position = glm::vec3(-15.0f, 0.5f, 10.0f);   // horizontal wall, to crossroad
    state.objects[42].width = 12.0f;
    state.objects[42].height = 1.0f;
    state.objects[42].depth = 0.2f;

    // player path
    state.objects[43].scale = glm::vec3(10.0f, 1.0f, 0.2f);
    state.objects[43].position = glm::vec3(-2.5f, 0.5f, -7.5f);   // 1st wall in front of player
    state.objects[43].width = 10.0f;
    state.objects[43].height = 1.0f;
    state.objects[43].depth = 0.2f;

    state.objects[44].scale = glm::vec3(8.0f, 1.0f, 0.2f);
    state.objects[44].position = glm::vec3(-6.5f, 0.5f, -2.5f);   // 1st wall connected to start left wall
    state.objects[44].width = 8.0f;
    state.objects[44].height = 1.0f;
    state.objects[44].depth = 0.2f;

    state.objects[45].scale = glm::vec3(0.2f, 1.0f, 8.0f);
    state.objects[45].position = glm::vec3(-10.5f, 0.5f, -6.5f);   // vertical wall, connect to 44
    state.objects[45].width = 0.2f;
    state.objects[45].height = 1.0f;
    state.objects[45].depth = 8.0f;

    // 'gates'
    state.objects[46].scale = glm::vec3(8.0f, 1.0f, 0.2f);
    state.objects[46].position = glm::vec3(-6.5f, 0.5f, -10.5f);   // horizontal wall between door wall and player wall
    state.objects[46].width = 8.0f;
    state.objects[46].height = 1.0f;
    state.objects[46].depth = 0.2f;

    state.objects[47].scale = glm::vec3(9.0f, 1.0f, 0.2f);
    state.objects[47].position = glm::vec3(4.5f, 0.5f, -10.5f);   // horizontal wall on same horizonal as 46, has gap in between
    state.objects[47].width = 9.0f;
    state.objects[47].height = 1.0f;
    state.objects[47].depth = 0.2f;

    state.objects[48].scale = glm::vec3(0.2f, 1.0f, 9.0f);
    state.objects[48].position = glm::vec3(-5.5f, 0.5f, -15.0f);   // vertical wall, connect to 44
    state.objects[48].width = 0.2f;
    state.objects[48].height = 1.0f;
    state.objects[48].depth = 9.0f;

    // connect gates to Top Left corner, gap in between for middle path
    state.objects[49].scale = glm::vec3(0.2f, 1.0f, 6.0f);
    state.objects[49].position = glm::vec3(-9.0f, 0.5f, -18.0f);   // vertical wall, connect to 44, 
    state.objects[49].width = 0.2f;
    state.objects[49].height = 1.0f;
    state.objects[49].depth = 6.0f;

    state.objects[50].scale = glm::vec3(12.0f, 1.0f, 0.2f);
    state.objects[50].position = glm::vec3(-8.5f, 0.5f, 2.5f);   // horizontal wall, dead end
    state.objects[50].width = 12.0f;
    state.objects[50].height = 1.0f;
    state.objects[50].depth = 0.2f;

    state.objects[51].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[51].position = glm::vec3(-14.5f, 0.5f, 0.0f);   // vertical wall
    state.objects[51].width = 0.2f;
    state.objects[51].height = 1.0f;
    state.objects[51].depth = 5.0f;

    state.objects[52].scale = glm::vec3(10.0f, 1.0f, 0.2f);
    state.objects[52].position = glm::vec3(-1.0f, 0.5f, 15.0f);   // horizontal wall, dead end, connect 2 verticals, map back
    state.objects[52].width = 10.0f;
    state.objects[52].height = 1.0f;
    state.objects[52].depth = 0.2f;

    state.objects[53].scale = glm::vec3(8.0f, 1.0f, 0.2f);
    state.objects[53].position = glm::vec3(5.0f, 0.5f, 10.0f);   // horizontal wall, dead end, connect 2 verticals, map back
    state.objects[53].width = 8.0f;
    state.objects[53].height = 1.0f;
    state.objects[53].depth = 0.2f;

    state.objects[54].scale = glm::vec3(0.2f, 1.0f, 4.0f);
    state.objects[54].position = glm::vec3(2.5f, 0.5f, 4.0f);   // vertical wall, connect to 44
    state.objects[54].width = 0.2f;
    state.objects[54].height = 1.0f;
    state.objects[54].depth = 4.0f;

    state.objects[55].scale = glm::vec3(8.0f, 1.0f, 0.2f);
    state.objects[55].position = glm::vec3(6.5f, 0.5f, 4.0f);   // horizontal wall, connect to entrence trick
    state.objects[55].width = 8.0f;
    state.objects[55].height = 1.0f;
    state.objects[55].depth = 0.2f;

    state.objects[56].scale = glm::vec3(0.2f, 1.0f, 4.0f);
    state.objects[56].position = glm::vec3(10.5f, 0.5f, 4.0f);   // vertical wall, connect entrence trick and 55
    state.objects[56].width = 0.2f;
    state.objects[56].height = 1.0f;
    state.objects[56].depth = 4.0f;

    state.objects[57].scale = glm::vec3(0.2f, 1.0f, 3.0f);
    state.objects[57].position = glm::vec3(7.5f, 0.5f, 8.5f);   // vertical wall, connect entrence trick and 55
    state.objects[57].width = 0.2f;
    state.objects[57].height = 1.0f;
    state.objects[57].depth = 3.0f;

    state.objects[58].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[58].position = glm::vec3(1.0f, 0.5f, 12.5f);   // vertical wall, connect entrence trick and 55
    state.objects[58].width = 0.2f;
    state.objects[58].height = 1.0f;
    state.objects[58].depth = 5.0f;

    state.objects[59].scale = glm::vec3(0.2f, 1.0f, 10.0f);
    state.objects[59].position = glm::vec3(-3.0f, 0.5f, 10.0f);   // vertical wall, connect entrence trick and 55
    state.objects[59].width = 0.2f;
    state.objects[59].height = 1.0f;
    state.objects[59].depth = 10.0f;

    state.objects[60].scale = glm::vec3(0.2f, 1.0f, 5.0f);
    state.objects[60].position = glm::vec3(-8.0f, 0.5f, 5.0f);   // vertical wall, connect entrence trick and 55
    state.objects[60].width = 0.2f;
    state.objects[60].height = 1.0f;
    state.objects[60].depth = 5.0f;

    state.objects[61].scale = glm::vec3(8.0f, 1.0f, 0.2f);
    state.objects[61].position = glm::vec3(-21.0f, 0.5f, -5.0f);   // horizontal front wall
    state.objects[61].width = 6.0f;
    state.objects[61].height = 1.0f;
    state.objects[61].depth = 0.2f;



    //// ----- HEALTH ----- 

    //GLuint healthTextureID = Util::LoadTexture("health.png");
    //Mesh* healthMesh = new Mesh();
    //healthMesh->LoadOBJ("cube.obj", 1);  // 12.3 -- duplicate texture 50 times

    //state.objects[62].textureID = healthTextureID;
    //state.objects[62].mesh = healthMesh;
    //state.objects[62].scale = glm::vec3(0.5f, 0.5f, 0.5f);
    //state.objects[62].position = glm::vec3(-22.0f, 0.5f, -22.0f);
    //state.objects[62].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    //state.objects[62].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    //state.objects[62].width = 0.5f;
    //state.objects[62].height = 0.5f;
    //state.objects[62].depth = 0.5f;
    //state.objects[62].entityType = HEALTH;


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
    state.player->Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
    
    // update objects
    for (size_t i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
    }

    // update enemies
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