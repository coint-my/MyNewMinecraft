#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MyPhysix.h"
#include "MyTestFirstPerson.h"
#include "MyMapHight.h"

enum MyDirectionCub { NONE = -1, LEFT, RIGHT, UP, DOWN, FRONT, BACK };

class MySector
{
public:
	static const int countCube = 16;
	unsigned int cubLength;
	int index = -1;
	GLuint ssbo;

	std::vector<MySector*> mySides;

	std::vector<InstanceData> cubes;
	//std::vector<InstanceData> renderCubes;
	std::vector<MyInstanceDataRender> renderCubes;

	glm::vec3 posCollider;
	glm::vec3 halfCollider;

	~MySector()
	{
		cubes.clear();
		renderCubes.clear();
	}

private:

	struct MyHelperSectorSide
	{
		MyDirectionCub dir;
		glm::i16vec3 posCub;
	};
	
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
		//renderCubes.push_back(_data);

		renderCubes.push_back(MyPakedInstanceData(_data));
	}

	/*int myCheckSides(const MyDirectionCub& _side, int _indexSide, int _qube, int _hight)
	{
		if (_indexSide >= 0 && _indexSide <= (_qube * _qube) * _hight - 1)
		{
			if (_side == LEFT)
				return _indexSide;
			else if (_side == RIGHT && _qube * _qube - 1 >= _indexSide)
				return _indexSide;
			else if (_side == FRONT && _qube - 1 != (_indexSide - 1) % _qube)
				return _indexSide;
			else if (_side == BACK && _qube - 1 != _indexSide % _qube)
				return _indexSide;
			else if (_side == UP)
				return _indexSide;
			else if (_side == DOWN)
				return _indexSide;
		}
		return -1;
	}*/

public:

	void myInitializeSSBO()
	{
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		/*glBufferData(GL_SHADER_STORAGE_BUFFER, renderCubes.size() * sizeof(InstanceData),
			renderCubes.data(), GL_STATIC_DRAW);*/
		glBufferData(GL_SHADER_STORAGE_BUFFER, renderCubes.size() * sizeof(MyInstanceDataRender),
			renderCubes.data(), GL_STATIC_DRAW);
	}

	std::pair<MyHelperSectorSide, InstanceData*> myCheckSideSectorDirection(glm::vec3 _posCub,
		MyDirectionCub _dir)
	{
		std::pair<MyHelperSectorSide, InstanceData*> myPairSide;

		if (_dir == LEFT && _posCub.x == 0 && mySides[LEFT])
		{
			glm::i16vec3 pos = glm::i16vec3(countCube - 1, _posCub.y, _posCub.z);
			myPairSide.first = MyHelperSectorSide{ LEFT, pos };
			myPairSide.second = &mySides[LEFT]->cubes[myGet1DIndex(pos.x, pos.y, pos.z)];
			return myPairSide;
		}
		else if (_dir == RIGHT && _posCub.x == countCube - 1 && mySides[RIGHT])
		{
			glm::i16vec3 pos = glm::i16vec3(0, _posCub.y, _posCub.z);
			myPairSide.first = MyHelperSectorSide{ RIGHT, pos };
			myPairSide.second = &mySides[RIGHT]->cubes[myGet1DIndex(pos.x, pos.y, pos.z)];
			return myPairSide;
		}
		else if (_dir == FRONT && _posCub.z == countCube - 1 && mySides[FRONT])
		{
			glm::i16vec3 pos = glm::i16vec3(_posCub.x, _posCub.y, 0);
			myPairSide.first = MyHelperSectorSide{ FRONT, pos };
			myPairSide.second = &mySides[FRONT]->cubes[myGet1DIndex(pos.x, pos.y, pos.z)];
			return myPairSide;
		}
		else if (_dir == BACK && _posCub.z == 0 && mySides[BACK])
		{
			glm::i16vec3 pos = glm::i16vec3(_posCub.x, _posCub.y, countCube - 1);
			myPairSide.first = MyHelperSectorSide{ BACK, pos };
			myPairSide.second = &mySides[BACK]->cubes[myGet1DIndex(pos.x, pos.y, pos.z)];
			return myPairSide;
		}
		else if (_dir == UP && _posCub.y == countCube - 1 && mySides[UP])
		{
			glm::i16vec3 pos = glm::i16vec3(_posCub.x, 0, _posCub.z);
			myPairSide.first = MyHelperSectorSide{ UP, pos };
			myPairSide.second = &mySides[UP]->cubes[myGet1DIndex(pos.x, pos.y, pos.z)];
			return myPairSide;
		}
		else if (_dir == DOWN && _posCub.y == 0 && mySides[DOWN])
		{
			glm::i16vec3 pos = glm::i16vec3(_posCub.x, countCube - 1, _posCub.z);
			myPairSide.first = MyHelperSectorSide{ DOWN, pos };
			myPairSide.second = &mySides[DOWN]->cubes[myGet1DIndex(pos.x, pos.y, pos.z)];
			return myPairSide;
		}

		myPairSide.first = MyHelperSectorSide{ NONE, glm::i16vec3() };
		return myPairSide;
	}

	std::vector<std::pair<MyHelperSectorSide, InstanceData*>> MyCheckSedesSector(glm::vec3 _pos)
	{
		std::vector<std::pair<MyHelperSectorSide, InstanceData*>> sides;
		std::pair<MyHelperSectorSide, InstanceData*> tempPair;

		for (size_t i = 0; i < 6; i++)
		{
			tempPair = myCheckSideSectorDirection(_pos, (MyDirectionCub)i);
			if (tempPair.first.dir > -1)
				sides.push_back(tempPair);
		}

		return sides;
	}

	bool myIsCheckBoundingSector(int _x, int _y, int _z, 
		std::vector<std::pair<MyHelperSectorSide, InstanceData*>>& _list)
	{
		InstanceData* left = nullptr;
		InstanceData* right = nullptr;
		InstanceData* front = nullptr;
		InstanceData* back = nullptr;
		InstanceData* up = nullptr;
		InstanceData* down = nullptr;

		if (_x - 1 < 0)
		{
			bool isFlag = false;
			for (auto& item : _list)
				if (item.first.dir == LEFT)
				{
					left = item.second;
					isFlag = true;
					break;
				}
			if (!isFlag)
				return true;
		}
		else
		{
			left = &cubes[myGet1DIndex(_x - 1, _y, _z)];
		}
		if (_x + 1 == countCube)
		{
			bool isFlag = false;
			for (auto& item : _list)
				if (item.first.dir == RIGHT)
				{
					right = item.second;
					isFlag = true;
					break;
				}
			if (!isFlag)
				return true;
		}
		else
		{
			right = &cubes[myGet1DIndex(_x + 1, _y, _z)];
		}
		if (_z - 1 < 0)
		{
			bool isFlag = false;
			for (auto& item : _list)
				if (item.first.dir == BACK)
				{
					back = item.second;
					isFlag = true;
					break;
				}
			if (!isFlag)
				return true;
		}
		else
		{
			back = &cubes[myGet1DIndex(_x, _y, _z - 1)];
		}
		if (_z + 1 == countCube)
		{
			bool isFlag = false;
			for (auto& item : _list)
				if (item.first.dir == FRONT)
				{
					front = item.second;
					isFlag = true;
					break;
				}
			if (!isFlag)
				return true;
		}
		else
		{
			front = &cubes[myGet1DIndex(_x, _y, _z + 1)];
		}
		if (_y - 1 < 0)
		{
			bool isFlag = false;
			for (auto& item : _list)
				if (item.first.dir == DOWN)
				{
					down = item.second;
					isFlag = true;
					break;
				}
			if (!isFlag)
				return true;
		}
		else
		{
			down = &cubes[myGet1DIndex(_x, _y - 1, _z)];
		}
		if (_y + 1 == countCube)
		{
			bool isFlag = false;
			for (auto& item : _list)
				if (item.first.dir == UP)
				{
					up = item.second;
					isFlag = true;
					break;
				}
			if (!isFlag)
				return true;
		}
		else
		{
			up = &cubes[myGet1DIndex(_x, _y + 1, _z)];
		}

		if (left->isVisible && right->isVisible && back->isVisible && front->isVisible &&
			down->isVisible && up->isVisible)
			return false;
		return true;
	}

	bool myCheckNeighborgCub(int _x, int _y, int _z)
	{
		std::vector<std::pair<MyHelperSectorSide, InstanceData*>> listSides =
			MyCheckSedesSector(glm::vec3(_x, _y, _z));

		if (listSides.size() > 0)
			return myIsCheckBoundingSector(_x, _y, _z, listSides);

		/*if (_x == 0 && _z == countCube - 1 && mySides[LEFT] != nullptr && mySides[FRONT] != nullptr)
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
		if (_y == 0 && mySides[UP] != nullptr)
		{
			int ind = myGet1DIndex(_x, countCube - 1, _z);

			if(mySides[UP]->cubes[ind].isVisible == true && 
				cubes[myGet1DIndex(_x, _y + 1, _z)].isVisible == true && _z > 0 && _z < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y, _z + 1)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y, _z - 1)].isVisible == true && _x > 0 && _x < countCube - 1 &&
				cubes[myGet1DIndex(_x - 1, _y, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x + 1, _y, _z)].isVisible == true)
				return false;
		}
		if (_y == countCube - 1 && mySides[DOWN] != nullptr)
		{
			int ind = myGet1DIndex(_x, 0, _z);

			if (mySides[DOWN]->cubes[ind].isVisible == true &&
				cubes[myGet1DIndex(_x, _y - 1, _z)].isVisible == true && _z > 0 && _z < countCube - 1 &&
				cubes[myGet1DIndex(_x, _y, _z + 1)].isVisible == true &&
				cubes[myGet1DIndex(_x, _y, _z - 1)].isVisible == true && _x > 0 && _x < countCube - 1 &&
				cubes[myGet1DIndex(_x - 1, _y, _z)].isVisible == true &&
				cubes[myGet1DIndex(_x + 1, _y, _z)].isVisible == true)
				return false;
		}*/

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
						hasEmptyNeighbor = myCheckNeighborgCub(x, y, z);
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

		cubes[0].isVisible = true;
	}

	void myAllCubesRender()
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

					myAddRenderer(cubes[len]);
				}
			}
		}
	}

	void myInitializeRandomHeight(const glm::vec3& _posSector = glm::vec3())
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
					cub.pos = pos;
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

	void myInitializeSectorParameters(const glm::vec3& _posSector, int _textureIndex,
		bool _isVisible)
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

					glm::vec3 pos = glm::vec3((float)x, (float)y, (float)z);
					pos = pos + posCollider - glm::vec3(countCube / 2);

					InstanceData cub;
					cub.isVisible = _isVisible;
					cub.pos = pos;
					cub.texIndex = _textureIndex;
					cub.index = len;

					cubes.push_back(cub);
				}
			}
		}

		cubLength = len;
	}

	void myInitializeHight(const MyMapHight& _map, const glm::vec3& _posSector, int _sectorLen,
		int _landscapeY, int _sectorHight)
	{
		if (_landscapeY == _sectorHight)
		{
			cubes.reserve(countCube * countCube * countCube);

			posCollider = glm::vec3(_posSector);
			halfCollider = glm::vec3(countCube / 2) + 3.0f;

			int len = -1;
			const GLuint widT = _sectorLen * 2;
			const GLuint heiT = _sectorLen * 2;
			const float widHeightStep = (float)(countCube * widT) / _map.myGetWidth();
			const float heiHeightStep = (float)(countCube * heiT) / _map.myGetHeight();
			glm::vec3 color = _map.myGetColor(0, 0);

			for (int x = 0; x < countCube; x++)
			{
				for (int y = 0; y < countCube; y++)
				{
					for (int z = 0; z < countCube; z++)
					{
						int heiCurrent = (((index % (int)pow(_sectorLen * 2, 2)) / heiT) * countCube + x);
						int widCurrent = ((index % widT) * countCube + z);
						int xResult = (int)(widCurrent / widHeightStep);
						int zResult = (int)(heiCurrent / heiHeightStep);
						color = _map.myGetColor(xResult, zResult);
						len++;

						glm::vec3 pos = glm::vec3((float)x, (float)y, (float)z);
						pos = pos + posCollider - glm::vec3(countCube / 2);

						InstanceData cub;
						cub.isVisible = false;
						cub.pos = pos;
						cub.texIndex = 0;
						cub.index = len;

						int yy = (color.r + 1) / (countCube - 2);

						if (y < yy + 1)
							cub.isVisible = true;

						if (y < countCube / 2 - 1)//this is grass and ground
							cub.texIndex = 1;
						if (y < 4)//this is rock
							cub.texIndex = 3;
						if (y < 2)//this is sad
							cub.texIndex = 2;

						cubes.push_back(cub);
					}
				}
			}

			cubLength = len;
		}
		else if (_landscapeY < _sectorHight)
		{
			myInitializeSectorParameters(_posSector, 0, false);
		}
		else if (_landscapeY > _sectorHight)
		{
			myInitializeSectorParameters(_posSector, 1, true);
		}
	}

	void myInitializeSides(std::vector<MySector>& _sectors, int _size, int _hight)
	{
		mySides.clear();
		mySides.reserve(6);
		mySides = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

		std::vector<MySector*> tempSector;
		for (auto& item : _sectors)
			if (MyPhysix::AABBIntersect(posCollider, glm::vec3(countCube), item.posCollider, glm::vec3(2)))
				tempSector.push_back(&item);

		glm::vec3 testRectIntersectLeft = glm::vec3(posCollider) + glm::vec3(-countCube, 0, 0);
		glm::vec3 testRectIntersectRight = glm::vec3(posCollider) + glm::vec3(countCube, 0, 0);
		glm::vec3 testRectIntersectBack = glm::vec3(posCollider) + glm::vec3(0, 0, -countCube);
		glm::vec3 testRectIntersectFront = glm::vec3(posCollider) + glm::vec3(0, 0, countCube);
		glm::vec3 testRectIntersectUp = glm::vec3(posCollider) + glm::vec3(0, countCube, 0);
		glm::vec3 testRectIntersectDown = glm::vec3(posCollider) + glm::vec3(0, -countCube, 0);

		for (auto& item : tempSector)
			if (MyPhysix::AABBIntersect(testRectIntersectLeft, glm::vec3(1), item->posCollider,
				item->halfCollider))
				mySides[LEFT] = item;

		for (auto& item : tempSector)
			if (MyPhysix::AABBIntersect(testRectIntersectRight, glm::vec3(1), item->posCollider,
				item->halfCollider))
				mySides[RIGHT] = item;

		for (auto& item : tempSector)
			if (MyPhysix::AABBIntersect(testRectIntersectBack, glm::vec3(1), item->posCollider,
				item->halfCollider))
				mySides[BACK] = item;

		for (auto& item : tempSector)
			if (MyPhysix::AABBIntersect(testRectIntersectFront, glm::vec3(1), item->posCollider,
				item->halfCollider))
				mySides[FRONT] = item;

		for (auto& item : tempSector)
			if (MyPhysix::AABBIntersect(testRectIntersectUp, glm::vec3(1), item->posCollider,
				item->halfCollider))
				mySides[UP] = item;

		for (auto& item : tempSector)
			if (MyPhysix::AABBIntersect(testRectIntersectDown, glm::vec3(1), item->posCollider,
				item->halfCollider))
				mySides[DOWN] = item;

		//int left = myCheckSides(LEFT, index - _size, _size, _hight);
		//int right = myCheckSides(RIGHT, index + _size, _size, _hight);
		//int forward = myCheckSides(FRONT, index + 1, _size, _hight);
		//int back = myCheckSides(BACK, index - 1, _size, _hight);
		//int up = myCheckSides(DOWN, index - (_size * _size), _size, _hight);
		//int down = myCheckSides(UP, index + (_size * _size), _size, _hight);
		////-------left
		//if (left >= 0)
		//	mySides[LEFT] = &_sectors[left];
		////-------right
		//if (right >= 0)
		//	mySides[RIGHT] = &_sectors[right];
		////-------front
		//if (forward >= 0)
		//	mySides[FRONT] = &_sectors[forward];
		////-------back
		//if (back >= 0)
		//	mySides[BACK] = &_sectors[back];
		////-------up
		//if (up >= 0)
		//	mySides[DOWN] = &_sectors[up];
		////-------down
		//if (down >= 0)
		//	mySides[UP] = &_sectors[down];
	}

	void myAddCube(const InstanceData& _cubData)
	{
		cubes[_cubData.index].isVisible = true;

		myUptimazeSector();

		myInitializeSSBO();
	}

	void myDeleteCub(const InstanceData& _cubData)
	{
		cubes[_cubData.index].isVisible = false;

		MyInstanceDataRender render = MyPakedInstanceData(_cubData);
		renderCubes.erase(std::remove(renderCubes.begin(), renderCubes.end(), render),
			renderCubes.end());

		/*renderCubes.erase(std::remove(renderCubes.begin(), renderCubes.end(), _cubData), 
			renderCubes.end());*/

		//myUptimazeSector();

		myInitializeSSBO();
	}

	void myCreateSector(const glm::vec3& _pos = glm::vec3(0))
	{
		myInitializeRandomHeight(_pos);
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