//
// Created by 兰俊康 on 2023/3/9.
//

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Example {
    struct TextureData {
    public:
        uint32_t width { 0 };
        uint32_t height { 0 };
        uint32_t depth { 0 };
        uint32_t mipLevels { 1 };
        uint32_t arrayLayers { 1 };
        size_t   size { 0 };
        void*    pixels { nullptr };

        bool isValid() const { return pixels != nullptr; }
    };

    struct MaterialData
    {
        TextureData* baseColorTexture;
        TextureData* metallicRoughnessTexture;
        TextureData* normalTexture;
        TextureData* occlusionTexture;
        TextureData* emissiveTexture;

        float alphaCutoff = 1.0f;
        float metallicFactor = 1.0f;
        float roughnessFactor = 1.0f;
        glm::vec4 baseColorFactor = glm::vec4(1.0f);
    };

    struct Vertex {
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec4 color;
        glm::vec3 normal;
    };

    // In assimp, a mesh represents a geometry or model with a single material.
    struct Mesh {
        std::string name;

        uint32_t firstIndex;
        uint32_t indexCount;
        uint32_t firstVertex;
        uint32_t vertexCount;

        MaterialData* materialData;

        struct Dimensions {
            glm::vec3 min = glm::vec3(FLT_MAX);
            glm::vec3 max = glm::vec3(-FLT_MAX);
            glm::vec3 size;
            glm::vec3 center;
            float radius;
        } dimensions;

        void setDimensions(glm::vec3 min, glm::vec3 max) {
            dimensions.min = min;
            dimensions.max = max;
            dimensions.size = max - min;
            dimensions.center = (min + max) / 2.0f;
            dimensions.radius = glm::distance(min, max) / 2.0f;
        }

        Mesh(uint32_t firstIndex, uint32_t indexCount, uint32_t firstVertex, uint32_t vertexCount, MaterialData* material)
            : firstIndex(firstIndex),
            indexCount(indexCount),
            firstVertex(firstVertex),
            vertexCount(vertexCount),
            materialData(material)
                {};

        ~Mesh() {
            if (materialData != nullptr) {
                delete materialData;
            }
        }
    };

    struct Node {
        Node* parent;
        std::vector<Node*> children;

        glm::mat4 matrix;
        std::vector<Mesh*> meshes;

        glm::vec3 translation{};
        glm::vec3 scale{ 1.0f };
        glm::quat rotation{};
        glm::mat4 localMatrix();
        glm::mat4 getMatrix();
        ~Node();
    };


    class Model {
    public:
        Model() = default;
        ~Model();

        void LoadFromFile(std::string path);
        void LoadNode(const aiScene* scene, aiNode* node, Node* parent);
//        void LoadImages(tinygltf::Model& model);
//        void LoadMaterials(tinygltf::Model& model);

    private:
        TextureData* GetTexture(uint32_t index)
        {
            if (index < textureDatas.size()) {
                return textureDatas[index];
            }
            return nullptr;
        }

        void CreateEmptyTexture()
        {
            emptyTexture->width = 1;
            emptyTexture->height = 1;
            emptyTexture->mipLevels = 1;
            emptyTexture->size = 4;
            emptyTexture->pixels = new unsigned char[emptyTexture->size];

            memset(emptyTexture->pixels, 0, emptyTexture->size);
        }

    public:
        std::vector<Node*> nodes;
        std::vector<Node*> linearNodes;

        std::vector<TextureData*> textureDatas;
        std::vector<MaterialData*> materialDatas;

        std::vector<uint32_t> raw_index_buffer;
        std::vector<Vertex> raw_vertex_buffer;
    private:

        TextureData* emptyTexture;

    };
}
