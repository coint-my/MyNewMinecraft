#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MyPhysix.h"
#include "MyTestFirstPerson.h"

enum MyDirectionCub { LEFT, RIGHT, UP, DOWN, FRONT, BACK };

class MySector
{
public:
	static const int countCube = 16;
	unsigned int cubLength;
	unsigned int index = -1;
	GLuint ssbo;

	std::vector<MySector*> mySides;

	std::vector<InstanceData> cubes;
	std::vector<InstanceData> renderCubes;

	glm::vec3 posCollider;
	glm::vec3 halfCollider;

	~MySector()
	{
		cubes.clear();
		renderCubes.clear();
	}

private:
	
	inline glm::vec3 myGet3DIndex(const GLuint _1Dindex) const
	{
		return glm::vec3(_1Dindex % countCube, (_1Dindex / countCube) % countCube,
			_1Dindex / (countCube * countCube));
	}

	inline GLuint myGet1DIndex(int _x, int _y, int _z) const
	{
		return _x * (countCube * countCube) + _y * countCube + _z;
	}

	void myAddRenderer(const InstanceData& _data)
	{
		// Добавляем только этот куб в список для GPU
		renderCubes.push_back(_data);
	}

	int myCheckSides(const MyDirectionCub& _side, int _indexSide, int _qube)
	{
		if (_indexSide >= 0 && _indexSide <= _qube * _qube)
		{
			if (_side == LEFT)
				return _indexSide;
			else if (_side == RIGHT && _qube * _qube - 1 >= _indexSide)
				return _indexSide;
			else if (_side == FRONT && _qube - 1 != (_indexSide - 1) % _qube)
				return _indexSide;
			else if (_side == BACK && _qube - 1 != _indexSide % _qube)
				return _indexSide;
		}
		return -1;
	}

public:

	void myInitializeSSBO()
	{
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, renderCubes.size() * sizeof(InstanceData),
			renderCubes.data(), GL_STATIC_DRAW);
	}

	bool myCheckNeighborgCub(int _x, int _y, int _z, int _index)
	{
		if (_x == 0 && _z == countCube - 1 && mySides[LEFT] != nullptr && mySides[FRONT] != nullptr)
		{
			int indLeft = myGet1DIndex(countCube - 1, _y, _z);
			int indFront = myGet1DIndex(_x, _y, 0);
			InstanceData& cubLeft = mySides[LEFT]->cubes[indLeft];
			InstanceData& cubFront = mySides[FRONT]->cubes[indFront];
			if (cubLeft.isVisible == true && cubFront.isVisible == true &&
				cubes[myGet1DIndex(_x + 1, _y, _z)].isVisible == true && _z >= 0 && _z <= countCube - 1 &&
				cubes[myGet1DIndex(_x, _y, _z - 1)].isVisible == true && _y > 0 && _y < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true)
				return false;
		}
		if (_x == countCube - 1 && _z == countCube - 1 && 
			mySides[RIGHT] != nullptr && mySides[FRONT] != nullptr)
		{
			int indRight = myGet1DIndex(0, _y, _z);
			int indFront = myGet1DIndex(_x, _y, 0);
			InstanceData& cubRight = mySides[RIGHT]->cubes[indRight];
			InstanceData& cubFront = mySides[FRONT]->cubes[indFront];
			if (cubRight.isVisible == true && cubFront.isVisible == true &&
				cubes[myGet1DIndex(_x - 1, _y, _z)].isVisible == true && _z >= 0 && _z <= countCube - 1 &&
				cubes[myGet1DIndex(_x, _y, _z - 1)].isVisible == true && _y > 0 && _y < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true)
				return false;
		}
		if (_x == 0 && _z == 0 && mySides[LEFT] != nullptr && mySides[BACK] != nullptr)
		{
			int indLeft = myGet1DIndex(countCube - 1, _y, _z);
			int indBack = myGet1DIndex(_x, _y, countCube - 1);
			InstanceData& cubLeft = mySides[LEFT]->cubes[indLeft];
			InstanceData& cubBack = mySides[BACK]->cubes[indBack];
			if (cubLeft.isVisible == true && cubBack.isVisible == true &&
				cubes[myGet1DIndex(_x + 1, _y, _z)].isVisible == true && _z >= 0 && _z <= countCube - 1 &&
				cubes[myGet1DIndex(_x, _y, _z + 1)].isVisible == true && _y > 0 && _y < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true)
				return false;
		}
		if (_x == countCube - 1 && _z == 0 && mySides[RIGHT] != nullptr && mySides[BACK] != nullptr)
		{
			int indRight = myGet1DIndex(0, _y, _z);
			int indBack = myGet1DIndex(_x, _y, countCube - 1);
			InstanceData& cubLeft = mySides[RIGHT]->cubes[indRight];
			InstanceData& cubBack = mySides[BACK]->cubes[indBack];
			if (cubLeft.isVisible == true && cubBack.isVisible == true &&
				cubes[myGet1DIndex(_x - 1, _y, _z)].isVisible == true && _z >= 0 && _z <= countCube - 1 &&
				cubes[myGet1DIndex(_x, _y, _z + 1)].isVisible == true && _y > 0 && _y < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true)
				return false;
		}
		if (_x == 0 && mySides[LEFT] != nullptr)
		{
			int ind = myGet1DIndex(countCube - 1, _y, _z);
			InstanceData& cub = mySides[LEFT]->cubes[ind];
			if (cub.isVisible == true &&
				cubes[myGet1DIndex(_x + 1, _y, _z)].isVisible == true && _z > 0 && _z < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y, _z - 1)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y, _z + 1)].isVisible == true && _y > 0 && _y < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true)
				return false;
		}
		if (_x == countCube - 1 && mySides[RIGHT] != nullptr)
		{
			int ind = myGet1DIndex(0, _y, _z);
			InstanceData& cub = mySides[RIGHT]->cubes[ind];
			if (cub.isVisible == true &&
				cubes[myGet1DIndex(_x - 1, _y, _z)].isVisible == true && _z > 0 && _z < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y, _z - 1)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y, _z + 1)].isVisible == true && _y > 0 && _y < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true)
				return false;
		}
		if (_z == countCube - 1 && mySides[FRONT] != nullptr)
		{
			int ind = myGet1DIndex(_x, _y, 0);
			InstanceData& cub = mySides[FRONT]->cubes[ind];
			if (cub.isVisible == true && 
				cubes[myGet1DIndex(_x, _y, _z - 1)].isVisible == true && _y > 0 && _y < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true && _x > 0 && _x < countCube - 1 &&
				cubes[myGet1DIndex(_x - 1, _y, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x + 1, _y, _z)].isVisible == true)
				return false;
		}
		if (_z == 0 && mySides[BACK] != nullptr)
		{
			int ind = myGet1DIndex(_x, _y, countCube - 1);
			InstanceData& cub = mySides[BACK]->cubes[ind];
			if (cub.isVisible == true &&
				cubes[myGet1DIndex(_x, _y, _z + 1)].isVisible == true && _y > 0 && _y < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true && _x > 0 && _x < countCube - 1 &&
				cubes[myGet1DIndex(_x - 1, _y, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x + 1, _y, _z)].isVisible == true)
				return false;
		}

		return true;
	}

	void myUptimazeSector()
	{
		renderCubes.clear();
		int len = -1;

		for (int x = 0; x < countCube; x++)
		{
			for (int y = 0; y < countCube; y++)
			{
				for (int z = 0; z < countCube; z++)
				{
					len++;
					// Если в этой ячейке пусто — пропускаем
					if (cubes[len].isVisible == false) continue;

					bool hasEmptyNeighbor = false;

					// Проверяем 6 направлений
					// Если куб на краю чанка (x=0 или x=15), считаем его видимым 
					// (или проверяем соседний чанк, если он есть)

					if (x == 0 || x == countCube - 1 || y == 0 || y == countCube - 1 ||
						z == 0 || z == countCube - 1)
					{
						hasEmptyNeighbor = myCheckNeighborgCub(x, y, z, len);
						//hasEmptyNeighbor = true;
					}
					else if (cubes[myGet1DIndex(x + 1, y, z)].isVisible == 1 &&
						cubes[myGet1DIndex(x - 1, y, z)].isVisible == 1 &&
						cubes[myGet1DIndex(x, y + 1, z)].isVisible == 1 &&
						cubes[myGet1DIndex(x, y - 1, z)].isVisible == 1 &&
						cubes[myGet1DIndex(x, y, z + 1)].isVisible == 1 &&
						cubes[myGet1DIndex(x, y, z - 1)].isVisible == 1)
					{
						hasEmptyNeighbor = false;
					}
					else if (cubes[len].isVisible == true)
					{
						hasEmptyNeighbor = true;
					}

					if (hasEmptyNeighbor)
					{
						myAddRenderer(cubes[len]);
					}
				}
			}
		}
	}

	void myInitialize(const glm::vec3& _posSector = glm::vec3())
	{
		cubes.reserve(countCube * countCube * countCube);

		posCollider = glm::vec3(_posSector);
		halfCollider = glm::vec3(countCube / 2) + 3.0f;

		int len = -1;

		for (int x = 0; x < countCube; x++)
		{
			for (int y = 0; y < countCube; y++)
			{
				for (int z = 0; z < countCube; z++)
				{
					len++;

					int yy = cos((x + y) * glfwGetTime() * 0.3) * 1.5f;
					int zz = cos((x + z) * glfwGetTime() * 0.2) * 1.5f;
					int xx = sin((x + x) * glfwGetTime() * 0.1) * 1.5f;

					glm::vec3 pos = glm::vec3((float)x, (float)y, (float)z);
					pos = pos + posCollider - glm::vec3(countCube / 2);

					InstanceData cub;
					cub.isVisible = false;
					cub.model = glm::translate(glm::mat4(1.0f), pos);
					cub.texIndex = 0;
					cub.index = len;

					if (y < countCube / 2 + yy + zz + xx)
					{
						cub.isVisible = true;
						cub.texIndex = 1;
					}

					if (y == countCube / 2 - 1)
						cub.texIndex = 0;

					cubes.push_back(cub);
				}
			}
		}

		cubLength = len;
	}

	void myInitializeSides(std::vector<MySector>& _sectors, int _size)
	{
		mySides.clear();
		mySides.reserve(6);
		mySides = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

		int left = myCheckSides(LEFT, index - _size, _size);
		int right = myCheckSides(RIGHT, index + _size, _size);
		int forward = myCheckSides(FRONT, index + 1, _size);
		int back = myCheckSides(BACK, index - 1, _size);
		//-------left
		if (left >= 0)
			mySides[LEFT] = &_sectors[left];
		//-------right
		if (right >= 0)
			mySides[RIGHT] = &_sectors[right];
		//-------front
		if (forward >= 0)
			mySides[FRONT] = &_sectors[forward];
		//-------back
		if (back >= 0)
			mySides[BACK] = &_sectors[back];
	}

	void myAddCube(const InstanceData& _cubData)
	{
		cubes[_cubData.index].isVisible = true;

		myUptimazeSector();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, renderCubes.size() * sizeof(InstanceData),
			renderCubes.data(), GL_STATIC_DRAW);
	}

	void myDeleteCub(const InstanceData& _cubData)
	{
		//auto it = std::find(renderCubes.begin(), renderCubes.end(), _cubData);
		cubes[_cubData.index].isVisible = false;

		renderCubes.erase(std::remove(renderCubes.begin(), renderCubes.end(), _cubData), 
			renderCubes.end());

		myUptimazeSector();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, renderCubes.size() * sizeof(InstanceData),
			renderCubes.data(), GL_STATIC_DRAW);
	}

	void myCreateSector(const glm::vec3& _pos = glm::vec3(0))
	{
		myInitialize(_pos);
		//myInitializeSSBO();
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

		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, renderCubes.size());
	}
};