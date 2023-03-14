//
// Created by 兰俊康 on 2023/3/9.
//


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include <GLTFParser.h>

namespace Example {
    Mesh::~Mesh()
    {
        for (auto* primitive : primitives) {
            delete primitive;
        }
    }

    glm::mat4 Node::localMatrix()
    {
        return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
    }

    glm::mat4 Node::getMatrix()
    {
        glm::mat4 m = localMatrix();
        Node* p = parent;
        while (p != nullptr) {
            m = p->localMatrix() * m;
            p = p->parent;
        }
        return m;
    }

    Node::~Node()
    {
        if (mesh != nullptr) {
            delete mesh;
        }
        for (auto& child : children) {
            delete child;
        }
    }

    void Model::LoadFromFile(std::string path)
    {
        tinygltf::TinyGLTF gltfContext;
        tinygltf::Model gltfModel;

        std::string warn, error;
        bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warn, path);

        if (fileLoaded) {
            LoadImages(gltfModel);
            LoadMaterials(gltfModel);

            const auto& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
            for (size_t i = 0; i < scene.nodes.size(); i++) {
                auto& node = gltfModel.nodes[scene.nodes[i]];
                LoadNode(gltfModel, node, nullptr, scene.nodes[i]);
            }
        } else {
            std::cerr << "failed to load gltf file: " << path << std::endl;
            exit(-1);
        }

        //// Precalculations
        for (auto* node : linearNodes) {
            if (node->mesh != nullptr) {
                const auto localMatrix = node->localMatrix();
                for (auto* primitive : node->mesh->primitives) {
                    for (uint32_t i = 0; i < primitive->vertexCount; i ++) {
                        Vertex& vertex = raw_vertex_buffer[primitive->firstIndex + i];

                        // Pre transform vertex postion by node hierarchy
                        vertex.pos = glm::vec3(localMatrix * glm::vec4(vertex.pos, 1.0f));
                        vertex.normal = glm::normalize(glm::mat3(localMatrix) * vertex.normal);

                        // flip y
                        vertex.pos.y *= -1.0f;
                        vertex.normal.y *= -1.0f;

                        // premultiply vertex colors with material base color
                        vertex.color = primitive->materialData->baseColorFactor * vertex.color;
                    }
                }
            }
        }
    }

    void Model::LoadImages(tinygltf::Model& model)
    {
        for (auto& image : model.images) {
            auto texture = new TextureData();

            texture->size = image.image.size();
            texture->pixels = image.image.data();
            texture->height = image.height;
            texture->width = image.width;
            texture->mipLevels = static_cast<uint32_t>(floor(log2(std::max(image.width, image.height))) + 1.0);

            textureDatas.emplace_back(texture);
        }
    }

    void Model::LoadMaterials(tinygltf::Model& model)
    {
        for (auto& mat : model.materials) {
            auto* material = new MaterialData();

            if (mat.values.find("baseColorTexture") != mat.values.end()) {
                material->baseColorTexture = GetTexture(model.textures[mat.values["baseColorTexture"].TextureIndex()].source);
            }

            if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
                material->metallicRoughnessTexture = GetTexture(model.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source);
            }

            if (mat.values.find("roughnessFactor") != mat.values.end()) {
                material->roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
            }
            if (mat.values.find("metallicFactor") != mat.values.end()) {
                material->metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
            }
            if (mat.values.find("baseColorFactor") != mat.values.end()) {
                material->baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
            }
            if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
                material->normalTexture = GetTexture(model.textures[mat.additionalValues["normalTexture"].TextureIndex()].source);
            } else {
                material->normalTexture = emptyTexture;
            }
            if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
                material->emissiveTexture = GetTexture(model.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source);
            }
            if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
                material->occlusionTexture = GetTexture(model.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source);
            }

            if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
                material->alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
            }

            materialDatas.emplace_back(material);
        }

        // Push a default material at the end of the list for meshes with no material assigned
        auto* defaultMaterial = new MaterialData();
        materialDatas.emplace_back(defaultMaterial);
    }

    void Model::LoadNode(tinygltf::Model& model, tinygltf::Node& node, Node* parent, uint32_t nodeIndex)
    {
        auto* mNode = new Node {};
        mNode->index = nodeIndex;
        mNode->parent = parent;
        mNode->matrix = glm::mat4(1.0f);

        auto translation = glm::vec3(0.0f);
        if (node.translation.size() == 3) {
            translation = glm::make_vec3(node.translation.data());
            mNode->translation = translation;
        }
        auto rotation = glm::mat4(1.0f);
        if (node.rotation.size() == 4) {
            glm::quat q = glm::make_quat(node.rotation.data());
            mNode->rotation = glm::mat4(q);
        }
        auto scale = glm::vec3(1.0f);
        if (node.scale.size() == 3) {
            scale = glm::make_vec3(node.scale.data());
            mNode->scale = scale;
        }
        if (node.matrix.size() == 16) {
            mNode->matrix = glm::make_mat4x4(node.matrix.data());
        }

        if (!node.children.empty()) {
            for (auto child : node.children) {
                LoadNode(model, model.nodes[child], mNode, child);
            }
        }

        if (node.mesh > -1) {
           auto& mesh = model.meshes[node.mesh];
           auto* mMesh = new Mesh();
           mMesh->name = mesh.name;

           for (const auto& primitive : mesh.primitives) {
                if (primitive.indices < 0) {
                    continue;
                }

                auto indexStart = static_cast<uint32_t>(raw_index_buffer.size());
                auto vertexStart = static_cast<uint32_t>(raw_vertex_buffer.size());
                uint32_t indexCount = 0;
                uint32_t vertexCount = 0;

                glm::vec3 posMin {};
                glm::vec3 posMax {};

                // node vertices
                {
                    const float* bufferPos = nullptr;
                    const float* bufferNormals = nullptr;
                    const float* bufferTexCoords = nullptr;
                    const float* bufferColors = nullptr;
                    uint32_t numColorComponents;

                    // Position attribute is required
                    assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

                    const auto& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                    const auto& posView = model.bufferViews[posAccessor.bufferView];
                    bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
                    posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
                    posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

                    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                        const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                        const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
                        bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                    }

                    if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                        const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                        const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                        bufferTexCoords = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                    }

                    if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
                    {
                        const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
                        const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
                        // Color buffer are either of type vec3 or vec4
                        numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
                        bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
                    }

                    vertexCount = static_cast<uint32_t>(posAccessor.count);

                    for (size_t i = 0; i < posAccessor.count; i++) {
                        Vertex vert {};
                        vert.pos = glm::make_vec3(&bufferPos[i * 3]);
                        vert.normal = glm::normalize(bufferNormals != nullptr ? glm::make_vec3(&bufferNormals[i * 3]) : glm::vec3(0.0f));
                        vert.uv = bufferTexCoords != nullptr ? glm::make_vec2(&bufferTexCoords[i * 2]) : glm::vec2(0.0f);
                        if (bufferColors != nullptr) {
                            if (numColorComponents == 3) {
                                vert.color = glm::vec4(glm::make_vec3(&bufferColors[i * 3]), 1.0f);
                            } else if (numColorComponents == 4){
                                vert.color = glm::make_vec4(&bufferColors[i * 4]);
                            }
                        } else {
                            vert.color = glm::vec4(1.0f);
                        }

                        raw_vertex_buffer.emplace_back(vert);
                    }
                }

                // node indices
                {
                    const auto& accessor = model.accessors[primitive.indices];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[bufferView.buffer];

                    indexCount = static_cast<uint32_t>(accessor.count);

                    switch (accessor.componentType) {
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                            auto* buf = new uint32_t[accessor.count];
                            memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
                            for (size_t index = 0; index < accessor.count; index++) {
                                raw_index_buffer.emplace_back(buf[index] + vertexStart);
                            }
                            delete[] buf;
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                            auto* buf = new uint16_t[accessor.count];
                            memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
                            for (size_t index = 0; index < accessor.count; index++) {
                                raw_index_buffer.emplace_back(buf[index] + vertexStart);
                            }
                            delete[] buf;
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                            auto* buf = new uint8_t[accessor.count];
                            memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
                            for (size_t index = 0; index < accessor.count; index++) {
                                raw_index_buffer.emplace_back(buf[index] + vertexStart);
                            }
                            delete[] buf;
                            break;
                        }

                        default:
                            std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                            return;
                    }
                }

                auto* mPrimitive = new Primitive(indexStart, indexCount, vertexStart, vertexCount, primitive.material > -1 ? materialDatas[primitive.material] : materialDatas.back());
                mPrimitive->setDimensions(posMin, posMax);
                mMesh->primitives.emplace_back(mPrimitive);
           }
           mNode->mesh = mMesh;
        }

        if (parent != nullptr) {
            parent->children.emplace_back(mNode);
        } else {
            nodes.emplace_back(mNode);
        }

        linearNodes.push_back(mNode);
    }

    Model::~Model()
    {
        for (auto textureData : textureDatas) {
            delete textureData;
        }

        for (auto materialData : materialDatas) {
            delete materialData;
        }

        for (auto node : nodes) {
            delete node;
        }

        delete emptyTexture;
    }
}