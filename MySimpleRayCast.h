#pragma once
#include "gl/glfw3.h"
#include "glm/glm.hpp"
#include "MyCameraFly.h"
#include "MyInstanceData.h"
#include <vector>
#include <iostream>

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

class MySimpleRayCast
{
private:
	double mouseX, mouseY;
	int width, height;

    // Шаг 2: Стандартные границы куба (если куб 1x1x1)
    AABB box = { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f) };
public:

    bool intersectSphere(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 cubeCenter, 
        float radius, float &t) 
    {
        glm::vec3 L = cubeCenter - rayOrigin;
        float tca = glm::dot(L, rayDir); // Проекция центра на луч
        if (tca < 0) return false; // Куб за спиной

        float d2 = glm::dot(L, L) - tca * tca; // Квадрат расстояния от центра до луча
        if (d2 > radius * radius) return false; // Луч пролетел мимо

        float thc = glm::sqrt(radius * radius - d2);
        t = tca - thc; // Дистанция до точни входа
        return true;
    }

    bool intersectAABB(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 box, float& t) 
    {
        // ВАЖНО: rayDir должен быть нормализован перед вызовом!
        glm::vec3 boxMax = box + glm::vec3(0.5f);
        glm::vec3 boxMin = box - glm::vec3(0.5f);
        // Используем очень маленькое число, чтобы избежать деления на чистый ноль
        // или используем стандартную обработку бесконечности
        float tmin = -INFINITY, tmax = INFINITY;

        for (int i = 0; i < 3; ++i) 
        { // Проходим по осям X, Y, Z
            if (std::abs(rayDir[i]) > 1e-7f) 
            {
                float invD = 1.0f / rayDir[i];
                float t1 = (boxMin[i] - rayOrigin[i]) * invD;
                float t2 = (boxMax[i] - rayOrigin[i]) * invD;

                tmin = std::max(tmin, std::min(t1, t2));
                tmax = std::min(tmax, std::max(t1, t2));
            }
            else 
            {
                // Если луч параллелен плоскостям этой оси, он должен быть внутри них
                if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i])
                    return false;
            }
        }

        if (tmax >= tmin && tmax > 0) 
        {
            t = tmin;
            return true;
        }
        return false;
    }
    
    bool checkIntersectionOBB(const glm::vec3& rayOriginWorld, const glm::vec3& rayDirWorld, 
        const glm::mat4& modelMatrix, float& distance) 
    {
        // Шаг 1: Переход в локальное пространство
        glm::mat4 invModel = glm::inverse(modelMatrix);

        glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(rayOriginWorld, 1.0f));
        glm::vec3 localDir = glm::vec3(invModel * glm::vec4(rayDirWorld, 0.0f));
        // Важно: localDir НЕ обязательно нормализовать для алгоритма Slab, 
        // но это поможет получить корректную дистанцию в мировых единицах позже.

        // Шаг 3: Стандартный алгоритм Slab Method
        glm::vec3 invDir = 1.0f / (localDir + glm::vec3(1e-9f));
        glm::vec3 t0 = (box.min - localOrigin) * invDir;
        glm::vec3 t1 = (box.max - localOrigin) * invDir;

        glm::vec3 tmin_v = glm::min(t0, t1);
        glm::vec3 tmax_v = glm::max(t0, t1);

        float tmin = glm::max(glm::max(tmin_v.x, tmin_v.y), tmin_v.z);
        float tmax = glm::min(glm::min(tmax_v.x, tmax_v.y), tmax_v.z);

        if (tmax >= tmin && tmax > 0) 
        {
            // Вычисляем реальную дистанцию в мировых координатах
            // Для этого нужно умножить локальное t на масштаб луча
            distance = tmin;

            // Дополнительная коррекция дистанции (если был Scale в матрице)
            // Чтобы дистанция была верной, умножаем локальную точку на модель
            glm::vec3 hitPointLocal = localOrigin + localDir * tmin;
            glm::vec3 hitPointWorld = glm::vec3(modelMatrix * glm::vec4(hitPointLocal, 1.0f));
            distance = glm::distance(rayOriginWorld, hitPointWorld);

            return true;
        }

        return false;
    }

    int myUpdate(const std::vector<InstanceData>& _list, const MyCamera& _cam)
    {
        glm::vec3 localOrigin = _cam.myGetPos();
        glm::vec3 localDir = _cam.myGetFront();

        float closestDistance = 10.0f; // Максимальная дальность прицела
        int hitIndex = -1;
        float dist;
        
        for (int i = 0; i < _list.size(); i++)
        {
            if (_list[i].isVisible && glm::distance(glm::vec3(_list[i].model[3]), localOrigin)
                < closestDistance)
            {
                if (checkIntersectionOBB(localOrigin, localDir, _list[i].model, dist))
                {
                    if (dist < closestDistance)
                    {
                        closestDistance = dist;
                        hitIndex = i;
                    }
                }
            }
        }

        return hitIndex;
    }

    int myUpdate(const std::vector<glm::mat4>& _list, const MyCamera& _cam)
    {
        glm::vec3 localOrigin = _cam.myGetPos();
        glm::vec3 localDir = _cam.myGetFront();

        float closestDistance = 10.0f; // Максимальная дальность прицела
        int hitIndex = -1;
        float dist;

        for (int i = 0; i < _list.size(); i++)
        {
            if (glm::distance(glm::vec3(_list[i][3]), localOrigin) < closestDistance)
            {
                /*if (checkIntersectionOBB(localOrigin, localDir, _list[i], dist))
                {
                    if (dist < closestDistance)
                    {
                        closestDistance = dist;
                        hitIndex = i;
                    }
                }*/
                /*if (intersectSphere(localOrigin, localDir, _list[i][3], 0.87f))
                {
                    closestDistance = dist;
                    hitIndex = i;
                }*/
            }
        }

        return hitIndex;
    }
};