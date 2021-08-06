#include "Menu.h"

#define OBJECT_COUNT 1

void Menu::Initialize() {
    state.nextScene = -1;


}

void Menu::Update(float deltaTime) {}

void Menu::Render(ShaderProgram* program) {
//void Menu::Render(ShaderProgram* program, ShaderProgram* programUI) {
    GLuint fontTextureID = Util::LoadTexture("font3.png");

    //Util::DrawText(programUI, fontTextureID, "Game Name", 0.5, 0.0f, glm::vec3(-1.5f, 1.5f, 0.0f));

    //Util::DrawText(programUI, fontTextureID, "Press Enter to Start", 0.5, 0.0f, glm::vec3(-2.0f, 0.5f, 0.0f));
}