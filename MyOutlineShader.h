#pragma once

#include "gl/glew.h"
#include "glm/glm.hpp"
#include "MyPrimitiveCube.h"
#include "MyShader.h"

class MyOutlineShader
{
private:
	//MyPrimitiveCube cub;
	GLuint outlineVAO;
	GLuint outlineInstanceVBO;
	MyShader outlineShader;
public:
	void myInitCube(GLuint& VBO, GLuint& EBO)
	{
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		MyVertex vertices[] =
		{
			// +X (right)
			{{ 0.5f, -0.5f, -0.5f}, { 1, 0, 0}, {0, 0}},
			{{ 0.5f,  0.5f, -0.5f}, { 1, 0, 0}, {1, 0}},
			{{ 0.5f,  0.5f,  0.5f}, { 1, 0, 0}, {1, 1}},
			{{ 0.5f, -0.5f,  0.5f}, { 1, 0, 0}, {0, 1}},

			// -X (left)
			{{-0.5f, -0.5f,  0.5f}, {-1, 0, 0}, {0, 0}},
			{{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {1, 0}},
			{{-0.5f,  0.5f, -0.5f}, {-1, 0, 0}, {1, 1}},
			{{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 1}},

			// +Y (top)
			{{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {0, 0}},
			{{-0.5f,  0.5f,  0.5f}, {0, 1, 0}, {0, 1}},
			{{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 1}},
			{{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 0}},

			// -Y (bottom)
			{{-0.5f, -0.5f,  0.5f}, {0,-1, 0}, {0, 0}},
			{{-0.5f, -0.5f, -0.5f}, {0,-1, 0}, {0, 1}},
			{{ 0.5f, -0.5f, -0.5f}, {0,-1, 0}, {1, 1}},
			{{ 0.5f, -0.5f,  0.5f}, {0,-1, 0}, {1, 0}},

			// +Z (front)
			{{-0.5f, -0.5f,  0.5f}, {0, 0, 1}, {0, 0}},
			{{ 0.5f, -0.5f,  0.5f}, {0, 0, 1}, {1, 0}},
			{{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1, 1}},
			{{-0.5f,  0.5f,  0.5f}, {0, 0, 1}, {0, 1}},

			// -Z (back)
			{{ 0.5f, -0.5f, -0.5f}, {0, 0,-1}, {0, 0}},
			{{-0.5f, -0.5f, -0.5f}, {0, 0,-1}, {1, 0}},
			{{-0.5f,  0.5f, -0.5f}, {0, 0,-1}, {1, 1}},
			{{ 0.5f,  0.5f, -0.5f}, {0, 0,-1}, {0, 1}},
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

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)0);
		glEnableVertexAttribArray(0);
	}
	void myInitialize(GLuint _cubeVAO)
	{
		outlineShader = MyShader("shader/outlineShaderV.txt", "shader/outlineShaderF.txt");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);

		//glBindVertexArray(_cubeVAO);
		glGenVertexArrays(1, &outlineVAO);
		glBindVertexArray(outlineVAO);

		//cub.myInitialize();
		GLuint VBO, EBO;
		myInitCube(VBO, EBO);

		// instance mat4 (ОДНА)
		glGenBuffers(1, &outlineInstanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineInstanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

		// атрибуты mat4 (location = 3..6)
		for (int i = 0; i < 4; i++)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE,
				sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
			glVertexAttribDivisor(3 + i, 1);
		}
	}

	void myRenderOutline(const MyCamera& _camera)
	{
		// Переключаем режим на линии
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(2.0f); // Установить толщину в 2 пикселя

		glBindVertexArray(outlineVAO);
		
		outlineShader.use();

		outlineShader.setMat4("view", _camera.myGetViewMatrix());
		outlineShader.setMat4("projection", _camera.myGetPerspective());
		outlineShader.setFloat("outlineScale", 1.01f);
		outlineShader.setFloat("thickness", 0.001f);
		outlineShader.setVec3("outlineColor", glm::vec3(1.0, 0.6, 0.1));


		glDisable(GL_DEPTH_TEST);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 1);
		glEnable(GL_DEPTH_TEST);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void myUpdateMatrix(const glm::mat4& _model)
	{
		glBindBuffer(GL_ARRAY_BUFFER, outlineInstanceVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), &_model);
	}
};