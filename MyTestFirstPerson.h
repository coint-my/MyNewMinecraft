#pragma once
#include "MyCameraFly.h"
#include "gl/glfw3.h"
#include "MyTestInstace.h"

class MyCameraFPS : public MyCameraFly
{
private:

public:
	MyCameraFPS() : MyCameraFly() { }
	MyCameraFPS(const glm::vec3& _pos) : MyCameraFly(_pos.x, _pos.y, _pos.z) { }
};

class MyTestFirstPerson
{
private:

public:
	bool isActivate = false;
	MyCameraFPS camFps;
	MyPhysix::CharacterController player;

	MyTestFirstPerson(const glm::vec3& _pos) : camFps(_pos)
	{
		player.position = _pos;
		player.velocity = glm::vec3(0);
		player.halfSize = glm::vec3(0.3f, 0.9f, 0.3f); // как капсула

		player.moveSpeed = 6.0f;
		player.jumpForce = 6.5f;
		player.gravity = 18.0f;
		player.grounded = false;
	}

	void myEventMouseMove(double xpos, double ypos, GLboolean constrainPitch = true)
	{
		camFps.myEventMouseMove(NULL, xpos, ypos);
	}

	glm::vec3 GetMovementInput(GLFWwindow* window, glm::vec3 forward, glm::vec3 right)
	{
		glm::vec3 dir(0);

		if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) isActivate = !isActivate;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir += forward;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir -= forward;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir += right;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir -= right;

		dir.y = 0.0f;

		if (glm::length(dir) > 0.001f)
			dir = glm::normalize(dir);

		return dir;
	}

	void MoveCharacter(MyPhysix::CharacterController& c, const glm::vec3& moveDir, float dt)
	{
		c.velocity.x = moveDir.x * c.moveSpeed;
		c.velocity.z = moveDir.z * c.moveSpeed;
	}

	void Jump(MyPhysix::CharacterController& c)
	{
		if (c.grounded)
		{
			c.velocity.y = c.jumpForce;
			c.grounded = false;
		}
	}

	void ApplyGravity(MyPhysix::CharacterController& c, float dt)
	{
		if (!c.grounded)
			c.velocity.y -= c.gravity * dt;
	}

	bool AABBIntersect(glm::vec3 posA, glm::vec3 halfA, glm::vec3 posB, glm::vec3 halfB)
	{
		return
			abs(posA.x - posB.x) <= (halfA.x + halfB.x) &&
			abs(posA.y - posB.y) <= (halfA.y + halfB.y) &&
			abs(posA.z - posB.z) <= (halfA.z + halfB.z);
	}

	void ResolveCharacterCollision(MyPhysix::CharacterController& c, const InstanceData& box)
	{
		glm::vec3 delta = c.position - glm::vec3(box.pos);

		float px = (c.halfSize.x + 0.5f/*box.collider.halfSize.x*/) - abs(delta.x);
		float py = (c.halfSize.y + 0.5f/*box.collider.halfSize.y*/) - abs(delta.y);
		float pz = (c.halfSize.z + 0.5f/*box.collider.halfSize.z*/) - abs(delta.z);

		if (px < py && px < pz)
		{
			c.position.x += (delta.x > 0 ? px : -px);
			c.velocity.x = 0;
		}
		else if (py < pz)
		{
			c.position.y += (delta.y > 0 ? py : -py);
			c.velocity.y = 0;

			if (delta.y > 0)
				c.grounded = true;
		}
		else
		{
			c.position.z += (delta.z > 0 ? pz : -pz);
			c.velocity.z = 0;
		}
	}

	void MyCharacterHandle(GLFWwindow* _window)
	{
		glm::vec3 moveDir = GetMovementInput(_window, camFps.myGetFront(), camFps.myGetRight());

		if (isActivate)
		{
			player.grounded = false;
			MoveCharacter(player, moveDir, camFps.myGetDeltaTime());
			ApplyGravity(player, camFps.myGetDeltaTime());
			player.position += player.velocity * camFps.myGetDeltaTime();
		}
	}

	void UpdateCharacter(InstanceData* world, const GLuint _count, GLFWwindow* window,
		std::vector<std::pair<InstanceData&, GLuint>>& _pairCubeRay, GLuint _indexSector)
	{
		for (int i = 0; i < _count; i++)
		{
			//raycast with world
			if (AABBIntersect(player.position, glm::vec3(3),
				world[i].pos, glm::vec3(0.5f)))
			{
				std::pair<InstanceData&, GLuint> pair = { world[i], _indexSector };
				_pairCubeRay.push_back(pair);

				if (world[i].isVisible)
				{
					if (AABBIntersect(player.position, player.halfSize,
						world[i].pos, glm::vec3(0.5f)))
					{
						ResolveCharacterCollision(player, world[i]);
					}
				}
			}
		}
	}

	void myUpdateCamera()
	{
		/*if (camFps.keys[GLFW_KEY_W] && body.isGrounded)
			body.ApplyImpulse(&body, camFps.myGetFront() * (float)speed * camFps.myGetDeltaTime());
		if (camFps.keys[GLFW_KEY_S] && body.isGrounded)
			body.ApplyImpulse(&body, -camFps.myGetFront() * (float)speed * camFps.myGetDeltaTime());
		if (camFps.keys[GLFW_KEY_A] && body.isGrounded)
			body.ApplyImpulse(&body, -camFps.myGetRight() * (float)speed * camFps.myGetDeltaTime());
		if (camFps.keys[GLFW_KEY_D] && body.isGrounded)
			body.ApplyImpulse(&body, camFps.myGetRight() * (float)speed * camFps.myGetDeltaTime());*/

		camFps.myUpdateCamera();

		camFps.mySetPos(player.position);
	}

	void myEventKey(int _key, int _scancode, int _action, int _mode)
	{
		if (_key == GLFW_KEY_SPACE && _action == GLFW_PRESS)
			Jump(player);
	}

	void myEventMouseButton(GLFWwindow* _window, int _button, int _action, int _mods)
	{
		camFps.myEventMouseButton(_window, _button, _action, _mods);
	}
};