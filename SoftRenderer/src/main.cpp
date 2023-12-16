#if 1
#include "Soft/Application.h"
#include "Soft/Renderer.h"
#include "Soft/Input.h"
#include "Soft/mesh.h"
#include <GLFW/glfw3.h>

void OnResize(uint32_t width, uint32_t height);

void FromNDCToScreenSpace(vertex& v)
{
	const float zInv = 1.0f / v.z;
	v.x = (v.x * zInv + 1.0f) * Soft::Renderer::GetWidth() * 0.5f;
	v.y = (-v.y * zInv + 1.0f) * Soft::Renderer::GetHeight() * 0.5f;
}

int main()
{
	Soft::AppSpecification AppSpec{ 800, 800, "soft" };
	Soft::Application* App = new Soft::Application(AppSpec);
	App->SetOnResizeCallBack(OnResize);
	
	Soft::Renderer::Init(800, 800);

	IndexedTriangleList CubeTriangleList = Factory3D::GetIndexedTriangleCube(1.0f);
	float thetaX = 0.0f;
	float thetaY = 0.0f;
	float thetaZ = 0.0f;

	uint32_t colors[] =
	{
		0xFF0000FF, // Red
		0xFF00FF00, // Green
		0xFFFF0000, // Blue
		0xFF00FFFF, // Yellow
		0xFFFF00FF, // Magenta
		0xFF00FFFF, // Cyan
		0xFF800080, // Purple
		0xFFFFA500, // Orange
		0xFF008000, // Dark Green
		0xFFA52A2A, // Brown
		0xFFFFC0CB, // Pink
		0xFFFFA500 // Orange
	};

	std::shared_ptr<Soft::Mesh> mesh = std::make_shared<Soft::Mesh>("res/cube.fbx");
	std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>("res/wood.jpg");

	while (!App->ShouldClose())
	{
		Soft::Renderer::ClearScreen(0xFF000000);

		
		// Handle keyboard input
		if (Soft::Input::IsKeyPressed(Soft::Key::Q))
			thetaX++;
		if (Soft::Input::IsKeyPressed(Soft::Key::W))
			thetaY++;
		if (Soft::Input::IsKeyPressed(Soft::Key::E))
			thetaZ++;



		// transform vertices
		std::vector<vertex> vertices = mesh->GetVertices();
		std::vector<uint32_t> indices = mesh->GetIndices();
		for (auto& v : vertices)
		{
			mat3 rot =
				mat3::GetRotationX(thetaX * ToRad) *
				mat3::GetRotationY(thetaY * ToRad) *
				mat3::GetRotationZ(thetaZ * ToRad);

			vec3 transf_v = { v.x, v.y, v.z };
			
			transf_v = rot * transf_v; // rotation
			transf_v += { 0.0f, 0.0f, 2.0f };

			v.x = transf_v.x;
			v.y = transf_v.y;
			v.z = transf_v.z;
			FromNDCToScreenSpace(v); // from ndc to screen space
		}

		for (int i = 0; i < indices.size() / 3; i++)
		{
			int index1 = indices[i * 3 + 0];
			int index2 = indices[i * 3 + 1];
			int index3 = indices[i * 3 + 2];


			auto v1 = vertices[index1];
			auto v2 = vertices[index2];
			auto v3 = vertices[index3];

			/*
			vec3 edge1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
			vec3 edge2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
			vec3 normal = edge1.cross(edge2);

			vec3 viewVector = -vec3(v1.x, v1.y, v1.z);
			float dotProduct = normal.dot(viewVector);
			*/
		
			/*
			Soft::Renderer::DrawTriangle(
				{ v1.x, v1.y, v1.z },
				{ v2.x, v2.y, v2.z },
				{ v3.x, v3.y, v3.z },
				colors[i % 12]);
			*/

			Soft::Renderer::DrawTriangle(
				v1,
				v2,
				v3,
				colors[i % 12],
				tex.get());
		}

		Soft::Renderer::Present();

		App->Update();
	}
	Soft::Renderer::Destroy();
}

void OnResize(uint32_t width, uint32_t height)
{
	Soft::Renderer::Resize(width, height);
}
#else


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GpuLayer/stb_image.h"

#include "GpuLayer/shader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("res/wall.jpg", &width, &height, &nrChannels, 0);
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
			* glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));

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