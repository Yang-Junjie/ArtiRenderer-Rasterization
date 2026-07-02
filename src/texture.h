#pragma once
#include "math/vec2.h"
#include "math/vec4.h"

#include <cstdint>

#include <stb_image.h>
#include <string>
#include <vector>

enum class AddressMode {
    Repeat,
    Clamp,
    Mirror
};

struct MipLevel {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixels;
};

class Texture {
public:
    Texture();
    Texture(int width, int height, int channels);
    ~Texture() = default;
    bool loadFromFIle(const std::string& path);
    void generateMipmaps();

    Vec4 sampleNearest(float u, float v) const;
    Vec4 sampleNearest(float u, float v, int level) const;

    Vec4 sampleBilinear(float u, float v) const;
    Vec4 sampleBilinear(float u, float v, int level) const;

    Vec4 sampleTrilinear(float u, float v, float level) const;

    Vec4 sample(float u, float v, float du, float dv) const;
    Vec4 sampleAnisotropic(float u, float v, const Vec2& duv_dx, const Vec2& duv_dy, int max_anisotropy = 8) const;

    int getWidth() const
    {
        return m_mip_levels.empty() ? 0 : m_mip_levels[0].width;
    }

    int getHeight() const
    {
        return m_mip_levels.empty() ? 0 : m_mip_levels[0].height;
    }

    int getChannels() const
    {
        return m_channels;
    }

    int getMipLevelCount() const
    {
        return static_cast<int>(m_mip_levels.size());
    }

    int getMaxMipLevel() const
    {
        return m_mip_levels.empty() ? 0 : static_cast<int>(m_mip_levels.size()) - 1;
    }

    void setChannels(int channels)
    {
        m_channels = channels;
    }

    void setAddressMode(const AddressMode& mode)
    {
        m_address_mode = mode;
    }

    AddressMode getAddressMode() const
    {
        return m_address_mode;
    }

    std::vector<uint8_t>& getPixels(int level = 0)
    {
        if (m_mip_levels.empty()) {
            m_mip_levels.emplace_back();
        }
        if (m_mip_levels.size() > 1) {
            m_mip_levels.resize(1);
        }
        return m_mip_levels[level].pixels;
    }

    const std::vector<uint8_t>& getPixels(int level) const
    {
        static const std::vector<uint8_t> empty_pixels;
        if (m_mip_levels.empty()) {
            return empty_pixels;
        }
        return m_mip_levels[level].pixels;
    }

private:
    Vec4 getPixel(int x, int y, int level) const;
    float wrapCoord(float coord) const;
    int wrapIndex(int index, int size) const;
    float computeMipLevel(float du, float dv) const;

private:
    int m_channels;
    std::vector<MipLevel> m_mip_levels;
    AddressMode m_address_mode = AddressMode::Clamp;
};
