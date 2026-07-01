#include "texture.h"

Texture::Texture()
    : m_width(0),
      m_height(0),
      m_channels(0),
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
