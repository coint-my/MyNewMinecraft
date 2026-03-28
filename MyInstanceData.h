#pragma once

#include "glm/glm.hpp"

struct InstanceData
{
    //glm::mat4 model;
    glm::i16vec3 pos;
    short texIndex;      // Индекс слоя в массиве текстур
    short isVisible;     // Рисовать куб
    int index;         // индекс в массиве секторов
    //float padding, padding2;  // Выравнивание до 16 байт (std430 требует этого для корректной работы)

    bool operator==(const InstanceData& other) const 
    { 
        /*return texIndex == other.texIndex && isVisible == other.isVisible &&
            index == other.index && model[3][0] == other.model[3][0] && 
            model[3][1] == other.model[3][1] && model[3][2] == other.model[3][2] && 
            model[3][3] == other.model[3][3];*/

        return index == other.index;
    }
};

struct MyInstanceDataRender
{
    uint32_t x_y;
    uint32_t z_texIndex;
    uint32_t visible_index;
    int index;

    bool operator==(const MyInstanceDataRender& other) const
    {
        return index == other.index;
    }
};

static MyInstanceDataRender MyPakedInstanceData(const InstanceData& _data)
{
    int16_t x = _data.pos.x, y = _data.pos.y, z = _data.pos.z;
    uint32_t packed_X_Y = ((uint32_t)(uint16_t)x << 16) | (uint32_t)(uint16_t)y;
    uint32_t packed_Z_texIndex = ((uint32_t)(uint16_t)z << 16) | (uint32_t)(uint16_t)_data.texIndex;
    uint32_t packed_visible_index = ((uint32_t)(uint16_t)_data.isVisible << 16) |
        (uint32_t)(uint16_t)_data.index;

    MyInstanceDataRender render = { packed_X_Y, packed_Z_texIndex, packed_visible_index, _data.index };
    return render;
}

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