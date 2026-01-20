#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MySimpleRayCast.h"

class MyPhysix
{
public:
    struct CharacterController
    {
        glm::vec3 position;   // позиция персонажа
        glm::vec3 velocity;   // скорость
        glm::vec3 halfSize;   // размеры капсулы/AABB

        bool grounded;        // стоит ли на земле

        float moveSpeed;      // скорость ходьбы
        float jumpForce;      // сила прыжка
        float gravity;        // гравитация
    };

    struct BoxCollider
    {
        glm::vec3 halfSize{ 0.5f };  // половина размера
    };

    struct PhysicsBody
    {
        glm::vec3 position;   // позиция в мире
        glm::vec3 velocity;   // скорость
        BoxCollider collider; // коллайдер
        bool isStatic;        // статичный или динамический
        float mass;      // масса

        float friction; // коэффициент трения (0..1)
        float frictionFly = 0.5f;
        bool isGrounded = false;
    };

    struct MyCube
    {
        //модель куба
        glm::mat4 model;
        //колайдер куба
        MyPhysix::PhysicsBody boxPhysix;
        //индекс текстуры куба
        unsigned int texture;
        //существует куб
        bool isVisible;
    };

    static bool AABBIntersect(glm::vec3 posA, glm::vec3 halfA, glm::vec3 posB, glm::vec3 halfB)
    {
        return
            abs(posA.x - posB.x) <= (halfA.x + halfB.x) &&
            abs(posA.y - posB.y) <= (halfA.y + halfB.y) &&
            abs(posA.z - posB.z) <= (halfA.z + halfB.z);
    }
};