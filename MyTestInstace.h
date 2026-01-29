#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gl/glew.h"
#include "MyPrimitiveCube.h"
#include "MySimpleRayCast.h"
#include "MyPhysix.h"
#include "MySector.h"
#include "MyShader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

class MyTestInstance
{
private:
    GLuint texArray;

    MyPrimitiveCube cub;
    MyPhysix myPhysix;
public:
    std::unique_ptr<std::pair<MyPhysix::MyCube, GLuint>> rayCastCub;
    std::unique_ptr<std::pair<MyPhysix::MyCube, GLuint>> rayCastCubAdd;
    std::vector<InstanceData> listInstanceData;
    std::vector<MySector> listSector;
private:
    
    void myLoadTexture()
    {
        int width, height, channels;
        unsigned char* imageData1 = stbi_load("texture/test1.png", &width, &height, &channels, 4);
        unsigned char* imageData2 = stbi_load("texture/test2.png", &width, &height, &channels, 4);
        unsigned char* imageData3 = stbi_load("texture/test3.png", &width, &height, &channels, 4);
        unsigned char* imageData4 = stbi_load("texture/test4.png", &width, &height, &channels, 4);

        if (imageData1) 
        {
            // Данные загружены
            std::cout << "Image loaded: " << width << "x" << height << ", Channels: " << channels << std::endl;
        }
        else
            std::cerr << "Failed to load image!" << std::endl; // Ошибка загрузки

        glGenTextures(1, &texArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);

        // Установите фильтрацию БЕЗ мип-мапов
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Создаем хранилище: ширина, высота, количество_слоев
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, 4);

        // Загружаем каждую текстуру в свой слой
        listInstanceData[0].texIndex = 1;
        listInstanceData[1].texIndex = 2;
        listInstanceData[2].texIndex = 3;
        // data — пиксели i-й текстуры
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, GL_RGBA,
            GL_UNSIGNED_BYTE, imageData1);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, width, height, 1, GL_RGBA,
            GL_UNSIGNED_BYTE, imageData2);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, width, height, 1, GL_RGBA,
            GL_UNSIGNED_BYTE, imageData3);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 3, width, height, 1, GL_RGBA,
            GL_UNSIGNED_BYTE, imageData4);

        // Генерация Mipmaps (рекомендуется)
        //glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        // Отвязка текстуры
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        // Освобождение памяти, занятой stb_image
        stbi_image_free(imageData1);
        stbi_image_free(imageData2);
        stbi_image_free(imageData3);
        stbi_image_free(imageData4);
    }

public:
    ~MyTestInstance()
    {
        listInstanceData.clear();
        /*for (MyPhysix::PhysicsBody* ptr : listPhysix)
        {
            if ((*ptr).isStatic)
            {
                delete ptr;
                ptr = nullptr;
            }
        }
        auto new_end = std::remove(listPhysix.begin(), listPhysix.end(), nullptr);
        listPhysix.erase(new_end, listPhysix.end());*/

        std::cout << "delete TestInstance" << std::endl;
    }

    inline GLuint myGetVAO() const { return cub.myGetVAO(); }

    void myBindTexture(MyShader& _shader)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
        _shader.setInt("uTexArray", 1);
    }

    InstanceData* myOffCube()
    {
        if (rayCastCub)
        {
            InstanceData& data = listInstanceData[rayCastCub->first.index];
            data.isVisible = false;

            GLuint indexSectorCub = rayCastCub->first.index - (listSector[rayCastCub->second].cubLength *
                rayCastCub->second);

            listSector[rayCastCub->second].cubes[indexSectorCub].isVisible = false;

            return &data;
        }
        return nullptr;
    }

    InstanceData* myOnCube()
    {
        if (rayCastCubAdd)
        {
            InstanceData& data = listInstanceData[rayCastCubAdd->first.index];
            data.isVisible = true;

            GLuint indexSectorCub = rayCastCubAdd->first.index -
                (listSector[rayCastCubAdd->second].cubLength * rayCastCubAdd->second);

            listSector[rayCastCubAdd->second].cubes[indexSectorCub].isVisible = true;

            return &data;
        }
        return nullptr;
    }

    void myCreateSector(glm::vec3 _pos)
    {
        MySector sector;
        sector.myInitialize(_pos * (float)sector.countCube);
        sector.myAddInRender(listInstanceData);
        listSector.push_back(sector);
    }

	void myInitialize(GLFWwindow* _window, MyTestFirstPerson& _testPerson)
	{
        cub.myInitialize();

        myCreateSector(glm::vec3(0));
        myCreateSector(glm::vec3(0, 0, 1));
        myCreateSector(glm::vec3(0, 0, -1));
        myCreateSector(glm::vec3(1, 0, 0));
        myCreateSector(glm::vec3(1, 0, 1));
        myCreateSector(glm::vec3(1, 0, -1));
        myCreateSector(glm::vec3(-1, 0, 0));
        myCreateSector(glm::vec3(-1, 0, 1));
        myCreateSector(glm::vec3(-1, 0, -1));

        myLoadTexture();
	}

    void myAddCubANormal(const glm::vec3 _norm, std::vector<std::pair<MyPhysix::MyCube, GLuint>>& _cubes,
        std::unique_ptr<std::pair<MyPhysix::MyCube, GLuint>>& _ptrAddCub)
    {
        glm::vec3 addCub = rayCastCub->first.boxPhysix.position + _norm;

        for (size_t i = 0; i < _cubes.size(); i++)
        {
            if (_cubes[i].first.boxPhysix.position == addCub)
            {
                _ptrAddCub = std::make_unique<std::pair<MyPhysix::MyCube, GLuint>>(_cubes[i]);
            }
        }
    }

    void myUpdate(MyTestFirstPerson& _person, GLFWwindow* _window)
    {
        _person.MyCharacterHandle(_window);

        std::vector<std::pair<MyPhysix::MyCube, GLuint>> pairCubeRay;
        float closestDistanceCast = 10.0f; // Максимальная дальность прицела
        rayCastCub = nullptr;

        for (int i = 0; i < listSector.size(); i++)
        {
            listSector[i].myUpdateSector(_person, _window, pairCubeRay, i);
        }

        for (size_t i = 0; i < pairCubeRay.size(); i++)
        {
            float t;

            if (pairCubeRay[i].first.isVisible &&
                MySimpleRayCast::intersectAABB(_person.camFps.myGetPos(), _person.camFps.myGetFront(), 
                pairCubeRay[i].first.model[3], t))
            {
                if (t < closestDistanceCast)
                {
                    closestDistanceCast = t;
                    rayCastCub = std::make_unique<std::pair<MyPhysix::MyCube, GLuint>>(pairCubeRay[i]);
                }
            }
        }

        if (rayCastCub)
        {
            glm::vec3 hitPoint = _person.camFps.myGetPos() + _person.camFps.myGetFront() * 
                closestDistanceCast;

            glm::vec3 normalCub = MySimpleRayCast::getNormal(hitPoint, rayCastCub->first.model[3]);
            rayCastCubAdd = nullptr;
            myAddCubANormal(normalCub, pairCubeRay, rayCastCubAdd);
        }
    }
};