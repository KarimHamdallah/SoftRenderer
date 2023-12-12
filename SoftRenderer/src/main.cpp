#if 1
#include "GpuLayer/GpuLayer.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"

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
	Vertex(float x, float y, float z, float w, float u, float v) : x(x), y(y), z(z), w(w), u(u), v(v) {}
	float x, y, z, w = 0.0f;
	float u, v = 0.0f;
};


int edgeFunction(Vertex a, Vertex b, Vertex c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
};

void DrawTriangle(uint32_t* pixels, Vertex A, Vertex B, Vertex C, uint32_t color,
	const Texture2D& Image, bool draw_back_face = true)
{
	// Calculate the edge function for the whole triangle (ABC)
	int ABC = edgeFunction(A, B, C);

	// Our nifty trick: Don't bother drawing the triangle if it's back facing
	bool is_back_face = ABC < 0;
	if (!draw_back_face && is_back_face)
		return;


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
				// Perform perspective correction if necessary
				// (assuming homogeneous coordinates, divide by w)
				// Perspective correction
				float w = weightA / A.w + weightB / B.w + weightC / C.w;
				weightA /= w;
				weightB /= w;
				weightC /= w;

				// Interpolate texture coordinates with perspective correction
				float u = weightA * A.u / A.w + weightB * B.u / B.w + weightC * C.u / C.w;
				float v = weightA * A.v / A.w + weightB * B.v / B.w + weightC * C.v / C.w;

				// Sample the texture using the interpolated texture coordinates
				int textureX = static_cast<int>(u * Image.Width) % Image.Width;
				int textureY = static_cast<int>(v * Image.Height) % Image.Height;

				// Interpolate the colours at point P
				int r = 255 * weightA + 0 * weightB + 0 * weightC;
				int g = 0 * weightA + 255 * weightB + 0 * weightC;
				int b = 0 * weightA + 0 * weightB + 255 * weightC;
				
				color = RGBA_COLOR(r, g, b, 255);


				{
					// Draw the pixel
					if (P.x < WindowWidth && P.x > 0 && P.y < WindowHeight && P.y > 0)
					{
						uint32_t textureColor = Image.pixels[textureY * Image.Width + textureX];
						pixels[(int)P.y * WindowWidth + (int)P.x] = textureColor;
					}
				}
			}
		}
	}
}

void FromNDCToScreenSpace(Vertex& v)
{
	v.x = (v.x * WindowWidth * 0.5f) + (v.w * WindowWidth * 0.5f);
	v.y = (v.y * WindowHeight * -0.5f) + (v.w * WindowHeight * 0.5f);
}

Vertex vec4tovertex(const glm::vec4& v)
{
	return Vertex(v.x, v.y, v.z, v.w);
}

void PerspectiveDevide(Vertex& v)
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

	// start
	Texture2D image = Texture2D("res/p.jpg", true);


	while (!glfwWindowShouldClose(window))
	{
		// events
		glfwPollEvents();

		// clear screen
		for (size_t i = 0; i < WindowWidth * WindowHeight; i++)
			pixels[i] = 0xFF000000;


		// vertex input
		Vertex A(-0.5, -0.5f, 0.0f, 1.0f);
		Vertex B(0.0f, 0.5f, 0.0f, 1.0f);
		Vertex C(0.5f, -0.5f, 0.0f, 1.0f);

		A.u = 0.0f; 		A.v = 0.0f;
		B.u = 0.5f; 		B.v = 1.0f;
		C.u = 1.0f; 		C.v = 0.0f;
		
		{ // vertex shader
			glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WindowWidth / (float)WindowHeight,
				0.01f, 1000.0f);

			glm::mat4 model =
				glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f))
				* glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			
			glm::vec4 A_transf = { A.x, A.y, A.z, A.w };
			glm::vec4 B_transf = { B.x, B.y, B.z, B.w };
			glm::vec4 C_transf = { C.x, C.y, C.z, C.w };

			A_transf = proj * model * A_transf;
			B_transf = proj * model * B_transf;
			C_transf = proj * model * C_transf;

			A = { A_transf.x, A_transf.y, A_transf.z, A_transf.w, A.u, A.v };
			B = { B_transf.x, B_transf.y, B_transf.z, B_transf.w, B.u, B.v };
			C = { C_transf.x, C_transf.y, C_transf.z, C_transf.w, C.u, C.v };

			FromNDCToScreenSpace(A);
			FromNDCToScreenSpace(B);
			FromNDCToScreenSpace(C);

			PerspectiveDevide(A);
			PerspectiveDevide(B);
			PerspectiveDevide(C);
		}
		// rasterization
		DrawTriangle(pixels, A, B, C, 0xFFFF0000, image);


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

#else


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GpuLayer/stb_image.h"

#include "GpuLayer/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("res/vs.vert", "res/fs.frag");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = 
	{
		// positions            // texture coords
		-0.5f, -0.5f, 0.0f,      0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,      1.0f, 0.0f,
		 0.0f, 0.5f, 0.0f,       0.5f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 2, // first triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("res/p.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
	// either set it manually like so:
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);



	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		// render container
		ourShader.use();

		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT,
			0.01f, 1000.0f);

		glm::mat4 model =
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f))
			* glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "mp"), 1, GL_FALSE, &(proj * model)[0][0]);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
#endif