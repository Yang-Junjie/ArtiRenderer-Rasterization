#include "resource/importers/model_loader.h"

#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <filesystem>
#include <map>
#include <memory>
#include <tiny_obj_loader.h>
#include <utility>
#include <vector>

std::shared_ptr<Model> ModelLoader::loadModel(const std::string& path) const
{
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    const std::filesystem::path obj_path(path);
    const std::filesystem::path base_dir = obj_path.parent_path();
    config.triangulate = true;
    config.mtl_search_path = base_dir.string();

    reader.ParseFromFile(path, config);

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();
    const std::vector<Vec3> generated_normals = generateNormals(attrib, shapes);

    std::map<int, SubMeshBuildData> sub_meshes;

    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            const int material_id = shape.mesh.material_ids[f];
            SubMeshBuildData& sub_mesh = sub_meshes[material_id];

            const uint32_t face_vertex_count = shape.mesh.num_face_vertices[f];
            for (uint32_t v = 0; v < face_vertex_count; ++v) {
                const tinyobj::index_t index = shape.mesh.indices[index_offset + v];

                Vertex vertex;

                // -------------- Position --------------
                vertex.position[0] = attrib.vertices[3 * index.vertex_index + 0];
                vertex.position[1] = attrib.vertices[3 * index.vertex_index + 1];
                vertex.position[2] = attrib.vertices[3 * index.vertex_index + 2];

                // -------------- Normal --------------
                if (index.normal_index >= 0) {
                    vertex.normal[0] = attrib.normals[3 * index.normal_index + 0];
                    vertex.normal[1] = attrib.normals[3 * index.normal_index + 1];
                    vertex.normal[2] = attrib.normals[3 * index.normal_index + 2];
                } else {
                    vertex.normal = getGeneratedNormal(generated_normals, index.vertex_index);
                }

                // -------------- TexCoord --------------
                if (index.texcoord_index >= 0) {
                    vertex.texCoord[0] = attrib.texcoords[2 * index.texcoord_index + 0];
                    vertex.texCoord[1] = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];
                } else {
                    vertex.texCoord = Vec2(0.0f, 0.0f);
                }

                vertex.color = materialColor(materials, material_id);
                sub_mesh.indices.push_back(static_cast<uint32_t>(sub_mesh.vertices.size()));
                sub_mesh.vertices.push_back(vertex);
            }

            index_offset += face_vertex_count;
        }
    }

    auto model = std::make_shared<Model>();
    for (auto& [material_id, sub_mesh] : sub_meshes) {
        auto mesh = std::make_unique<Mesh>(std::move(sub_mesh.vertices), std::move(sub_mesh.indices));
        auto material = createMaterial(materials, material_id, base_dir);
        model->addSubMesh(std::move(mesh), std::move(material));
    }

    return model;
}

Vec3 ModelLoader::position(const tinyobj::attrib_t& attrib, int vertex_index)
{
    return Vec3(attrib.vertices[3 * vertex_index + 0],
                attrib.vertices[3 * vertex_index + 1],
                attrib.vertices[3 * vertex_index + 2]);
}

std::vector<Vec3> ModelLoader::generateNormals(const tinyobj::attrib_t& attrib,
                                               const std::vector<tinyobj::shape_t>& shapes)
{
    std::vector<Vec3> normals(attrib.vertices.size() / 3, Vec3(0.0f, 0.0f, 0.0f));

    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            const uint32_t face_vertex_count = shape.mesh.num_face_vertices[f];
            if (face_vertex_count < 3) {
                index_offset += face_vertex_count;
                continue;
            }

            const tinyobj::index_t i0 = shape.mesh.indices[index_offset + 0];
            const tinyobj::index_t i1 = shape.mesh.indices[index_offset + 1];
            const tinyobj::index_t i2 = shape.mesh.indices[index_offset + 2];

            const Vec3 p0 = position(attrib, i0.vertex_index);
            const Vec3 p1 = position(attrib, i1.vertex_index);
            const Vec3 p2 = position(attrib, i2.vertex_index);
            const Vec3 face_normal = cross(p1 - p0, p2 - p0);

            if (!face_normal.nearZero()) {
                for (uint32_t v = 0; v < face_vertex_count; ++v) {
                    const int vertex_index = shape.mesh.indices[index_offset + v].vertex_index;
                    normals[static_cast<size_t>(vertex_index)] += face_normal;
                }
            }

            index_offset += face_vertex_count;
        }
    }

    for (Vec3& normal : normals) {
        if (normal.nearZero()) {
            normal = Vec3(0.0f, 0.0f, 1.0f);
        } else {
            normal = unitVector(normal);
        }
    }

    return normals;
}

Vec3 ModelLoader::getGeneratedNormal(const std::vector<Vec3>& generated_normals, int vertex_index)
{
    if (vertex_index < 0) {
        return Vec3(0.0f, 0.0f, 1.0f);
    }

    return generated_normals[static_cast<size_t>(vertex_index)];
}

Vec4 ModelLoader::materialColor(const std::vector<tinyobj::material_t>& materials, int material_id)
{
    if (material_id < 0) {
        return Vec4(255.0f, 255.0f, 255.0f, 255.0f);
    }

    const tinyobj::material_t& material = materials[static_cast<size_t>(material_id)];
    return Vec4(material.diffuse[0] * 255.0f,
                material.diffuse[1] * 255.0f,
                material.diffuse[2] * 255.0f,
                material.dissolve * 255.0f);
}

float ModelLoader::average(float x, float y, float z)
{
    return (x + y + z) / 3.0f;
}

std::unique_ptr<Material> ModelLoader::createMaterial(const std::vector<tinyobj::material_t>& materials,
                                                      int material_id,
                                                      const std::filesystem::path& base_dir)
{
    auto material = std::make_unique<Material>();
    if (material_id < 0) {
        return material;
    }

    const tinyobj::material_t& source = materials[static_cast<size_t>(material_id)];

    material->ambient_strength = average(source.ambient[0], source.ambient[1], source.ambient[2]);
    material->diffuse_strength = average(source.diffuse[0], source.diffuse[1], source.diffuse[2]);
    material->specular_strength = std::max({source.specular[0], source.specular[1], source.specular[2]});
    material->shininess = source.shininess;
    material->metallic = source.metallic;
    material->specular_color = Vec3(source.specular[0], source.specular[1], source.specular[2]);

    if (!source.diffuse_texname.empty()) {
        const std::filesystem::path texture_path = base_dir / source.diffuse_texname;
        material->albedo.loadFromFIle(texture_path.string());
        material->albedo.setAddressMode(AddressMode::Repeat);
    }

    return material;
}


