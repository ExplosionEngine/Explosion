//
// Created by Junkang on 2023/3/9.
//

#pragma once

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <Common/Memory.h>
#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>

struct TextureData {
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t  component = 0;
    std::vector<unsigned char> buffer;

    bool IsValid() const { return buffer.size() == (width * height * component); }
    uint32_t GetSize() const { return buffer.size(); }
};

struct MaterialData
{
    Common::UniqueRef<TextureData> baseColorTexture = nullptr;
    Common::UniqueRef<TextureData> normalTexture = nullptr;
};

struct Vertex {
    Common::FVec4 pos;
    Common::FVec2 uv;
    Common::FVec4 color;
    Common::FVec3 normal;
};

// In assimp, a mesh represents a geometry or model with a single material.
struct Mesh {
    std::string name;

    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t firstVertex;
    uint32_t vertexCount;

    Common::SharedRef<MaterialData> materialData;

    Mesh(uint32_t firstIndex, uint32_t indexCount, uint32_t firstVertex, uint32_t vertexCount, Common::SharedRef<MaterialData>& material)
        : firstIndex(firstIndex)
        , indexCount(indexCount)
        , firstVertex(firstVertex)
        , vertexCount(vertexCount)
        , materialData(material)
    {
    }

    // mustn`t delele material here, for different meshes can point to the same material
};

struct Node {
    explicit Node(Common::SharedRef<Node>& p)
        : parent(Common::WeakRef<Node>(p))
        , matrix(Common::FMat4x4Consts::identity)
    {
    }

    Common::WeakRef<Node> parent;
    std::vector<Common::SharedRef<Node>> children;

    Common::FMat4x4 matrix;

    Common::FMat4x4 LocalMatrix() const;
    Common::FMat4x4 GetMatrix();
};


class Model {
public:
    Model() = default;
    ~Model() = default;

    void LoadFromFile(const std::string& path);
    void LoadNode(const aiScene* scene, aiNode* node, Common::SharedRef<Node>& parent);
    void LoadMaterials(const aiScene* scene);

private:
    Common::UniqueRef<TextureData> LoadMaterialTexture(const aiScene* scene, const aiMaterial* mat, aiTextureType type, bool fromEmbedded) const;

    void CreateEmptyTexture() const
    {
        emptyTexture->width = 1;
        emptyTexture->height = 1;
        emptyTexture->component = 4;
        emptyTexture->buffer = std::vector<unsigned char> { 0, 0, 0, 0 };
    }

public:
    Common::SharedRef<Node> rootNode;
    std::vector<Common::SharedRef<MaterialData>> materialDatas;
    std::vector<uint32_t> rawIndBuffer;
    std::vector<Vertex> rawVertBuffer;
    std::vector<Common::UniqueRef<Mesh>> meshes;
    std::string directory;

private:
    Common::UniqueRef<TextureData> emptyTexture = nullptr;
};
