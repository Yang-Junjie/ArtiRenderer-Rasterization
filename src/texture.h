#pragma once

#include <stb_image.h>
#include <string>
#include <vector>
#include <cstdint>

class Texture {
public:
    Texture();
    ~Texture() = default;
    bool loadFromFIle(const std::string& path);

    int getWidth() const{
        return m_width;
    }
    int getHeight() const{
        return m_height;
    }
    int getChannels() const{
        return m_channels;
    }
    
    std::vector<uint8_t> getPixels() const{
        return m_pixels;
    }

private:
    int m_width;
    int m_height;
    int m_channels;
    std::vector<uint8_t> m_pixels;
};
