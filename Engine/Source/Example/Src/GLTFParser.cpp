//
// Created by 兰俊康 on 2023/3/9.
//


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include <GLTFParser.h>

namespace Example {
    glm::mat4 Node::localMatrix()
    {
        return matrix;
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
        for (auto* mesh : meshes) {
            delete mesh;
        }

        for (auto* child : children) {
            delete child;
        }
    }

    void Model::LoadFromFile(const std::string path)
    {
        Assimp::Importer importer;
        const auto* scene = importer.ReadFile(path, aiProcess_Triangulate);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "failed to load gltf file: " << path << std::endl;
            exit(-1);
        }

        for (int i = 0; i < scene->mRootNode->mNumChildren; i++) {
            LoadNode(scene, scene->mRootNode->mChildren[i], nullptr);
        }
    }

    void Model::LoadNode(const aiScene* scene, aiNode* node, Node* parent)
    {
        auto* mNode = new Node {};
        mNode->parent = parent;
        mNode->matrix = glm::mat4(1.0f);

        mNode->matrix = glm::transpose(glm::make_mat4x4(&node->mTransformation.a1));

        for (int m = 0; m < node->mNumChildren; m++) {
            LoadNode(scene, node->mChildren[m], mNode);
        }

        for (int k = 0; k < node->mNumMeshes; k++) {
            auto* mesh = scene->mMeshes[node->mMeshes[k]];

            auto indexStart = static_cast<uint32_t>(raw_index_buffer.size());
            auto vertexStart = static_cast<uint32_t>(raw_vertex_buffer.size());
            uint32_t indexCount = 0;
            uint32_t vertexCount = 0;

            glm::vec3 posMin {};
            glm::vec3 posMax {};

            // node vertices
            {
                vertexCount = static_cast<uint32_t>(mesh->mNumVertices);

                for (size_t i = 0; i < vertexCount; i++) {
                    Vertex vert {};
                    glm::vec3 vector;

                    // pos
                    vector.x = mesh->mVertices[i].x;
                    vector.y = mesh->mVertices[i].y;
                    vector.z = mesh->mVertices[i].z;
                    vert.pos = vector;

                    // normal
                    if (mesh->HasNormals()) {
                        vector.x = mesh->mNormals[i].x;
                        vector.y = mesh->mNormals[i].y;
                        vector.z = mesh->mNormals[i].z;
                        vert.normal = vector;
                    }

                    if (mesh->mTextureCoords[0]) {
                        vert.uv.x = mesh->mTextureCoords[0][i].x;
                        vert.uv.y = mesh->mTextureCoords[0][i].y;
                    }

                    if (mesh->HasVertexColors(0)) {
                        vert.color.r = mesh->mColors[0][i].r;
                        vert.color.g = mesh->mColors[0][i].g;
                        vert.color.b = mesh->mColors[0][i].b;
                        vert.color.a = mesh->mColors[0][i].a;
                    }

                    raw_vertex_buffer.emplace_back(vert);
                }
            }

            // node indices
            {
                for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                    const auto& face = mesh->mFaces[i];
                    for (unsigned int j = 0; j < face.mNumIndices; j++) {
                        raw_index_buffer.emplace_back(face.mIndices[j]);
                        indexCount++;
                    }
                }

            }

            auto* mMesh = new Mesh(indexStart, indexCount, vertexStart, vertexCount, materialDatas.back());
            mMesh->setDimensions(posMin, posMax);
            mNode->meshes.emplace_back(mMesh);
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
        for (auto* textureData : textureDatas) {
            delete textureData;
        }

        for (auto* materialData : materialDatas) {
            delete materialData;
        }

        for (auto* node : nodes) {
            delete node;
        }

        delete emptyTexture;
    }
}