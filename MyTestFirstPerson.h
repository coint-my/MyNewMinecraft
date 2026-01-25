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

	void ResolveCharacterCollision(MyPhysix::CharacterController& c, const MyPhysix::PhysicsBody& box)
	{
		glm::vec3 delta = c.position - box.position;

		float px = (c.halfSize.x + box.collider.halfSize.x) - abs(delta.x);
		float py = (c.halfSize.y + box.collider.halfSize.y) - abs(delta.y);
		float pz = (c.halfSize.z + box.collider.halfSize.z) - abs(delta.z);

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

	/*void UpdateCharacter(MyPhysix::CharacterController& c, 
		const std::vector<MyPhysix::PhysicsBody*>& world,
		GLFWwindow* window, glm::vec3 forward, glm::vec3 right, float dt)
	{
		c.grounded = false;

		glm::vec3 moveDir = GetMovementInput(window, forward, right);
		MoveCharacter(c, moveDir, dt);

		ApplyGravity(c, dt);

		c.position += c.velocity * dt;

		for (const MyPhysix::PhysicsBody* box : world)
		{
			if (glm::distance(c.position, box->position) < 3.0f)
			{
				if (!box->isStatic) continue;

				if (AABBIntersect(c.position, c.halfSize,
					box->position, box->collider.halfSize))
				{
					ResolveCharacterCollision(c, *box);
				}
			}
		}
	}*/

	void MyCharacterHandle(GLFWwindow* _window)
	{
		player.grounded = false;
		glm::vec3 moveDir = GetMovementInput(_window, camFps.myGetFront(), camFps.myGetRight());
		MoveCharacter(player, moveDir, camFps.myGetDeltaTime());
		ApplyGravity(player, camFps.myGetDeltaTime());
		player.position += player.velocity * camFps.myGetDeltaTime();
	}

	void UpdateCharacter(MyPhysix::MyCube* world, const GLuint _count, GLFWwindow* window,
		std::vector<glm::mat4>& _posCubeRay)
	{
		for (int i = 0; i < _count; i++)
		{
			//raycast with world
			if (world[i].isVisible && AABBIntersect(player.position, glm::vec3(3),
				world[i].boxPhysix.position, world[i].boxPhysix.collider.halfSize))
			{
				_posCubeRay.push_back(world[i].model);
			}
			//player collision with world
			if (world[i].isVisible && glm::distance(player.position, world[i].boxPhysix.position) < 2.0f)
			{
				if (!world[i].boxPhysix.isStatic) continue;

				if (AABBIntersect(player.position, player.halfSize,
					world[i].boxPhysix.position, world[i].boxPhysix.collider.halfSize))
				{
					ResolveCharacterCollision(player, world[i].boxPhysix);
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