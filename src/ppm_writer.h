#pragma once
#include "math/vec3.h"

#include <cstdint>

#include <algorithm> 
#include <fstream>
#include <string>
#include <vector>

class PPMWriter {
public:
    // data should be in the range [0, 255]
    void writePPM(const std::vector<Vec3>& data)
    {
        std::ofstream ofs(m_file_name, std::ios::binary);
        if (!ofs.is_open()) {
            return;
        }

        ofs << "P6\n" << m_image_width << " " << m_image_height << "\n255\n";

        for (size_t i = 0; i < m_image_width * m_image_height; ++i) {
            uint8_t pixel[] = {
                static_cast<uint8_t>(std::clamp(data[i].x(), 0.0f, 255.0f)),
                static_cast<uint8_t>(std::clamp(data[i].y(), 0.0f, 255.0f)),
                static_cast<uint8_t>(std::clamp(data[i].z(), 0.0f, 255.0f)),
            };
            ofs.write(reinterpret_cast<const char*>(pixel), 3);
        }

        ofs.close();
    }

    void setImageSize(uint32_t width, uint32_t height)
    {
        m_image_width = width;
        m_image_height = height;
    }

    void setFileName(const std::string& file_name)
    {
        m_file_name = file_name;
    }

private:
    uint32_t m_image_width = 0;
    uint32_t m_image_height = 0;
    std::string m_file_name;
};
