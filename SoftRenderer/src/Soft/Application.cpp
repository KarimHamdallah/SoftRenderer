#include "Application.h"
#include <GLFW/glfw3.h>
#include "../GpuLayer/GpuLayer.h"

namespace Soft
{
	Application::Application(const AppSpecification & spec)
	{
		m_Instance = this;

		if (!glfwInit())
		{
			std::cout << "failed to init glfw\n";
			return;
		}

		GLFWwindow* window = glfwCreateWindow(spec.Width, spec.Height, spec.Title.c_str(), nullptr, nullptr);
		
		glfwSetWindowUserPointer(window, &m_Data);

		glfwSetWindowSizeCallback(window,
			[](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			std::cout << "window resized: " << width << "x" << height << "\n";

			if (data.m_OnResizeFuncPtr != nullptr)
				data.m_OnResizeFuncPtr(width, height);
		});

		GpuLayer::Init(window);

		m_Data.Width = spec.Width;
		m_Data.Height = spec.Height;
		m_Data.WindowHandel = window;
	}

	Application::~Application()
	{
		glfwDestroyWindow((GLFWwindow*)m_Data.WindowHandel);
		glfwTerminate();
	}

	bool Application::ShouldClose()
	{
		return glfwWindowShouldClose((GLFWwindow*)m_Data.WindowHandel);
	}

	void Application::Update()
	{
		glfwSwapBuffers((GLFWwindow*)m_Data.WindowHandel);
		glfwPollEvents();
	}
}
