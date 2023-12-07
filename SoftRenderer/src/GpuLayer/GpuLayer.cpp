#include "GpuLayer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace
{
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    void main() {
        gl_Position = vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)";

    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;

    uniform sampler2D ourTexture;

    void main() {
        FragColor = texture(ourTexture, TexCoord);
    }
)";

    GLuint FrameBuffer_VBO, FrameBuffer_VAO;
    GLuint vertexShader, fragmentShader, shaderProgram;
}

void GpuLayer::Init(void* NativeWindowHandel)
{
	glfwMakeContextCurrent((GLFWwindow*)NativeWindowHandel);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	GPULAYER_ASSERT(status, "Failed To Initialize GLAD!");


    {// Setup Frame Buffer OpenGL Objects and shaders

        // Compile and link shaders

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Set up vertex data for a quad
        float vertices[] = 
        {
            -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        };

        glGenVertexArrays(1, &FrameBuffer_VAO);
        glGenBuffers(1, &FrameBuffer_VBO);

        glBindVertexArray(FrameBuffer_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, FrameBuffer_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

FrameBuffer::FrameBuffer(uint32_t Width, uint32_t Height)
{
    Invalidate(Width, Height);
}

void FrameBuffer::Invalidate(uint32_t Width, uint32_t Height)
{
    m_Width = Width;
    m_Height = Height;

    if (textureID)
        glDeleteTextures(1, &textureID);


    // Create texture object
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Allocate storage for the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Set texture parameters (you can adjust these as needed)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0, 0, Width, Height);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteTextures(1, &textureID);
}


void FrameBuffer::Bind(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void FrameBuffer::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::UpdatePixels(void* Pixels)
{
    // Update the texture with new pixel data
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::Resize(uint32_t Width, uint32_t Height)
{
    Invalidate(Width, Height);
}

void FrameBuffer::Present(void* Pixels)
{
    UpdatePixels(Pixels);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    Bind();

    glBindVertexArray(FrameBuffer_VAO);
    glDrawArrays(GL_QUADS, 0, 4);
    Unbind();
}
