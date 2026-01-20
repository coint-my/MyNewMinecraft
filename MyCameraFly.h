#pragma once

#include "MyCamera.h"
#include "gl/glfw3.h"

class MyCameraFly : public MyCamera
{
private:
	GLfloat lastX = 400, lastY = 300;
	bool firstMouse = true;
	int _frame;
	GLdouble _lastTime;
	GLfloat lastFrame = 0.0f;
protected:
	GLfloat deltaTime = 0.0f;

	enum { SYNCRONIZE_OFF, SYNCRONIZE_ON_60, SYNCRONIZE_ON_30 };
public:
	bool keys[1024] = { false };

	MyCameraFly() : MyCamera()
	{
		Position = glm::vec3(0.0f, 0.0f, 0.0f);

		_frame = 0;
		_lastTime = glfwGetTime();

		// включить или выключить синхронизацию
		glfwSwapInterval(SYNCRONIZE_ON_60);
	}

	MyCameraFly(float _x, float _y, float _z)
	{
		Position = glm::vec3(_x, _y, _z);
	}

	inline GLfloat myGetDeltaTime() const { return deltaTime; }

	inline GLfloat myGetFPS() const { return _frame; }

	void myEventKey(GLFWwindow* _window, int _key, int _scancode, int _action, int _mode)
	{
		if (_key >= 0 && _key < 1024)
		{
			if (_action == GLFW_PRESS)
				keys[_key] = true;
			else if (_action == GLFW_RELEASE)
				keys[_key] = false;
		}
	}

	void myEventMouseButton(GLFWwindow* _window, int _button, int _action, int _mods)
	{
		if (_button == GLFW_MOUSE_BUTTON_LEFT && _action == GLFW_PRESS)
		{
			// ЛКМ нажата
			// Спрятать курсор на форме и заморозить его
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		if (_button == GLFW_MOUSE_BUTTON_RIGHT && _action == GLFW_RELEASE)
		{
			// ПКМ отпущена
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void myEventMouseScroll(GLFWwindow* _window, double _xoffset, double _yoffset)
	{
		if (this->Zoom >= 44.0f && this->Zoom <= 45.0f)
			this->Zoom -= _yoffset * 0.1f;
		if (this->Zoom <= 44.0f)
			this->Zoom = 44.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

	void myEventMouseMove(GLFWwindow* window, double xpos, double ypos, GLboolean constrainPitch = true)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

		lastX = xpos;
		lastY = ypos;

		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->myUpdateCameraVectors();
	}

	inline virtual void myUpdateCamera()
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		GLdouble currentTime = glfwGetTime();
		_frame++;
		if (currentTime - _lastTime >= 0)
		{
			std::cout << "frame second = " << _frame << std::endl;
			_frame = 0;
			_lastTime += 1.0;
		}
	}

	virtual void myUpdateEventKeyboard()
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
		if (keys[GLFW_KEY_LEFT_SHIFT]) velocity = velocity * 10;
		// Camera controls
		if (keys[GLFW_KEY_W])
			this->Position += this->Front * velocity;
		if (keys[GLFW_KEY_S])
			this->Position -= this->Front * velocity;
		if (keys[GLFW_KEY_A])
			this->Position -= this->Right * velocity;
		if (keys[GLFW_KEY_D])
			this->Position += this->Right * velocity;
		if (keys[GLFW_KEY_SPACE])
			this->Position += this->Up * velocity;
		if (keys[GLFW_KEY_LEFT_CONTROL])
			this->Position -= this->Up * velocity;
	}

	inline virtual void myUpdateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};