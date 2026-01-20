#pragma once

//#include "vector"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gl/glew.h"
#include "MyPrimitiveCube.h"
#include "MySimpleRayCast.h"
#include "MyPhysix.h"
//#include "MyTestFirstPerson.h"
#include "MySector.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

class MyTestInstance
{
private:
    GLuint texArray;

    MyPrimitiveCube cub;
    MyPhysix myPhysix;
public:
    std::unique_ptr<glm::mat4> rayCastCub;
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

	const int INSTANCE_COUNT = 16384;

    inline GLuint myGetVAO() const { return cub.myGetVAO(); }

    //inline int myGetIndexCub() const { return indCub; }

    //inline std::unique_ptr<glm::mat4> myGetMatrixRayCastCub() const { return rayCastCub; }

    void myBindTexture(MyShader& _shader)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
        _shader.setInt("uTexArray", 1);
    }

	void myInitialize(GLFWwindow* _window, MyTestFirstPerson& _testPerson)
	{
        //rayCastCub = nullptr;
        cub.myInitialize();

        MySector sectorOne;
        sectorOne.myInitialize();
        sectorOne.myAddInRender(listInstanceData);
        listSector.push_back(sectorOne);

        MySector sectorTwo;
        sectorTwo.myInitialize(glm::vec3((float)sectorTwo.countCube, 0.0f, 0.0f));
        sectorTwo.myAddInRender(listInstanceData);
        listSector.push_back(sectorTwo);

        MySector sectorThree;
        sectorThree.myInitialize(glm::vec3((float)sectorThree.countCube, 0.0f,
            (float)sectorThree.countCube));
        sectorThree.myAddInRender(listInstanceData);
        listSector.push_back(sectorThree);

        MySector sectorFour;
        sectorFour.myInitialize(glm::vec3(-(float)sectorFour.countCube, 0.0f,
            0.0f));
        sectorFour.myAddInRender(listInstanceData);
        listSector.push_back(sectorFour);

        MySector sectorFive;
        sectorFive.myInitialize(glm::vec3(-(float)sectorFive.countCube, 0.0f,
            -(float)sectorFive.countCube));
        sectorFive.myAddInRender(listInstanceData);
        listSector.push_back(sectorFive);

        myLoadTexture();
	}

    void myUpdate(MyTestFirstPerson& _person, GLFWwindow* _window)
    {
        _person.MyCharacterHandle(_window);

        for (int i = 0; i < listSector.size(); i++)
        {
            listSector[i].myUpdateSector(_person, _window, rayCastCub);
        }
    }
};