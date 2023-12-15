#include "mesh.h"
#include "obj_loader.h"

#include <iostream>

Mesh::Mesh(const std::filesystem::path& path)
{
    // Load .obj file
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    std::string warn;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str());

    if (!warn.empty())
        std::cout << warn << std::endl;

    if (!err.empty())
        std::cout << err << std::endl;

    if (!ret)
        return;

    // Process vertices and indices
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            // Access vertex data
            tinyobj::real_t vx = attrib.vertices[3 * index.vertex_index + 0];
            tinyobj::real_t vy = attrib.vertices[3 * index.vertex_index + 1];
            tinyobj::real_t vz = attrib.vertices[3 * index.vertex_index + 2];

            // Access normal data if needed
            tinyobj::real_t nx = attrib.normals[3 * index.normal_index + 0];
            tinyobj::real_t ny = attrib.normals[3 * index.normal_index + 1];
            tinyobj::real_t nz = attrib.normals[3 * index.normal_index + 2];

            // Access texture coordinate data if needed
            tinyobj::real_t u = attrib.texcoords[2 * index.texcoord_index + 0];
            tinyobj::real_t v = attrib.texcoords[2 * index.texcoord_index + 1];

            m_Vertices.push_back({ vx, vy, vz, 1.0f, u, v });
        }
    }
}
