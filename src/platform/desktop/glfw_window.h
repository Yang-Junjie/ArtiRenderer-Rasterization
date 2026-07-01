#pragma once

#include "platform/window.h"

#include <cstdint>

#include <string>
#include <vector>

struct GLFWwindow;

class GlfwWindow final : public Window {
public:
    GlfwWindow(uint32_t width, uint32_t height, const std::string& title);
    ~GlfwWindow() override;

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;

    void update() override;
    void present(const std::vector<Vec4>& frame_buffer) override;
    bool shouldClose() const override;

private:
    void initializeRenderResources();
    void updateTexture(const std::vector<Vec4>& frame_buffer);

private:
    GLFWwindow* m_window = nullptr;
    unsigned int m_shader_program = 0;
    unsigned int m_vertex_array = 0;
    unsigned int m_vertex_buffer = 0;
    unsigned int m_texture = 0;
    std::vector<uint8_t> m_upload_pixels;
};
