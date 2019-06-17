#include "modelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "graphics.h"
#include <unordered_map>

#include <bx/pixelformat.h>

inline uint32_t encodeNormalRgba8(float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
{
    const float src[] =
    {
        _x * 0.5f + 0.5f,
        _y * 0.5f + 0.5f,
        _z * 0.5f + 0.5f,
        _w * 0.5f + 0.5f,
    };
    uint32_t dst;
    bx::packRgba8(&dst, src);
    return dst;
}

void LoadShape(std::vector<Mesh>& elements, tinyobj::attrib_t& attrib,
    const tinyobj::shape_t& shape,
    std::vector<tinyobj::material_t>& materials)
{
    const auto allocSize = shape.mesh.indices.size();

    std::vector<uint16_t> meshIndices;
    std::vector<PosNormalVertex> meshVertices;

    /*
    std::unordered_map<int, int> materialToShapeMap;
    for (auto materialId : shape.mesh.material_ids)
    {
        if (materialToShapeMap.find(materialId) == materialToShapeMap.end())
        {
            OBJModelLoader::SubmeshDesc desc;
            desc.materialName = materials[materialId].name;

            int pos = submeshes.size();
            submeshes.push_back(desc);

            materialToShapeMap.emplace(materialId, pos);
        }
    }
    */

    //Iterate over faces
    size_t index_offset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
    {
        const int fv = shape.mesh.num_face_vertices[f];

        //int materialId = shape.mesh.material_ids[f];
        //const int descIndex = materialToShapeMap[materialId];

        //Iterate over vertices
        for (size_t v = 0; v < fv; v++)
        {
            auto& index = shape.mesh.indices[index_offset + v];

            PosNormalVertex vertex = {};
            vertex.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.z = attrib.vertices[3 * index.vertex_index + 2];

            if (attrib.normals.size() > 0)
            {
                float normalY = attrib.normals[3 * index.normal_index + 0];
                float normalX = attrib.normals[3 * index.normal_index + 1];
                float normalZ = attrib.normals[3 * index.normal_index + 2];

                vertex.normal = encodeNormalRgba8(normalX, normalY, normalZ);
            }

            /*if (attrib.texcoords.size() > 0)
            {
                vertex.texCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.texCoord.y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];
            }*/

            //TODO: Avoid duplicated vertices
            /*submeshes[descIndex].indices.push_back(static_cast<uint32_t>(submeshes[descIndex].vertices.size()));
            submeshes[descIndex].vertices.push_back(vertex);*/

            meshIndices.push_back(meshVertices.size());
            meshVertices.push_back(vertex);
        }
        index_offset += fv;
    }

    bgfx::VertexDecl pcvDecl;
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
        .end();

    const auto verticesMemory = bgfx::alloc(sizeof PosNormalVertex * meshVertices.size());
    memcpy(verticesMemory->data, meshVertices.data(), sizeof PosNormalVertex * meshVertices.size());

    const auto indicesMemory = bgfx::alloc(sizeof uint16_t * meshIndices.size());
    memcpy(indicesMemory->data, meshIndices.data(), sizeof uint16_t * meshIndices.size());

    Mesh mesh{};
    mesh.vertexBuffer = bgfx::createVertexBuffer(verticesMemory, pcvDecl);
    mesh.indexBuffer = bgfx::createIndexBuffer(indicesMemory);

    bgfx::ShaderHandle vsh = graphics::loadShader("vs_phong");
    bgfx::ShaderHandle fsh = graphics::loadShader("fs_phong");
    mesh.program = bgfx::createProgram(vsh, fsh, true);

    elements.push_back(mesh);
}

void modelLoader::loadModel(std::string modelPath, std::vector<Mesh>& elements)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    std::string warn;

    printf("Started to load Obj File");
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
    {
        throw std::runtime_error(err);
    }
    printf("Succesfully loaded obj file");

    for (const auto& shape : shapes)
    {
        LoadShape(elements, attrib, shape, materials);
        printf("Succesfully loaded a shape");
    }

    /*
    for (const auto& material : materials)
    {
        const auto data = LoadMaterial(material);
        outMaterials[data.name] = data;
        Logger::DebugLog("Succesfully loaded a material");
    }
    */
}


/*
 * void LoadShape(std::vector<OBJModelLoader::SubmeshDesc>& submeshes, tinyobj::attrib_t& attrib,
               const tinyobj::shape_t& shape,
               std::vector<tinyobj::material_t>& materials)
{
    const auto allocSize = shape.mesh.indices.size();

    std::unordered_map<int, int> materialToShapeMap;
    for (auto materialId : shape.mesh.material_ids)
    {
        if (materialToShapeMap.find(materialId) == materialToShapeMap.end())
        {
            OBJModelLoader::SubmeshDesc desc;
            desc.materialName = materials[materialId].name;

            int pos = submeshes.size();
            submeshes.push_back(desc);

            materialToShapeMap.emplace(materialId, pos);
        }
    }

    //Iterate over faces
    size_t index_offset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
    {
        const int fv = shape.mesh.num_face_vertices[f];

        int materialId = shape.mesh.material_ids[f];
        const int descIndex = materialToShapeMap[materialId];

        //Iterate over vertices
        for (size_t v = 0; v < fv; v++)
        {
            auto& index = shape.mesh.indices[index_offset + v];

            Vertex vertex = {};
            vertex.position.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.position.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.position.z = attrib.vertices[3 * index.vertex_index + 2];

            if (attrib.normals.size() > 0)
            {
                vertex.normal.y = attrib.normals[3 * index.normal_index + 0];
                vertex.normal.x = attrib.normals[3 * index.normal_index + 1];
                vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
            }

            if(attrib.texcoords.size() > 0)
            {
                vertex.texCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.texCoord.y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];
            }
           
            //TODO: Avoid duplicated vertices
            submeshes[descIndex].indices.push_back(static_cast<uint32_t>(submeshes[descIndex].vertices.size()));
            submeshes[descIndex].vertices.push_back(vertex);
        }
        index_offset += fv;
    }
}

OBJModelLoader::MaterialDesc OBJModelLoader::LoadMaterial(tinyobj::material_t material)
{
    MaterialDesc data;
    data.name = material.name;

    data.properties = material.illum == 1 ? Diffuse : None;

    data.ambient = Color(material.ambient[0], material.ambient[1], material.ambient[2]);
    data.diffuse = Color(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
    data.specular = Color(material.specular[0], material.specular[1], material.specular[2]);
    data.shininess = material.shininess;
    data.diffuseTexName = material.diffuse_texname;
    data.specularTexName = material.specular_texname;

    return data;
}

void OBJModelLoader::LoadObjFile(std::string path, std::vector<SubmeshDesc>& outSubmeshes,
                                 std::unordered_map<std::string, MaterialDesc>& outMaterials)
{
    EASY_FUNCTION();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    Logger::DebugLog("Started to load Obj File");
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), "Assets/"))
    {
        throw std::runtime_error(err);
    }
    Logger::DebugLog("Succesfully loaded obj file");

    for (const auto& shape : shapes)
    {
        EASY_BLOCK("Submesh");
        LoadShape(outSubmeshes, attrib, shape, materials);
        Logger::DebugLog("Succesfully loaded a shape");
        EASY_END_BLOCK;
    }

    for (const auto& material : materials)
    {
        const auto data = LoadMaterial(material);
        outMaterials[data.name] = data;
        Logger::DebugLog("Succesfully loaded a material");
    }
}
 */