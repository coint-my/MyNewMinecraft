#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MyPhysix.h"
#include "MyTestFirstPerson.h"

class MySector
{
public:
	static const int countCube = 16;
	unsigned int cubLength;

	std::vector<MyPhysix::MyCube> cubes;

	glm::vec3 posCollider;
	glm::vec3 halfCollider;

	~MySector()
	{

	}

	void myInitialize(const glm::vec3& _posSector = glm::vec3())
	{
		cubes.reserve(countCube * countCube * countCube);
		
		posCollider = glm::vec3(_posSector);
		halfCollider = glm::vec3(countCube / 2) + 3.0f;

		int len = -1;		

		for (int y = 0; y < countCube; y++)
		{
			for (int x = 0; x < countCube; x++)
			{
				for (int z = 0; z < countCube; z++)
				{
					len++;

					glm::vec3 pos = glm::vec3((float)x, (float)y, (float)z);
					pos = pos + posCollider - glm::vec3(countCube / 2);

					MyPhysix::MyCube cub;
					cub.isVisible = false;
					cub.model = glm::translate(glm::mat4(1.0f), pos);
					cub.texture = 0;
					cub.boxPhysix.position = pos;
					cub.boxPhysix.collider.halfSize = glm::vec3(0.5f);
					cub.boxPhysix.friction = 0.1f;
					cub.boxPhysix.mass = 0.0f;
					cub.boxPhysix.isStatic = true;

					if (y < countCube / 2)
					{
						cub.isVisible = true;
						cub.texture = 1;
					}
					cubes.push_back(cub);
				}
			}
		}

		cubLength = len;
	}

	void myAddInRender(std::vector<InstanceData>& _renderer) const
	{
		for (int cub = 0; cub < cubLength; cub++)
		{
			InstanceData data = { cubes[cub].model, cubes[cub].texture, cubes[cub].isVisible };
			
			_renderer.push_back(data);
		}
	}

	void myUpdateSector(MyTestFirstPerson& _player, GLFWwindow* _window, 
		std::vector<glm::mat4>& _posCubeRay)
	{
		if (MyPhysix::AABBIntersect(posCollider, halfCollider, _player.player.position,
			_player.player.halfSize))
		{
			_player.UpdateCharacter(cubes.data(), cubLength, _window, _posCubeRay);
		}
	}
};