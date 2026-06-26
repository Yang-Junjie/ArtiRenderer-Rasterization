#pragma once
#include "math/vec4.h"

#include <cstdint>

#include <string>
#include <vector>

class Window {
public:
    Window(uint32_t width, uint32_t height, std::string title);
    virtual ~Window();

    virtual void update() = 0;
    virtual void present(const std::vector<Vec4>& frameBuffer) = 0;
    virtual bool shouldClose() const = 0;

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    const std::string& getTitle() const;

protected:
    uint32_t m_width;
    uint32_t m_height;
    std::string m_title;
};
