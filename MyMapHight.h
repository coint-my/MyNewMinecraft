#pragma once

#include "glm/glm.hpp"
#include <vector>

class MyMapHight
{
private:
	int width, height;
	std::vector<std::vector<glm::vec3>> map;
public:
	MyMapHight() : width(0), height(0) { }

	MyMapHight(int _width, int _height, unsigned char* _data) : width(_width), height(_height)
	{
		map.resize(width, std::vector<glm::vec3>(height, glm::vec3(0)));

		for (size_t h = 0; h < width; h++)
		{
			for (size_t w = 0; w < height; w++)
			{
				map[w][h] = glm::vec3(_data[(h * width + w) * 4], _data[(h * width + w) * 4 + 1],
					_data[(h * width + w) * 4 + 2]);
			}
		}

		printf("test mapHeight\n");
	}

	inline glm::vec3 myGetColor(int _w, int _h) const
	{
		return map[_w][_h];
	}

	inline GLuint myGetWidth() const { return width; }
	inline GLuint myGetHeight() const { return height; }
};