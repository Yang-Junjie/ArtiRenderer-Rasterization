#include "platform/window.h"

#include <utility>

Window::Window(uint32_t width, uint32_t height, std::string title)
    : m_width(width),
      m_height(height),
      m_title(std::move(title))
{}

Window::~Window() = default;

uint32_t Window::getWidth() const
{
    return m_width;
}

uint32_t Window::getHeight() const
{
    return m_height;
}

const std::string& Window::getTitle() const
{
    return m_title;
}
