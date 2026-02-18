#pragma once

#include "glm/glm.hpp"

struct InstanceData
{
    glm::mat4 model;
    int texIndex;      // Индекс слоя в массиве текстур
    int isVisible;     // Рисовать куб
    int index;         // индекс в массиве секторов
    float padding;  // Выравнивание до 16 байт (std430 требует этого для корректной работы)

    bool operator==(const InstanceData& other) const 
    { 
        /*return texIndex == other.texIndex && isVisible == other.isVisible &&
            index == other.index && model[3][0] == other.model[3][0] && 
            model[3][1] == other.model[3][1] && model[3][2] == other.model[3][2] && 
            model[3][3] == other.model[3][3];*/

        return index == other.index;
    }
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