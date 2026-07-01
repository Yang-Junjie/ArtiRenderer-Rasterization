#pragma once
#include "math/vec4.h"

#include <cstdint>

#include <stb_image.h>
#include <string>
#include <vector>


class Texture {
public:
    Texture();
    ~Texture() = default;
    bool loadFromFIle(const std::string& path);

    int getWidth() const
    {
        return m_width;
    }

    int getHeight() const
    {
        return m_height;
    }

    int getChannels() const
    {
        return m_channels;
    }

    void setChannels(int channels)
    {
        // Ensure channels do not exceed 4 (RGBA)
        m_channels = std::min(channels, 4);
    }

    std::vector<uint8_t> getPixels() const
    {
        return m_pixels;
    }

    Vec4 sampleNearest(float u, float v) const;

private:
    int m_width;
    int m_height;
    int m_channels;
    std::vector<uint8_t> m_pixels;
};
