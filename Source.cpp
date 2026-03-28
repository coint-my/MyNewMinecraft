#include "iostream"

#define GLEW_STATIC
#pragma comment(lib, "gl_lib/opengl32.lib")
#pragma comment(lib, "gl_lib/glew32s.lib")
#pragma comment(lib, "gl_lib/glfw3.lib")

#include "gl/glew.h"
#include "gl/glfw3.h"
#include "MyCameraFly.h"
#include "MyShader.h"
#include "MyPrimitiveCube.h"
#include "MyTestInstace.h"
#include "MyTestShadow.h"
#include "MyOutlineShader.h"
#include "MyScreen.h"
#include "MyTestFirstPerson.h"
#include "MyGPUCulling.h"

//ширина и высота окна
int MyScrren::WID = 800;
int MyScrren::HEI = 600;
//окно OpenGL
GLFWwindow* window;
//камера полета
//MyCameraFly cameraFly(0.0f, 0.0f, -1.0f);
//fps
MyTestFirstPerson firstPerson(glm::vec3(0, 48, 0));
//шейдер
MyShader shaderSimple;
//MyShader shaderInstance;
//тестовый куб
MyPrimitiveCube testCube;
//тестовый инстанс
MyTestInstance testInstance;
//тест тени
MyTestShadow shadow;
//outline class
MyOutlineShader outline;
//оптимизация отрисовки кубов
//MyGPUCulling myCulling;
//источники света
//тест direct instance
//MyDirectInstansing dInstance;
MyShader renderShader;

GLuint VAO, VBO;

float testVertex[] =
{
	-0.5f, -0.5f, 0,
	 0.0f,  0.5f, 0,
	 0.5f, -0.5f, 0
};

void myGenerateVao()
{
	const char* vertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
	
	out vec3 pos;
	
	void main()
	{
		gl_Position = projection * view * model * vec4(aPos, 1.0);
		pos = aPos;
	}
	)";

	const char* fragmentShaderSource = R"(
	#version 330 core
	out vec4 FragColor;
	in vec3 pos;

	void main()
	{
		FragColor = vec4(pos.xyz, 1.0);
	}
	)";
	//----------------------------------------
	shaderSimple = MyShader("shader/selfShaderV.txt", "shader/selfShaderF.txt");
	//shaderInstance = MyShader("shader/instanceShaderV.txt", "shader/simpleShaderF.txt");
	//renderShader = MyShader("shader/directInstanceV.txt", "shader/simpleShaderF.txt");
	renderShader = MyShader("shader/directInstanceDataV.txt", "shader/simpleShaderF.txt");
	//----------------------------------------
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(testVertex), testVertex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	testCube.myInitialize();
	testInstance.myInitialize(window, firstPerson);
	shadow.myInitialize();
	outline.myInitialize(testCube.myGetVAO());

	glEnable(GL_DEPTH_TEST);
}

void myEventKey(GLFWwindow* _window, int _key, int _scancode, int _action, int _mode)
{
	if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (_key == GLFW_KEY_Q && _action == GLFW_RELEASE)
	{
		if (testInstance.rayCastCub)
		{
			testInstance.rayCastCub->first.isVisible = false;

			testInstance.myDeleteCub(testInstance.rayCastCub->first,
				testInstance.rayCastCub->second);
		}
	}
	else if (_key == GLFW_KEY_E && _action == GLFW_RELEASE)
	{
		if (testInstance.rayCastCubAdd)
		{
			testInstance.rayCastCubAdd->first.isVisible = true;
			
			testInstance.myAddCube(testInstance.rayCastCubAdd->first,
				testInstance.rayCastCubAdd->second);
		}
	}

	firstPerson.myEventKey(_key, _scancode, _action, _mode);
}

void myEventMouseButton(GLFWwindow* _window, int _button, int _action, int _mods)
{
	firstPerson.myEventMouseButton(_window, _button, _action, _mods);
}

void myEventMouseMove(GLFWwindow* _window, double _xpos, double _ypos)
{
	//cameraFly.myEventMouseMove(_window, _xpos, _ypos);
	firstPerson.myEventMouseMove(_xpos, _ypos);
}

void myEventMouseScroll(GLFWwindow* _window, double _xoffset, double _yoffset)
{
	//cameraFly.myEventMouseScroll(_window, _xoffset, _yoffset);
}

void myRender()
{
	//----------------test shadow
	shadow.myUpdateMatrixLight(firstPerson.camFps);
	testInstance.myRenderSectorFromShadow(firstPerson.camFps.myGetPos());
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	//----------------test shadow

	glViewport(0, 0, MyScrren::WID, MyScrren::HEI);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//cameraFly.myUpdateCamera();
	firstPerson.myUpdateCamera();

	// Create transformations
	glm::mat4 view = firstPerson.camFps.myGetViewMatrix();//cameraFly.myGetViewMatrix();
	glm::mat4 projection = firstPerson.camFps.myGetPerspective();//cameraFly.myGetPerspective();

	//---------shadow
	shadow.myActivateShadowTexture(renderShader);
	//---------shadow

	//---------test Culling
	//myCulling.myRenderStart(firstPerson.camFps);
	//---------end Culling

	//---------Draw Instance

	testInstance.myBindTexture(renderShader);

	// GL_CW (Clockwise):
	glFrontFace(GL_CCW);

	// 3.
	glCullFace(GL_BACK);

	//test direct instance
	renderShader.use();
	renderShader.setMat4("view", view);
	renderShader.setMat4("projection", projection);
	//renderShader.setVec3("camPos", firstPerson.camFps.myGetPos());
	renderShader.setMat4("lightSpaceMatrix", shadow.myGetSpaceMatrix());

	renderShader.setVec3("sunDir", shadow.lightDir); // Светит сверху вниз
	renderShader.setVec3("sunColor", glm::vec3(1.0f, 0.98f, 0.9f));
	renderShader.setVec3("ambient", glm::vec3(0.15f, 0.15f, 0.15f)); // Синеватый оттенок неба

	testInstance.myRenderer(projection * view);
	//---------End Draw Instance

	//---------test end stencil
	if (testInstance.rayCastCub)
	{
		outline.myUpdateMatrix(/*testInstance.rayCastCub->first.model*/
			glm::translate(glm::mat4(1.0f), glm::vec3(testInstance.rayCastCub->first.pos)));

		outline.myRenderOutline(firstPerson.camFps);
	}
	//---------Draw cube crosshair
	shaderSimple.use();

	glBindVertexArray(testCube.myGetVAO());
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void myRefreshViewport(GLFWwindow* _window, int _width, int _height)
{
	glViewport(0, 0, _width, _height);

	MyScrren::WID = _width;
	MyScrren::HEI = _height;
}

void myUpdate()
{
	testInstance.myUpdate(firstPerson, window);
}

int main()
{
	//Инициализация GLFW
	glfwInit();
	//Задается минимальная требуемая версия OpenGL. 
	//Мажорная 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Установка профайла для которого создается контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Выключение возможности изменения размера окна
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	//буфер трафарета
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	//создание окна
	window = glfwCreateWindow(MyScrren::WID, MyScrren::HEI, "OpenGL test Minecraft", nullptr, nullptr);
	//место создания окна
	glfwSetWindowPos(window, 300, 100);

	if (window == nullptr)
	{
		std::cout << "Не удалось создать окно opengl" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Не удалось инициализировать GLEW" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glfwSetKeyCallback(window, myEventKey);
	glfwSetCursorPosCallback(window, myEventMouseMove);
	glfwSetMouseButtonCallback(window, myEventMouseButton);
	glfwSetScrollCallback(window, myEventMouseScroll);

	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, myRefreshViewport);

	myGenerateVao();
	glEnable(GL_BLEND);
	// 1. Включаем отсечение задних граней (опционально, но часто используется)
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(1); // Включить вертикальную синхронизацию

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		myUpdate();

		myRender();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}