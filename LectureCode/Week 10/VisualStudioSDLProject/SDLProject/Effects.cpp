#include "Effects.h"

Effects::Effects(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	// Non textured Shader
	program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	currentEffect = NONE;
	alpha = 0;
}

void Effects::DrawOverlay() {
	glUseProgram(program.programID);
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
}

void Effects::Start(EffectType effectType) {
	currentEffect = effectType;

	switch (currentEffect) {
	case NONE:
		break;

	case FADEIN:
		alpha = 1.0f;	// 10.4 -- start with black screen
		break;
	}
}

void Effects::Update(float deltaTime) {
	switch (currentEffect) {
	case NONE:
		break;

	case FADEIN:
		alpha -= deltaTime;		// decrease alpha

		// prevents from keep fading out and drawing square when you don't need to
		if (alpha <= 0.0f) {	
			currentEffect = NONE;
		}
		break;
	}
}

void Effects::Render() {
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	switch (currentEffect) {
	case NONE:
		return;
		break;

	case FADEIN:
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 1.0f));	// incr size of black square to cover whole screen
		program.SetModelMatrix(modelMatrix);
		program.SetColor(0.0f, 0.0f, 0.0f, alpha);		// set square color to black
		DrawOverlay();
		break;
	}
}