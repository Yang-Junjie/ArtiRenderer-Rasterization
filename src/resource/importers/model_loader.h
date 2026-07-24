#pragma once
#include "resource/model.h"

#include <cstdint>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace tinyobj {
struct attrib_t;
struct material_t;
struct shape_t;
} // namespace tinyobj

class ModelLoader {
public:
    ModelLoader() = default;
    ModelLoader(const ModelLoader&) = delete;
    ModelLoader& operator=(const ModelLoader&) = delete;

    ModelLoader(ModelLoader&&) = delete;
    ModelLoader& operator=(ModelLoader&&) = delete;

    static const ModelLoader& get()
    {
        static ModelLoader model_loader;
        return model_loader;
    }

    std::shared_ptr<Model> loadModel(const std::string& path) const;

private:
    struct SubMeshBuildData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    static Vec3 position(const tinyobj::attrib_t& attrib, int vertex_index);
    static std::vector<Vec3> generateNormals(const tinyobj::attrib_t& attrib,
                                             const std::vector<tinyobj::shape_t>& shapes);
    static Vec3 getGeneratedNormal(const std::vector<Vec3>& generated_normals, int vertex_index);
    static Vec4 materialColor(const std::vector<tinyobj::material_t>& materials, int material_id);
    static float average(float x, float y, float z);
    static std::unique_ptr<Material> createMaterial(const std::vector<tinyobj::material_t>& materials,
                                                    int material_id,
                                                    const std::filesystem::path& base_dir);
};
