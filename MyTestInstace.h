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
    MyMapHight mapH;
public:
    GLuint texArray;
    std::unique_ptr<std::pair<InstanceData&, GLuint>> rayCastCub;
    std::unique_ptr<std::pair<InstanceData&, GLuint>> rayCastCubAdd;
    std::vector<MySector> listSector;
private:
    
    void myLoadMapHight()
    {
        int width, height, channels;
        unsigned char* imageMapHight = stbi_load("texture/myCloudRandom_512.png", &width, &height, &channels, 4);
        mapH = MyMapHight(width, height, imageMapHight);

        stbi_image_free(imageMapHight);
    }

    void myLoadTexture()
    {
        int width, height, channels;
        unsigned char* imageData1 = stbi_load("texture/block_ground_grass1.png", &width, &height, &channels, 4);
        unsigned char* imageData2 = stbi_load("texture/block_ground1.png", &width, &height, &channels, 4);
        unsigned char* imageData3 = stbi_load("texture/myRockDirty_1.png", &width, &height, &channels, 4);
        unsigned char* imageData4 = stbi_load("texture/mySad_1.png", &width, &height, &channels, 4);
        unsigned char* imageData5 = stbi_load("texture/test4.png", &width, &height, &channels, 4);
        
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
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        // Отвязка текстуры
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        // Освобождение памяти, занятой stb_image
        stbi_image_free(imageData1);
        stbi_image_free(imageData2);
        stbi_image_free(imageData3);
        stbi_image_free(imageData4);
        stbi_image_free(imageData5);
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
    //depricate
    //void myChangeCub(const InstanceData& _cubData, GLuint _sector)
    //{
    //    // 1. Копируем только этот куб в GPU
    //    glBindBuffer(GL_SHADER_STORAGE_BUFFER, listSector[_sector].ssbo);
    //    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
    //        _cubData.index * sizeof(InstanceData),      // Смещение до нужного куба
    //        sizeof(InstanceData),                       // Размер одного куба
    //        &_cubData                                   // Откуда берем данные
    //    );
    //}

    void myAddCube(const InstanceData& _cubData, GLuint _sector)
    {
        listSector[_sector].myAddCube(_cubData);
    }

    void myDeleteCub(const InstanceData& _cubData, GLuint _sector)
    {
        listSector[_sector].myDeleteCub(_cubData);
    }

    /*void myCreateSector(glm::vec3 _pos, int _index)
    {
        MySector sector;
        sector.myCreateSector(_pos * (float)sector.countCube);
        sector.index = _index;
        listSector.push_back(sector);
    }*/

    void myCreateSectorHight(glm::vec3 _pos, int _index, int _sectorLen, int _landscape,
        int _sectorHight)
    {
        MySector sector;
        sector.index = _index;
        sector.myInitializeHight(mapH, _pos * (float)sector.countCube, _sectorLen, _landscape,
            _sectorHight);
        listSector.push_back(sector);
    }

	void myInitialize(GLFWwindow* _window, MyTestFirstPerson& _testPerson)
	{
        cub.myInitialize();
        myLoadMapHight();

        /*glm::i16vec3 p = glm::i16vec3(256, -256, 0);
        char test1 = std::numeric_limits<char>::max();
        std::cout << "unsigned = " << (int)test1 << " size = " << sizeof(test1) << std::endl;*/

        /*int16_t x = -10050, y = 12000, z = -1200;
        uint32_t packedXY = ((uint32_t)(uint16_t)x << 16) | (uint32_t)(uint16_t)y;

        int16_t x_res = (int16_t)(packedXY >> 16);
        int16_t y_res = (int16_t)(packedXY & 0xFFFF);*/

        int sectorLen = 2;
        int hight = 2;
        int len = -1;
        for (int y = 0; y < hight; y++)
        {
            for (int x = -sectorLen; x < sectorLen; x++)
            {
                for (int z = -sectorLen; z < sectorLen; z++)
                {
                    len++;
                    myCreateSectorHight(glm::vec3(x, y, z), len, sectorLen, 1, y);
                }
            }
        }

        for (auto& sec : listSector)
        {
            sec.myInitializeSides(listSector, sectorLen * 2, hight);
        }
        int countRenderCub = 0;
        for (auto& sec : listSector)
        {
            sec.myUptimazeSector();
            //sec.myAllCubesRender();
            sec.myInitializeSSBO();
            countRenderCub += sec.renderCubes.size();
        }
        std::cout << "render cubes = " << countRenderCub << std::endl;
        myLoadTexture();
	}

    void myAddCubANormal(const glm::i16vec3 _norm, std::vector<std::pair<InstanceData&, GLuint>>& _cubes,
        std::unique_ptr<std::pair<InstanceData&, GLuint>>& _ptrAddCub)
    {
        glm::i16vec3 addCub = rayCastCub->first.pos + _norm;

        for (size_t i = 0; i < _cubes.size(); i++)
        {
            if (_cubes[i].first.pos == addCub)
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
        rayCastCubAdd = nullptr;

        for (int i = 0; i < listSector.size(); i++)
        {
            listSector[i].myUpdateSector(_person, _window, pairCubeRay, i);
        }

        for (size_t i = 0; i < pairCubeRay.size(); i++)
        {
            float t;

            if (pairCubeRay[i].first.isVisible &&
                MySimpleRayCast::intersectAABB(_person.camFps.myGetPos(), _person.camFps.myGetFront(), 
                pairCubeRay[i].first.pos, t))
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

            glm::vec3 normalCub = MySimpleRayCast::getNormal(hitPoint, rayCastCub->first.pos);
            myAddCubANormal(normalCub, pairCubeRay, rayCastCubAdd);
        }
    }

    /*void myRenderSectorCurrent()
    {
        glBindVertexArray(myGetVAO());
        listSector[55].myRenderSector();
    }*/

    void myRenderSectorFromShadow(const glm::vec3& _playerPos)
    {
        std::vector<MySector*> data;

        for (auto& sector : listSector)
        {
            if (MyPhysix::AABBIntersect(sector.posCollider, glm::vec3(16.f),
                _playerPos, glm::vec3(16.f)))
                data.push_back(&sector);
        }

        glBindVertexArray(myGetVAO());

        for (auto& sector : data)
            sector->myRenderSector();
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