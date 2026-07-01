#include "texture.h"

#include <algorithm>

Texture::Texture()
    : m_width(0),
      m_height(0),
      m_channels(3),
      m_pixels()
{}

bool Texture::loadFromFIle(const std::string& path)
{
    int w, h, c;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);

    if (!data) {
        return false;
    }
    const size_t size = static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(c);

    m_pixels.resize(size);
    std::memcpy(m_pixels.data(), data, size);

    stbi_image_free(data);

    m_width = w;
    m_height = h;
    m_channels = c;

    return true;
}

Vec4 Texture::sampleNearest(float u, float v) const
{
    if (m_width <= 0 || m_height <= 0 || m_channels <= 0 || m_pixels.empty()) {
        return Vec4(255.0f, 255.0f, 255.0f, 255.0f);
    }

    // Clamp u and v to [0, 1)
    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);

    // Convert to pixel coordinates
    int x = static_cast<int>(u * m_width);
    int y = static_cast<int>(v * m_height);

    x = std::min(x, m_width - 1);
    y = std::min(y, m_height - 1);

    // Calculate the index of the pixel
    size_t index = (y * m_width + x) * m_channels;

    const float r = static_cast<float>(m_pixels[index]);
    const float g = m_channels > 1 ? static_cast<float>(m_pixels[index + 1]) : 0;
    const float b = m_channels > 2 ? static_cast<float>(m_pixels[index + 2]) : 0;
    const float a = m_channels > 3 ? static_cast<float>(m_pixels[index + 3]) : 255.0f;

    return Vec4(r, g, b, a);
}
