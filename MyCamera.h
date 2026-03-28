#pragma once

#include "gl/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MyScreen.h"

class MyCamera
{
protected:
	// Default camera values
	const GLfloat YAW = 90.0f;
	const GLfloat PITCH = 0.0f;
	const GLfloat SPEED = 3.0f;
	const GLfloat SENSITIVTY = 0.10f;
	const GLfloat ZOOM = 45.0f;

	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;
public:
	MyCamera()
	{
		Yaw = YAW;
		Pitch = PITCH;
		MovementSpeed = SPEED;
		MouseSensitivity = SENSITIVTY;
		Zoom = ZOOM;

		Front = glm::vec3(.0f, .0f, -1.0f);
		Up = glm::vec3(.0f, 1.0f, .0f);
		Right = glm::vec3(1.0f, .0f, .0f);
		WorldUp = Up;
	}
    inline glm::vec3 myGetPos() const { return Position; }

	inline void mySetPos(const glm::vec3& _pos) { Position = _pos; }

	inline glm::vec3 myGetFront() const { return Front; }

	inline glm::vec3 myGetRight() const { return Right; }

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	inline glm::mat4 myGetViewMatrix() const
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	inline glm::mat4 myGetPerspective() const
	{
		return glm::perspective(Zoom, (float)MyScrren::WID / (float)MyScrren::HEI, 0.1f, 300.0f);
	}

	virtual void myUpdateCameraVectors() = 0;
};