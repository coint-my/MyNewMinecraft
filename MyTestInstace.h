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

struct Plane { glm::vec3 normal; float distance; };

class MyTestInstance
{
private:

    MyPrimitiveCube cub;
    MyPhysix myPhysix;
public:
    GLuint texArray;
    std::unique_ptr<std::pair<InstanceData&, GLuint>> rayCastCub;
    std::unique_ptr<std::pair<InstanceData&, GLuint>> rayCastCubAdd;
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
        //listInstanceData[0].texIndex = 1;
        //listInstanceData[1].texIndex = 2;
        //listInstanceData[2].texIndex = 3;
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
        //listInstanceData.clear();
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

private:
    std::vector<Plane> myGetFrustumPlanes(const glm::mat4& viewProj) 
    {
        std::vector<Plane> planes(6);
        // Матрица в GLM column-major, поэтому индексы такие:
        // Left, Right, Bottom, Top, Near, Far
        planes[0] = { glm::vec3(viewProj[0][3] + viewProj[0][0], viewProj[1][3] + 
            viewProj[1][0], viewProj[2][3] + viewProj[2][0]), viewProj[3][3] + viewProj[3][0] };
        planes[1] = { glm::vec3(viewProj[0][3] - viewProj[0][0], viewProj[1][3] - 
            viewProj[1][0], viewProj[2][3] - viewProj[2][0]), viewProj[3][3] - viewProj[3][0] };
        planes[2] = { glm::vec3(viewProj[0][3] + viewProj[0][1], viewProj[1][3] + 
            viewProj[1][1], viewProj[2][3] + viewProj[2][1]), viewProj[3][3] + viewProj[3][1] };
        planes[3] = { glm::vec3(viewProj[0][3] - viewProj[0][1], viewProj[1][3] - 
            viewProj[1][1], viewProj[2][3] - viewProj[2][1]), viewProj[3][3] - viewProj[3][1] };
        planes[4] = { glm::vec3(viewProj[0][3] + viewProj[0][2], viewProj[1][3] + 
            viewProj[1][2], viewProj[2][3] + viewProj[2][2]), viewProj[3][3] + viewProj[3][2] };
        planes[5] = { glm::vec3(viewProj[0][3] - viewProj[0][2], viewProj[1][3] - 
            viewProj[1][2], viewProj[2][3] - viewProj[2][2]), viewProj[3][3] - viewProj[3][2] };

        // Нормализуем плоскости
        for (auto& p : planes) 
        {
            float length = glm::length(p.normal);
            p.normal /= length;
            p.distance /= length;
        }
        return planes;
    }

    bool isSphereInFrustum(const glm::vec3& center, float radius, const std::vector<Plane>& planes) 
    {
        for (const auto& plane : planes) 
        {
            // Расстояние от центра сферы до плоскости
            if (glm::dot(plane.normal, center) + plane.distance < -radius) 
            {
                return false; // Сфера полностью за плоскостью
            }
        }
        return true; // Сфера пересекает или внутри пирамиды
    }

public:

    void myChangeCub(const InstanceData& _cubData, GLuint _sector)
    {
        // 1. Копируем только этот куб в GPU
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, listSector[_sector].ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER,
            _cubData.index * sizeof(InstanceData),      // Смещение до нужного куба
            sizeof(InstanceData),                       // Размер одного куба
            &_cubData                                   // Откуда берем данные
        );
    }

    void myCreateSector(glm::vec3 _pos)
    {
        MySector sector;
        sector.myCreateSector(_pos * (float)sector.countCube);
        listSector.push_back(sector);
    }

	void myInitialize(GLFWwindow* _window, MyTestFirstPerson& _testPerson)
	{
        cub.myInitialize();

        for (int z = -5; z < 5; z++)
        {
            for (int x = -5; x < 5; x++)
            {
                myCreateSector(glm::vec3(x, 0, z));
            }
        }

        myLoadTexture();
	}

    void myAddCubANormal(const glm::vec3 _norm, std::vector<std::pair<InstanceData&, GLuint>>& _cubes,
        std::unique_ptr<std::pair<InstanceData&, GLuint>>& _ptrAddCub)
    {
        glm::vec3 addCub = glm::vec3(rayCastCub->first.model[3]) + _norm;

        for (size_t i = 0; i < _cubes.size(); i++)
        {
            if (glm::vec3(_cubes[i].first.model[3]) == addCub)
            {
                _ptrAddCub = std::make_unique<std::pair<InstanceData&, GLuint>>(_cubes[i]);
            }
        }
    }

    void myUpdate(MyTestFirstPerson& _person, GLFWwindow* _window)
    {
        _person.MyCharacterHandle(_window);

        std::vector<std::pair<InstanceData&, GLuint>> pairCubeRay;
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
                    rayCastCub = std::make_unique<std::pair<InstanceData&, GLuint>>(pairCubeRay[i]);
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

    void myRenderSectorCurrent()
    {
        glBindVertexArray(myGetVAO());
        listSector[55].myRenderSector();
    }

    void myRenderer(const glm::mat4& _frustumMatrix)
    {
        auto planes = myGetFrustumPlanes(_frustumMatrix);
        //float sectorRadius = 14.0f; // Для сектора 16x16x16
        float sectorRadius = 10.0f;
        glBindVertexArray(myGetVAO());

        for (auto& sector : listSector)
        {
            if (!isSphereInFrustum(sector.posCollider, sectorRadius, planes))
                continue; // Пропускаем весь сектор (тысячи кубов)

            sector.myRenderSector();
        }
    }
};