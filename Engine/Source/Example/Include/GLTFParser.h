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

namespace Example {
    class TextureData {
    public:
        uint32_t width {0};
        uint32_t height {0};
        uint32_t depth {0};
        uint32_t mipLevels {1};
        uint32_t arrayLayers {1};
        size_t   size {0};
        void*    pixels {nullptr};

        TextureData() = default;
        ~TextureData()
        {
            if (pixels)
            {
                free(pixels);
            }
        }
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

    struct Primitive {
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

        Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t firstVertex, uint32_t vertexCount, MaterialData* material)
            : firstIndex(firstIndex),
            indexCount(indexCount),
            firstVertex(firstVertex),
            vertexCount(vertexCount),
            materialData(material)
                {};
    };

    struct Mesh {
        std::string name;

        std::vector<Primitive*> primitives;

        ~Mesh();
    };

    struct Node {
        Node* parent;
        std::vector<Node*> children;

        uint32_t index;
        glm::mat4 matrix;
        Mesh* mesh;

        glm::vec3 translation{};
        glm::vec3 scale{ 1.0f };
        glm::quat rotation{};
        glm::mat4 localMatrix();
        glm::mat4 getMatrix();
        ~Node();
    };


    class Model {
    public:
        Model() {};
        ~Model();

        void LoadFromFile(std::string path);
        void LoadNode(tinygltf::Model& model, tinygltf::Node& node, Node* parent, uint32_t nodeIndex);
        void LoadImages(tinygltf::Model& model);
        void LoadMaterials(tinygltf::Model& model);

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
