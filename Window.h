#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow*, int, int);
void mouse_callback(GLFWwindow*, double, double);
void key_callback(GLFWwindow*, int, int, int, int); 
void scroll_callback(GLFWwindow*, double, double);


class Window
{
	GLFWwindow* window;
	int width;
	int height;
	const GLFWvidmode* mode;
	bool fullscreen = true;
public:
	//Инициализация
	Window(const int& width, const int& height, const char* name)
	{
		// glfw: инициализация и конфигурирование
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfw: создание окна
		Set_Size(width, height);
		window = glfwCreateWindow(width, height, name, NULL, NULL);
		if (window == NULL)
		{
			std::cout << ("ERROR::CreateWindow");
			exit(-1);
		}
		mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		Set_FullScreen();
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetKeyCallback(window, key_callback);
		glfwSetScrollCallback(window, scroll_callback);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << ("ERROR::Loadglad");
			exit(-1);
		}
	}
	//Деструктор
	~Window()
	{
		glfwTerminate();
	}
	//Переключение в полноэкранный режим
	void Set_FullScreen()
	{
		if (fullscreen)
		{
			glfwSetWindowMonitor(window, NULL, (mode->width - width) / 2, (mode->height - height) / 2, width, height, mode->refreshRate);
			fullscreen = false;
		}
		else
		{
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
			fullscreen = true;
		}
	}
	//Изменение размеров окна
	void Set_Size(const int& width, const int& height)
	{
		this->width = width;
		this->height = height;
	}
	//Возвращает ширину
	const int& Get_Width()const
	{
		return width;
	}
	//Возвращает высоту
	const int& Get_Height()const
	{
		return height;
	}
	//Возвращает дескриптор
	GLFWwindow* Get_Window() const
	{
		return window;
	}
};

