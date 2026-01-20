#pragma once
#include <cstdint>
#include "MyShader.h"
#include "MyTestInstace.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_access.hpp"

struct DrawElementsIndirectCommand 
{
    uint32_t count;         // Кол-во индексов (36 для куба)
    uint32_t instanceCount; // Сюда Compute Shader запишет кол-во видимых
    uint32_t firstIndex;    // 0
    uint32_t baseVertex;    // 0
    uint32_t baseInstance;  // 0
};

struct SortData 
{
    float distance; // Ключ, по которому сортируем
    uint32_t index; // Порядковый номер куба в массиве матриц
    float padding[2];   // 8 байт (добиваем до 16)
};

class MyGPUCulling
{
private:
    MyShader shaderCulling;
    MyShader shaderPreSort;
    MyShader shaderSort;
    DrawElementsIndirectCommand cmd;
public:
    GLuint sortSSBO, inputSSBO, outputSSBO, commandBuffer, totalCount;
    MyGPUCulling() { }

    MyGPUCulling(const std::string& _name)
    {
        shaderCulling = MyShader(_name.c_str());
        //----------
        shaderPreSort = MyShader("shader/preSortCube.comp");
        //----------
        shaderSort = MyShader("shader/sort.comp");
    }
    ~MyGPUCulling()
    {
        if (inputSSBO) glDeleteBuffers(1, &inputSSBO);
        if (outputSSBO) glDeleteBuffers(1, &outputSSBO);
        if (commandBuffer) glDeleteBuffers(1, &commandBuffer);
    }

    void mySortCubes(const GLuint _count)
    {
        glGenBuffers(1, &sortSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, sortSSBO);
        // Резервируем память под максимальное количество кубов
        glBufferData(GL_SHADER_STORAGE_BUFFER, _count * sizeof(SortData), nullptr, GL_DYNAMIC_COPY);
    }

    void myInitialize(const std::vector<InstanceData>& _listInstance)
    {
        // 1. Буфер со всеми данными (заполняется один раз или при изменениях)
        glGenBuffers(1, &inputSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, _listInstance.size() * sizeof(InstanceData),
            _listInstance.data(), GL_STATIC_DRAW);

        // 2. Пустой буфер для видимых данных
        glGenBuffers(1, &outputSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, _listInstance.size() * sizeof(InstanceData),
            nullptr, GL_STREAM_DRAW);

        // 3. Буфер команды отрисовки
        cmd = { 36, 0, 0, 0, 0 };
        glGenBuffers(1, &commandBuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(cmd), &cmd, GL_DYNAMIC_DRAW);

        totalCount = _listInstance.size();
        mySortCubes(totalCount);
    }

    void myRenderStart(const MyCamera& _camera)
    {
        // --- ШАГ 1: Обнуление счетчика видимых объектов ---
        uint32_t zero = 0;
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
        // Сдвиг на 4 байта — это поле instanceCount в структуре
        glBufferSubData(GL_DRAW_INDIRECT_BUFFER, sizeof(uint32_t), sizeof(uint32_t), &zero);
        // --- ШАГ 2: Запуск вычислений (Culling) ---
        shaderCulling.use();
        shaderCulling.setMat4("viewProj", _camera.myGetPerspective() * _camera.myGetViewMatrix());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, commandBuffer);

        glDispatchCompute((totalCount + 63) / 64, 1, 1);

        // Ждем завершения записи в буферы
        glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        glFinish(); // Заставит CPU ждать завершения GPU
        //----------------------------------------------------------------------
        // 2. Считаем расстояния (Pre-Sort)
        shaderPreSort.use();
        shaderPreSort.setVec3("cameraPos", _camera.myGetPos());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sortSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, commandBuffer);

        glDispatchCompute((totalCount + 63) / 64, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // 3. Запускаем сортировку (Bitonic Sort)
        shaderSort.use();
        // Алгоритм требует, чтобы размер массива был степенью двойки (например, 1024, 2048)
        int nextPow2 = pow(2, ceil(log2(totalCount)));
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sortSSBO);

        for (int stage = 2; stage <= nextPow2; stage <<= 1) 
        {
            for (int step = stage >> 1; step > 0; step >>= 1) 
            {
                shaderSort.setInt("pStep", step);
                shaderSort.setInt("pStage", stage);
                glDispatchCompute(nextPow2 / 512 + 1, 1, 1);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); // Ждем завершения обмена данными
            }
        }
    }

    void myRenderEnd()
    {
        // Привязываем буфер с результатами к аттрибутам инстансинга
        // Теперь отрисовка будет брать матрицы из outputSSBO
        glBindBuffer(GL_ARRAY_BUFFER, outputSSBO);
        // ... тут стандартная настройка glVertexAttribPointer для mat4 (location 3) ...

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
        glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);

        // ВНИМАНИЕ: Это замедляет программу, используйте только для теста!
        //uint32_t resultCount = 0;
        //glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
        //glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, sizeof(uint32_t), sizeof(uint32_t), &resultCount);
        //std::cout << "Видимых кубов: " << resultCount << std::endl;
    }
};