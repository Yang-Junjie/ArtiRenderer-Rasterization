#pragma once
#include "math/vec4.h"

#include <cstdint>

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

class PPMWriter {
public:
    // data should be in the range [0, 255]
    void writePPM(const std::vector<Vec4>& data)
    {
        std::ofstream ofs(m_file_name, std::ios::binary);
        if (!ofs.is_open()) {
            return;
        }

        ofs << "P6\n" << m_image_width << " " << m_image_height << "\n255\n";

        const size_t pixel_count = data.size();
        std::vector<uint8_t> buffer;
        buffer.resize(pixel_count * 3);

        for (size_t i = 0; i < pixel_count; ++i) {
            const auto& pixel = data[i];
            const size_t idx = i * 3;
            buffer[idx] = static_cast<uint8_t>(std::clamp(pixel.x(), 0.0f, 255.0f));
            buffer[idx + 1] = static_cast<uint8_t>(std::clamp(pixel.y(), 0.0f, 255.0f));
            buffer[idx + 2] = static_cast<uint8_t>(std::clamp(pixel.z(), 0.0f, 255.0f));
        }

        ofs.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    }

    void setImageSize(uint32_t width, uint32_t height)
    {
        m_image_width = width;
        m_image_height = height;
    }

    void setFileName(const std::string& fileName)
    {
        m_file_name = fileName;
    }

private:
    uint32_t m_image_width = 0;
    uint32_t m_image_height = 0;
    std::string m_file_name;
};
