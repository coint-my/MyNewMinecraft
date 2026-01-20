#pragma once

#include "glm/glm.hpp"

struct InstanceData
{
    glm::mat4 model;
    int texIndex;      // Индекс слоя в массиве текстур
    int isVisible;     // Рисовать куб
    float padding[2];  // Выравнивание до 16 байт (std430 требует этого для корректной работы)
};