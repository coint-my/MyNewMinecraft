#pragma once

#include "glm/glm.hpp"

struct InstanceData
{
    glm::mat4 model;
    int texIndex;      // Индекс слоя в массиве текстур
    int isVisible;     // Рисовать куб
    int index;         // индекс в массиве секторов
    float padding;  // Выравнивание до 16 байт (std430 требует этого для корректной работы)
};

class MyDirectInstansing
{
public:
    GLuint staticSSBO;

    void myInitialize(const std::vector<InstanceData>& _listInstance, const GLuint _count)
    {
        glGenBuffers(1, &staticSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, staticSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, _count * sizeof(InstanceData),
            _listInstance.data(), GL_STATIC_DRAW);
    }
};