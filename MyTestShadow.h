#pragma once

#include "gl/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "myShader.h"

class MyTestShadow
{
private:
    // Framebuffer для depth-only рендера
    GLuint depthFBO;
    // Текстура, в которую будем писать глубину
    GLuint depthMap;
    // Итоговая матрица
    glm::mat4 lightSpaceMatrix;
    //шейдер глубины
    MyShader depthShader;
public:
    // Размер карты теней (чем больше — тем чётче тени)
    const unsigned int SHADOW_WIDTH = 2048;
    const unsigned int SHADOW_HEIGHT = 2048;

    void myUpdateMatrixLight(const MyCamera& _camera)
    {
        // Позиция directional света (как солнце)
        glm::vec3 lightPos(_camera.myGetPos());

        // Ортографическая проекция (для directional light)
        glm::mat4 lightProjection = glm::ortho(-20.f, 20.f, -20.f, 20.f, 0.1f, 50.f);

        // Камера света
        glm::mat4 lightView = glm::lookAt(lightPos + glm::vec3(-10, 40, -10), lightPos,
            glm::vec3(0, 1, 0));

        // Итоговая матрица
        lightSpaceMatrix = lightProjection * lightView;

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

        glClear(GL_DEPTH_BUFFER_BIT);

        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }
    void myActivateShadowTexture(MyShader &_shaderInstance)
    {
        // shadow map
        _shaderInstance.use();
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        _shaderInstance.setInt("shadowMap", 2);
    }

    glm::mat4 myGetSpaceMatrix() const { return lightSpaceMatrix; }

	void myInitialize()
	{
        // Framebuffer для depth-only рендера
        depthFBO;
        glGenFramebuffers(1, &depthFBO);

        // Текстура, в которую будем писать глубину
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        // Создаём пустую depth-текстуру
        glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
            GL_DEPTH_COMPONENT,GL_FLOAT, nullptr /*данных нет*/);

        // Фильтрация — без размытия (важно для depth)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Если координаты выходят за [0,1] — считаем, что света нет
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // Цвет границы = максимальная глубина
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // Привязываем текстуру к framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

        // Нам не нужен цвет
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // Возвращаемся к default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        depthShader = MyShader("shader/depthShaderV.txt", "shader/depthShaderF.txt");
	}
};