#pragma once

#include "gl/glew.h"
#include "glm/glm.hpp"

struct MyVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

class MyPrimitiveCube
{
private:
	GLuint VAO, EBO, VBO;

public:
    ~MyPrimitiveCube()
    {
        if (EBO) glDeleteBuffers(1, &EBO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (VAO) glDeleteVertexArrays(1, &VAO);

        EBO = VBO = VAO = 0;

        std::cout << "delete PrimitiveCube" << std::endl;
    }
    void myInitialize()
    {
        float half = 0.5f;
        MyVertex vertices[] =
        {
            // +X (right)
            {{ 0.5f, -0.5f, -0.5f}, { 1, 0, 0}, {0, 1}},
            {{ 0.5f,  0.5f, -0.5f}, { 1, 0, 0}, {0, half}},
            {{ 0.5f,  0.5f,  0.5f}, { 1, 0, 0}, {half, half}},
            {{ 0.5f, -0.5f,  0.5f}, { 1, 0, 0}, {half, 1}},

            // -X (left)
            {{-0.5f, -0.5f,  0.5f}, {-1, 0, 0}, {0, 1}},
            {{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {0, half}},
            {{-0.5f,  0.5f, -0.5f}, {-1, 0, 0}, {half, half}},
            {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {half, 1}},

            // +Y (top)
            {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {0, half}},
            {{-0.5f,  0.5f,  0.5f}, {0, 1, 0}, {0, 0}},
            {{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {half, 0}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}, {half, half}},

            // -Y (bottom)
            {{-0.5f, -0.5f,  0.5f}, {0,-1, 0}, {half, 1}},
            {{-0.5f, -0.5f, -0.5f}, {0,-1, 0}, {half, half}},
            {{ 0.5f, -0.5f, -0.5f}, {0,-1, 0}, {1, half}},
            {{ 0.5f, -0.5f,  0.5f}, {0,-1, 0}, {1, 1}},

            // +Z (front)
            {{-0.5f, -0.5f,  0.5f}, {0, 0, 1}, {half, 1}},
            {{ 0.5f, -0.5f,  0.5f}, {0, 0, 1}, {0, 1}},
            {{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {0, half}},
            {{-0.5f,  0.5f,  0.5f}, {0, 0, 1}, {half, half}},

            // -Z (back)
            {{ 0.5f, -0.5f, -0.5f}, {0, 0,-1}, {half, 1}},
            {{-0.5f, -0.5f, -0.5f}, {0, 0,-1}, {0, 1}},
            {{-0.5f,  0.5f, -0.5f}, {0, 0,-1}, {0, half}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 0,-1}, {half, half}},
        };

        GLuint indices[] =
        {
             0,  1,  2,   0,  2,  3,   // +X
             4,  5,  6,   4,  6,  7,   // -X
             8,  9, 10,   8, 10, 11,   // +Y
            12, 13, 14,  12, 14, 15,   // -Y
            16, 17, 18,  16, 18, 19,   // +Z
            20, 21, 22,  20, 22, 23    // -Z
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)0);
        glEnableVertexAttribArray(0);

        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex),
            (void*)offsetof(MyVertex, normal));
        glEnableVertexAttribArray(1);

        // uv
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex),
            (void*)offsetof(MyVertex, uv));
        glEnableVertexAttribArray(2);
    }

    inline GLuint myGetVAO() const { return VAO; }
};