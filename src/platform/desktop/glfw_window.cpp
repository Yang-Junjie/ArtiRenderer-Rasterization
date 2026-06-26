#include "platform/desktop/glfw_window.h"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <cstddef>

#include <algorithm>
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace {

constexpr const char* kVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
    vTexCoord = aTexCoord;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
)";

constexpr const char* kFragmentShaderSource = R"(
#version 330 core
in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D uFrameTexture;

void main()
{
    fragColor = texture(uFrameTexture, vTexCoord);
}
)";

uint8_t toByte(float value)
{
    return static_cast<uint8_t>(std::clamp(value, 0.0f, 255.0f));
}

unsigned int compileShader(unsigned int shaderType, const char* shaderSource)
{
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);
    return shader;
}

struct QuadVertex {
    float x;
    float y;
    float u;
    float v;
};

} // namespace

GlfwWindow::GlfwWindow(uint32_t width, uint32_t height, const std::string& title)
    : Window(width, height, title)
{
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window =
        glfwCreateWindow(static_cast<int>(m_width), static_cast<int>(m_height), m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        throw std::runtime_error("Failed to load OpenGL functions");
    }

    initializeRenderResources();
}

GlfwWindow::~GlfwWindow()
{
    glDeleteTextures(1, &m_texture);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteVertexArrays(1, &m_vertex_array);
    glDeleteProgram(m_shader_program);

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void GlfwWindow::update()
{
    glfwPollEvents();

    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}

void GlfwWindow::present(const std::vector<Vec4>& frameBuffer)
{
    updateTexture(frameBuffer);

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(m_window, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m_shader_program);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray(m_vertex_array);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapBuffers(m_window);
}

bool GlfwWindow::shouldClose() const
{
    return glfwWindowShouldClose(m_window) == GLFW_TRUE;
}

void GlfwWindow::initializeRenderResources()
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, kVertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSource);

    m_shader_program = glCreateProgram();
    glAttachShader(m_shader_program, vertexShader);
    glAttachShader(m_shader_program, fragmentShader);
    glLinkProgram(m_shader_program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    constexpr QuadVertex kQuadVertices[] = {
        {-1.0f, -1.0f, 0.0f, 1.0f},
        {1.0f, -1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
    };

    glGenVertexArrays(1, &m_vertex_array);
    glGenBuffers(1, &m_vertex_buffer);

    glBindVertexArray(m_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), reinterpret_cast<void*>(2 * sizeof(float)));

    m_upload_pixels.resize(static_cast<std::size_t>(m_width) * static_cast<std::size_t>(m_height) * 3);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB8,
                 static_cast<int>(m_width),
                 static_cast<int>(m_height),
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 nullptr);

    glUseProgram(m_shader_program);
    glUniform1i(glGetUniformLocation(m_shader_program, "uFrameTexture"), 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void GlfwWindow::updateTexture(const std::vector<Vec4>& frameBuffer)
{
    for (std::size_t i = 0; i < frameBuffer.size(); ++i) {
        m_upload_pixels[i * 3] = toByte(frameBuffer[i].x());
        m_upload_pixels[i * 3 + 1] = toByte(frameBuffer[i].y());
        m_upload_pixels[i * 3 + 2] = toByte(frameBuffer[i].z());
    }

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    static_cast<int>(m_width),
                    static_cast<int>(m_height),
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    m_upload_pixels.data());
}
