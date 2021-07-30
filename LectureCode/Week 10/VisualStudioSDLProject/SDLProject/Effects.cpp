#include "Effects.h"

Effects::Effects(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	// Non textured Shader
	program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	currentEffect = NONE;
	alpha = 0;
	speed = 1.0f;

	viewOffset = glm::vec3(0.0f, 0.0f, 0.0f);	// 10.7 -- initialize camera as no offset
}

void Effects::DrawOverlay() {
	glUseProgram(program.programID);
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
}

void Effects::Start(EffectType effectType, float effectSpeed) {
	currentEffect = effectType;
	speed = effectSpeed;

	switch (currentEffect) {
	case NONE:
		break;

	case FADEIN:
		alpha = 1.0f;	// 10.4 -- start with black screen
		break;

	case FADEOUT:
		alpha = 0.0f;	// 10.5 -- start with game screen
		break;

	case GROW:
		size = 0.0f;
		break;

	case SHRINK:
		size = 10.0f;
		break;

	case SHAKE:
		timeLeft = 1.0f;	// 1 sec
		break;






	}
}

void Effects::Update(float deltaTime) {
	switch (currentEffect) {
	case NONE:
		break;

	case FADEIN:
		alpha -= deltaTime * speed;		// decrease alpha
		if (alpha <= 0.0f) currentEffect = NONE;	// prevents from fading out and drawing square when unnecessary
		break;

	case FADEOUT:
		alpha += deltaTime * speed;		// increase alpha
		break;

	case GROW:
		size += deltaTime * speed;		// increase size
		break;

	case SHRINK:
		size -= deltaTime * speed;		// decrease size
		if (size <= 0.0f) currentEffect = NONE;
		break;

	case SHAKE:
		timeLeft -= deltaTime * speed;	// 1 sec

		// if time on shake ends, reset view and stop effect
		// else,
		if (timeLeft <= 0.0f) {
			viewOffset = glm::vec3(0.0f, 0.0f, 0.0f);
			currentEffect = NONE;
		}
		else {
			// how much the shake can move
			float max = 0.1f;
			float min = -0.1f;

			float r = ((float)rand() / RAND_MAX) * (max - min) + min;	// choose a random num b/t max and min
			viewOffset = glm::vec3(r, r, 0);
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

	case FADEOUT:	// uses the same code as FADEIN
	case FADEIN:
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 1.0f));	// incr size of black square to cover whole screen
		program.SetModelMatrix(modelMatrix);
		program.SetColor(0.0f, 0.0f, 0.0f, alpha);		// set square color to black
		DrawOverlay();
		break;

	case SHRINK:
	case GROW:
		modelMatrix = glm::scale(modelMatrix, glm::vec3(size, size * 0.75f, 1.0f));	// * 0.75 b/c screen isn't square
		program.SetModelMatrix(modelMatrix);
		program.SetColor(0.0f, 0.0f, 0.0f, 1.0f);		// set square color to black
		DrawOverlay();
		break;

	case SHAKE:
		break;	// nothing is drawn
	}
}