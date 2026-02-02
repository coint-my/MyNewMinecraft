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
	GLuint ssbo;

	std::vector<InstanceData> cubes;

	glm::vec3 posCollider;
	glm::vec3 halfCollider;

	~MySector()
	{
		cubes.clear();
	}

private:
	void myInitializeSSBO()
	{
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, cubes.size() * sizeof(InstanceData),
			cubes.data(), GL_STATIC_DRAW);
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

					InstanceData cub;
					cub.isVisible = false;
					cub.model = glm::translate(glm::mat4(1.0f), pos);
					cub.texIndex = 0;
					cub.index = len;

					if (y < countCube / 2)
					{
						cub.isVisible = true;
						cub.texIndex = 1;
					}
					cubes.push_back(cub);
				}
			}
		}

		cubLength = len;
	}

public:

	void myCreateSector(const glm::vec3& _pos = glm::vec3(0))
	{
		myInitialize(_pos);
		myInitializeSSBO();
	}

	void myUpdateSector(MyTestFirstPerson& _player, GLFWwindow* _window, 
		std::vector<std::pair<InstanceData&, GLuint>>& _posCubeRay, GLuint _indexSector)
	{
		if (MyPhysix::AABBIntersect(posCollider, halfCollider, _player.player.position,
			_player.player.halfSize))
		{
			_player.UpdateCharacter(cubes.data(), cubLength, _window, _posCubeRay, _indexSector);
		}
	}

	void myRenderSector()
	{
		// Привязываем буфер с данными
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, cubes.size());
	}
};