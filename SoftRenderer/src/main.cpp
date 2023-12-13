#include "GpuLayer/GpuLayer.h"
#include <GLFW/glfw3.h>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define RGBA_COLOR(r, g, b, a) ((r) | (g << 8) | (b << 16) | (a << 24))


int WindowWidth = 800;
int WindowHeight = 600;

FrameBuffer* framebuffer = nullptr;
uint32_t* pixels = nullptr;

void OnResize(GLFWwindow* window, int width, int height);

void DrawCircle(uint32_t* pixels, int cx, int cy, int r, uint32_t color)
{
	float half_raduis = 0.5f * r;
	float topleft_x = cx - half_raduis;
	float topleft_y = cy - half_raduis;

	float bottomright_x = cx + half_raduis;
	float bottomright_y = cy + half_raduis;

	for (int py = topleft_y; py < bottomright_y; py++)
	{
		for (int px = topleft_x; px < bottomright_x; px++)
		{
			float x = px - cx;
			float y = py - cy;
			float dist = sqrtf(x * x + y * y);

			if (dist <= half_raduis)
				if (px > 0 && py > 0 && px < WindowWidth && py < WindowHeight)
					pixels[py * WindowWidth + px] = color;
		}
	}
}

bool mouse_on_circle(int cx, int cy, int r, double mousex, double mousey)
{
	float x = (float)mousex - (float)cx;
	float y = (float)mousey - (float)cy;
	float dist = sqrtf(x * x + y * y);

	std::cout << dist << "\n";

	if (dist <= (r * 0.5f))
		return true;

	return false;
}


struct Vertex
{
	float x, y, z, w = 0.0f;
};



int edgeFunction(Vertex a, Vertex b, Vertex c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
};

void DrawTriangle(uint32_t* pixels, Vertex A, Vertex B, Vertex C,
	uint32_t color)
{
	// Calculate the edge function for the whole triangle (ABC)
	int ABC = edgeFunction(A, B, C);


	// Initialise our Vertex
	Vertex P;

	// Get the bounding box of the triangle
	int minX = std::min(std::min(A.x, B.x), C.x);
	int minY = std::min(std::min(A.y, B.y), C.y);
	int maxX = std::max(std::max(A.x, B.x), C.x);
	int maxY = std::max(std::max(A.y, B.y), C.y);

	// Loop through all the pixels of the bounding box
	for (P.y = minY; P.y < maxY; P.y++)
	{
		for (P.x = minX; P.x < maxX; P.x++)
		{
			// Calculate our edge functions
			int ABP = edgeFunction(A, B, P);
			int BCP = edgeFunction(B, C, P);
			int CAP = edgeFunction(C, A, P);

			// Normalise the edge functions by dividing by the total area to get the barycentric coordinates
			float weightA = (float)BCP / ABC;
			float weightB = (float)CAP / ABC;
			float weightC = (float)ABP / ABC;

			// If all the edge functions are positive, the Vertex is inside the triangle
			if (weightA >= 0 && weightB >= 0 && weightC >= 0)
			{
				{
					// Draw the pixel
					if (P.x < WindowWidth && P.x > 0 && P.y < WindowHeight && P.y > 0)
					{
						pixels[(int)P.y * WindowWidth + (int)P.x] = color;
					}
				}
			}
		}
	}
}


void ConvertFromNDCToScreenCoordinates(Vertex& v)
{
	v.x = (v.x * WindowWidth * 0.5f) + (v.w * WindowWidth * 0.5f);
	v.y = (v.y * WindowHeight * -0.5f) + (v.w * WindowHeight * 0.5f);
}

void PerspectiveDivision(Vertex& v)
{
	v.x /= v.w;
	v.y /= v.w;
	v.z /= v.w;
}


int main()
{
	if (!glfwInit())
	{
		std::cout << "failed to init glfw\n";
		return 0;
	}

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "software renderer", nullptr, nullptr);
	
	glfwSetWindowSizeCallback(window, OnResize);
	GpuLayer::Init(window);

	framebuffer = new FrameBuffer(WindowWidth, WindowHeight);
	pixels = new uint32_t[WindowWidth * WindowHeight];


	while (!glfwWindowShouldClose(window))
	{
		// events
		glfwPollEvents();

		// clear screen
		for (size_t i = 0; i < WindowWidth * WindowHeight; i++)
			pixels[i] = 0xFF000000;


		// vertices in NDC
		Vertex A{ -0.5f, -0.5f, 0.0f, 1.0f };
		Vertex B{ 0.0f,  0.5f,   0.0f, 1.0f };
		Vertex C{ 0.5f, -0.5f,  0.0f, 1.0f };


		glm::vec4 A_transformed = { A.x, A.y, A.z, A.w };
		glm::vec4 B_transformed = { B.x, B.y, B.z, B.w };
		glm::vec4 C_transformed = { C.x, C.y, C.z, C.w };

		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WindowWidth / (float)WindowHeight,
			0.01f, 1000.0f);

		glm::mat4 model =
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f))
			* glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		A_transformed = proj * model * A_transformed;
		B_transformed = proj * model * B_transformed;
		C_transformed = proj * model * C_transformed;

		A = { A_transformed.x, A_transformed.y, A_transformed.z, A_transformed.w };
		B = { B_transformed.x, B_transformed.y, B_transformed.z, B_transformed.w };
		C = { C_transformed.x, C_transformed.y, C_transformed.z, C_transformed.w };

		// Verices in Screen space
		ConvertFromNDCToScreenCoordinates(A);
		ConvertFromNDCToScreenCoordinates(B);
		ConvertFromNDCToScreenCoordinates(C);


		PerspectiveDivision(A);
		PerspectiveDivision(B);
		PerspectiveDivision(C);


		DrawTriangle(pixels, A, B, C, 0xFFFF0000);


		framebuffer->Present(pixels);

		// swap buffers
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


void OnResize(GLFWwindow* window, int width, int height)
{
	WindowWidth = width;
	WindowHeight = height;

	std::cout << "window resized: " << width << "x" << height << "\n";
	
	framebuffer->Resize(width, height);
	delete[] pixels;
	pixels = new uint32_t[WindowWidth * WindowHeight];
}
