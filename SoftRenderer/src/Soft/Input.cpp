#include "Input.h"
#include <GLFW/glfw3.h>
#include "Application.h"

namespace Soft
{
	bool Input::IsKeyPressed(Key Key)
	{
		int KeyState = glfwGetKey
		(
			(GLFWwindow*)Application::Get().GetNativeWindow(),
			(int)Key
		);
		return KeyState == GLFW_PRESS || KeyState == GLFW_REPEAT;
	}
	bool Input::IsKeyReleased(Key Key)
	{
		int KeyState = glfwGetKey
		(
			(GLFWwindow*)Application::Get().GetNativeWindow(),
			(int)Key
		);
		return KeyState == GLFW_RELEASE;
	}
	bool Input::IsKeyDown(Key Key)
	{
		int KeyState = glfwGetKey
		(
			(GLFWwindow*)Application::Get().GetNativeWindow(),
			(int)Key
		);
		return KeyState == GLFW_PRESS && KeyState != GLFW_REPEAT;
	}
	bool Input::IsMouseButtonPressed(MouseButton Button)
	{
		int ButtonState = glfwGetMouseButton
		(
			(GLFWwindow*)Application::Get().GetNativeWindow(),
			(int)Button
		);

		return ButtonState == GLFW_PRESS || ButtonState == GLFW_REPEAT;
	}
	bool Input::IsMouseButtonReleased(MouseButton Button)
	{
		int ButtonState = glfwGetMouseButton
		(
			(GLFWwindow*)Application::Get().GetNativeWindow(),
			(int)Button
		);

		return ButtonState == GLFW_RELEASE;
	}
	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return (float)x;
	}
	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return (float)y;
	}
	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return std::pair<float, float>((float)x, float(y));
	}
}
