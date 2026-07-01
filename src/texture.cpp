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
    u = wrapCoord(u);
    v = wrapCoord(v);

    // Convert to pixel coordinates
    int x = static_cast<int>(u * m_width);
    int y = static_cast<int>(v * m_height);

    return getPixel(x, y);
}

Vec4 Texture::sampleBilinear(float u, float v) const
{
    u = wrapCoord(u);
    v = wrapCoord(v);

    // Convert to pixel coordinates
    float fx = u * static_cast<float>(m_width) - 0.5f;
    float fy = v * static_cast<float>(m_height) - 0.5f;

    // Get fx and fy as the integer coordinates of the surrounding pixels
    // [x0,y0] [x0,y1]
    // [x1,y0] [x1,y1]
    int x0 = static_cast<int>(std::floor(fx));
    int y0 = static_cast<int>(std::floor(fy));
    float dx = fx - std::floor(fx);
    float dy = fy - std::floor(fy);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    x0 = std::clamp(x0, 0, m_width - 1);
    x1 = std::clamp(x1, 0, m_width - 1);
    y0 = std::clamp(y0, 0, m_height - 1);
    y1 = std::clamp(y1, 0, m_height - 1);

    Vec4 c00 = getPixel(x0, y0);
    Vec4 c10 = getPixel(x1, y0);
    Vec4 c01 = getPixel(x0, y1);
    Vec4 c11 = getPixel(x1, y1);

    // bilinear interpolation
    Vec4 c0 = c00 * (1.0f - dx) + c10 * dx;
    Vec4 c1 = c01 * (1.0f - dx) + c11 * dx;

    Vec4 result = c0 * (1.0f - dy) + c1 * dy;

    return result;
}

float Texture::wrapCoord(float coord) const
{
    switch (m_address_mode) {
        case AddressMode::Clamp:
            return std::clamp(coord, 0.0f, 1.0f);

        case AddressMode::Repeat: {
            float result = coord - std::floor(coord);
            if (result >= 1.0f - 1e-6f) {
                result = 0.0f;
            }
            return result;
        }

        case AddressMode::Mirror: {
            float t = std::fmod(coord, 2.0f);
            if (t < 0.0f) {
                t += 2.0f;
            }
            return t > 1.0f ? 2.0f - t : t;
        }
    }
    return coord;
}

Vec4 Texture::getPixel(int x, int y) const
{
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
