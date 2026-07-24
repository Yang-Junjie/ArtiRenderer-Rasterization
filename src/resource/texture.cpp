#include "resource/texture.h"

#include <cmath>
#include <cstring>

#include <algorithm>
#include <utility>

Texture::Texture()
    : m_channels(3),
      m_mip_levels()
{}

Texture::Texture(int width, int height, int channels)
    : m_channels(channels),
      m_mip_levels()
{
    MipLevel base;
    base.width = width;
    base.height = height;
    base.pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels));

    m_mip_levels.push_back(std::move(base));
}

bool Texture::loadFromFIle(const std::string& path)
{
    int w, h, c;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);

    if (!data) {
        return false;
    }

    const size_t size = static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(c);

    MipLevel base;
    base.width = w;
    base.height = h;
    base.pixels.resize(size);
    std::memcpy(base.pixels.data(), data, size);

    stbi_image_free(data);

    m_channels = c;
    m_mip_levels.clear();
    m_mip_levels.push_back(std::move(base));
    generateMipmaps();

    return true;
}

Vec4 Texture::sampleNearest(float u, float v) const
{
    return sampleNearest(u, v, 0);
}

Vec4 Texture::sampleNearest(float u, float v, int level) const
{
    if (m_mip_levels.empty()) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    level = std::clamp(level, 0, static_cast<int>(m_mip_levels.size()) - 1);
    const MipLevel& mip = m_mip_levels[level];

    u = wrapCoord(u);
    v = wrapCoord(v);

    // Convert to pixel coordinates
    int x = static_cast<int>(u * static_cast<float>(mip.width));
    int y = static_cast<int>(v * static_cast<float>(mip.height));

    return getPixel(x, y, level);
}

Vec4 Texture::sampleBilinear(float u, float v) const
{
    return sampleBilinear(u, v, 0);
}

Vec4 Texture::sampleBilinear(float u, float v, int level) const
{
    if (m_mip_levels.empty()) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    level = std::clamp(level, 0, static_cast<int>(m_mip_levels.size()) - 1);
    const MipLevel& mip = m_mip_levels[level];

    u = wrapCoord(u);
    v = wrapCoord(v);

    // Convert to pixel coordinates
    float fx = u * static_cast<float>(mip.width) - 0.5f;
    float fy = v * static_cast<float>(mip.height) - 0.5f;

    // Get fx and fy as the integer coordinates of the surrounding pixels
    // [x0,y0] [x0,y1]
    // [x1,y0] [x1,y1]
    const float floor_x = std::floor(fx);
    const float floor_y = std::floor(fy);
    int x0 = static_cast<int>(floor_x);
    int y0 = static_cast<int>(floor_y);
    float dx = fx - floor_x;
    float dy = fy - floor_y;

    int x1 = x0 + 1;
    int y1 = y0 + 1;

    Vec4 c00 = getPixel(x0, y0, level);
    Vec4 c10 = getPixel(x1, y0, level);
    Vec4 c01 = getPixel(x0, y1, level);
    Vec4 c11 = getPixel(x1, y1, level);

    // bilinear interpolation
    Vec4 c0 = c00 * (1.0f - dx) + c10 * dx;
    Vec4 c1 = c01 * (1.0f - dx) + c11 * dx;

    Vec4 result = c0 * (1.0f - dy) + c1 * dy;

    return result;
}

Vec4 Texture::sampleTrilinear(float u, float v, float level) const
{
    if (m_mip_levels.empty()) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    float maxLevel = static_cast<float>(m_mip_levels.size() - 1);
    level = std::clamp(level, 0.0f, maxLevel);

    int level0 = static_cast<int>(std::floor(level));
    int level1 = std::min(level0 + 1, static_cast<int>(m_mip_levels.size()) - 1);
    float frac = level - static_cast<float>(level0);

    Vec4 c0 = sampleBilinear(u, v, level0);
    Vec4 c1 = sampleBilinear(u, v, level1);

    return c0 * (1.0f - frac) + c1 * frac;
}

Vec4 Texture::sample(float u, float v, float du, float dv) const
{
    if (m_mip_levels.empty()) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    // calculate the appropriate mip level based on the derivatives
    float level = computeMipLevel(du, dv);

    if (level <= 0.0f) {
        return sampleBilinear(u, v, 0);
    } else {
        return sampleTrilinear(u, v, level);
    }
}

Vec4 Texture::sampleAnisotropic(float u, float v, const Vec2& duv_dx, const Vec2& duv_dy, int max_anisotropy) const
{
    if (m_mip_levels.empty()) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    const MipLevel& base = m_mip_levels[0];
    if (base.width <= 0 || base.height <= 0) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    max_anisotropy = std::max(1, max_anisotropy);

    const Vec2 dx_texels(duv_dx.x() * static_cast<float>(base.width), duv_dx.y() * static_cast<float>(base.height));
    const Vec2 dy_texels(duv_dy.x() * static_cast<float>(base.width), duv_dy.y() * static_cast<float>(base.height));

    const float dx_len = dx_texels.length();
    const float dy_len = dy_texels.length();

    const Vec2 major_axis_uv = dx_len >= dy_len ? duv_dx : duv_dy;
    const float major_len = std::max(dx_len, dy_len);
    const float raw_minor_len = std::min(dx_len, dy_len);

    if (major_len < 1e-6f) {
        return sampleBilinear(u, v, 0);
    }

    const float max_anisotropy_f = static_cast<float>(max_anisotropy);
    const float minor_len = std::max({raw_minor_len, major_len / max_anisotropy_f, 1.0f});
    const float level = std::clamp(
        std::log2(minor_len),
        0.0f,
        static_cast<float>(m_mip_levels.size() - 1));

    const float anisotropy = std::max(1.0f, major_len / minor_len);
    const int sample_count = std::clamp(static_cast<int>(std::ceil(anisotropy)), 1, max_anisotropy);

    Vec4 result(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < sample_count; ++i) {
        const float offset = (static_cast<float>(i) + 0.5f) / static_cast<float>(sample_count) - 0.5f;
        result += sampleTrilinear(
            u + major_axis_uv.x() * offset,
            v + major_axis_uv.y() * offset,
            level);
    }

    return result / static_cast<float>(sample_count);
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

int Texture::wrapIndex(int index, int size) const
{
    if (size <= 0) {
        return 0;
    }

    switch (m_address_mode) {
        case AddressMode::Clamp:
            return std::clamp(index, 0, size - 1);

        case AddressMode::Repeat: {
            int wrapped = index % size;
            if (wrapped < 0) {
                wrapped += size;
            }
            return wrapped;
        }

        case AddressMode::Mirror: {
            if (size == 1) {
                return 0;
            }

            const int period = size * 2;
            int wrapped = index % period;
            if (wrapped < 0) {
                wrapped += period;
            }
            if (wrapped >= size) {
                wrapped = period - 1 - wrapped;
            }
            return wrapped;
        }
    }

    return std::clamp(index, 0, size - 1);
}

Vec4 Texture::getPixel(int x, int y, int level) const
{
    if (m_mip_levels.empty() || m_channels <= 0) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    level = std::clamp(level, 0, static_cast<int>(m_mip_levels.size()) - 1);
    const MipLevel& mip = m_mip_levels[level];

    if (mip.width <= 0 || mip.height <= 0 || mip.pixels.empty()) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    x = wrapIndex(x, mip.width);
    y = wrapIndex(y, mip.height);

    // Calculate the index of the pixel
    size_t index = (static_cast<size_t>(y) * static_cast<size_t>(mip.width) + static_cast<size_t>(x)) *
                   static_cast<size_t>(m_channels);

    if (index >= mip.pixels.size()) {
        return Vec4(0.0f, 0.0f, 0.0f, 255.0f);
    }

    const auto read_channel = [&](int channel, float fallback) {
        if (channel >= m_channels) {
            return fallback;
        }

        const size_t channel_index = index + static_cast<size_t>(channel);
        if (channel_index >= mip.pixels.size()) {
            return fallback;
        }

        return static_cast<float>(mip.pixels[channel_index]);
    };

    const float r = read_channel(0, 0.0f);
    const float g = read_channel(1, 0.0f);
    const float b = read_channel(2, 0.0f);
    const float a = read_channel(3, 255.0f);

    return Vec4(r, g, b, a);
}

void Texture::generateMipmaps()
{
    if (m_mip_levels.empty()) {
        return;
    }

    if (m_mip_levels.size() > 1) {
        MipLevel base = std::move(m_mip_levels[0]);
        m_mip_levels.clear();
        m_mip_levels.push_back(std::move(base));
    }

    const MipLevel& base = m_mip_levels[0];
    if (base.width <= 0 || base.height <= 0 || m_channels <= 0 || base.pixels.empty()) {
        return;
    }

    // gererate mipmaps until the last level is 1x1
    while (m_mip_levels.back().width > 1 || m_mip_levels.back().height > 1) {

        const MipLevel& prev = m_mip_levels.back();

        // Create the next mip level
        MipLevel next;
        next.width = std::max(1, (prev.width + 1) / 2);
        next.height = std::max(1, (prev.height + 1) / 2);
        next.pixels.resize(static_cast<size_t>(next.width) * static_cast<size_t>(next.height) *
                           static_cast<size_t>(m_channels));

        // Downsample the previous level to create the next level
        for (int dstY = 0; dstY < next.height; ++dstY) {
            for (int dstX = 0; dstX < next.width; ++dstX) {
                int srcX = dstX * 2;
                int srcY = dstY * 2;

                for (int channel = 0; channel < m_channels; ++channel) {
                    int sum = 0;
                    int count = 0;

                    for (int dy = 0; dy < 2; ++dy) {
                        int sampleY = srcY + dy;
                        if (sampleY >= prev.height) {
                            continue;
                        }

                        for (int dx = 0; dx < 2; ++dx) {
                            int sampleX = srcX + dx;
                            if (sampleX >= prev.width) {
                                continue;
                            }

                            size_t srcIndex =
                                (static_cast<size_t>(sampleY) * prev.width + static_cast<size_t>(sampleX)) *
                                    m_channels +
                                channel;

                            sum += prev.pixels[srcIndex];
                            count++;
                        }
                    }

                    size_t dstIndex =
                        (static_cast<size_t>(dstY) * next.width + static_cast<size_t>(dstX)) * m_channels + channel;

                    next.pixels[dstIndex] = static_cast<uint8_t>(sum / count);
                }
            }
        }

        m_mip_levels.push_back(std::move(next));
    }
}

float Texture::computeMipLevel(float du, float dv) const
{
    if (m_mip_levels.empty()) {
        return 0.0f;
    }

    float maxDeriv = std::max(std::abs(du), std::abs(dv));

    if (maxDeriv < 1e-6f) {
        return 0.0f;
    }

    // calculate the pixel range covered by the texture
    const MipLevel& base = m_mip_levels[0];
    float pixelRange = maxDeriv * static_cast<float>(std::max(base.width, base.height));

    // calculate the mip level based on the pixel range
    float level = std::log2(pixelRange);

    return std::clamp(level, 0.0f, static_cast<float>(m_mip_levels.size() - 1));
}
