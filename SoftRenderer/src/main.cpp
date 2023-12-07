#include "GpuLayer/GpuLayer.h"
#include <GLFW/glfw3.h>
#include <windows.h>

#define RGBA_COLOR(r, g, b, a) ((r) | (g << 8) | (b << 16) | (a << 24))

void OnResize(GLFWwindow* window, int width, int height);

FrameBuffer* framebuffer = nullptr;
uint32_t* Pixels = nullptr;

uint32_t WindowWidth = 800;
uint32_t WindowHeight = 600;

uint32_t ClearColor;
uint32_t Speed = 0.0f;

int main()
{
	srand(NULL);
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "soft", nullptr, nullptr);
	glfwSetWindowSizeCallback(window, OnResize);

	GpuLayer::Init(window);

	framebuffer = new FrameBuffer(WindowWidth, WindowHeight);

	Pixels = new uint32_t[WindowWidth * WindowHeight];

	int index = 0;
	ClearColor = RGBA_COLOR(255, 0, 255, 0);
	Speed = 1000;

	while (true)
	{
		glfwPollEvents();

		if (index < WindowWidth * WindowHeight)
		{
			for (size_t i = 0; i < Speed; i++)
			{
				Pixels[index++] = ClearColor;
			}
		}
		else
		{
			index = 0;
			ClearColor = RGBA_COLOR(rand() % 255, rand() % 255, rand() % 255, 0);
		}

		framebuffer->Present(Pixels);

		glfwSwapBuffers(window);
	}
}

void OnResize(GLFWwindow* window, int width, int height)
{
	framebuffer->Resize(width, height);
	delete[] Pixels;
	Pixels = new uint32_t[width * height];

	WindowWidth = width;
	WindowHeight = height;
}