#include "GpuLayer/GpuLayer.h"
#include <GLFW/glfw3.h>
#include <algorithm>

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
	Vertex() = default;
	Vertex(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	float x, y, z, w = 0.0f;
};

// Function to compute barycentric coordinates
void BarycentricCoordinates(Vertex v0, Vertex v1, Vertex v2, float x, float y, float& alpha, float& beta, float& gamma)
{
	float detT = (v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y);
	alpha = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) / detT;
	beta = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) / detT;
	gamma = 1.0f - alpha - beta;
}

// Function to rasterize a filled triangle using barycentric coordinates
void DrawTriangle(uint32_t* pixels, Vertex v0, Vertex v1, Vertex v2, uint32_t color)
{
	// Bounding box of the triangle
	int minX = std::max(0, (int)ceil(std::min(v0.x, std::min(v1.x, v2.x))));
	int maxX = std::min(WindowWidth - 1, (int)floor(std::max(v0.x, std::max(v1.x, v2.x))));
	int minY = std::max(0, (int)ceil(std::min(v0.y, std::min(v1.y, v2.y))));
	int maxY = std::min(WindowHeight - 1, (int)floor(std::max(v0.y, std::max(v1.y, v2.y))));

	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			float alpha, beta, gamma;
			BarycentricCoordinates(v0, v1, v2, x + 0.5f, y + 0.5f, alpha, beta, gamma);

			if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f)
			{
				/*
				// Check depth to perform depth test
				// Interpolate depth
				float depth = alpha * v0.z + beta * v1.z + gamma * v2.z;
				if (depth < framebuffer.depthBuffer[y * framebuffer.width + x]) {
					// Interpolate color
					Color color;
					color.r = (int)(alpha * v0.color.r + beta * v1.color.r + gamma * v2.color.r);
					color.g = (int)(alpha * v0.color.g + beta * v1.color.g + gamma * v2.color.g);
					color.b = (int)(alpha * v0.color.b + beta * v1.color.b + gamma * v2.color.b);

					// Write to framebuffer
					framebuffer.SetPixel(x, y, depth, color);
				}
				*/

				// Write to framebuffer
				pixels[y * WindowWidth + x] = color;
			}
		}
	}
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

	// start

	while (!glfwWindowShouldClose(window))
	{
		// events
		glfwPollEvents();

		// clear screen
		for (size_t i = 0; i < WindowWidth * WindowHeight; i++)
			pixels[i] = RGBA_COLOR(0, 0, 0, 0); // black color


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
