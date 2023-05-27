//
// Created by Junkang on 2023/3/9.
//


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "GLTFParser.h"

using namespace Common;

glm::mat4 Node::LocalMatrix() const
{
    return matrix;
}

glm::mat4 Node::GetMatrix()
{
    glm::mat4 m = LocalMatrix();
    auto& p = parent;
    while (p.Lock().Get() != nullptr) {
        m = p.Lock()->LocalMatrix() * m;
        p = p.Lock()->parent;
    }
    return m;
}

void Model::LoadFromFile(const std::string& path)
{
    Assimp::Importer importer;
    const auto* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "failed to load gltf file: " << path << std::endl;
        exit(-1);
    }

    directory = path.substr(0, path.find_last_of('/'));

    assert(scene->HasMaterials() && scene->HasMeshes());

    LoadMaterials(scene);

    SharedRef<Node> parentForRoot = nullptr;
    rootNode = SharedRef<Node>(new Node(parentForRoot));
    for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; i++) {
        LoadNode(scene, scene->mRootNode->mChildren[i], rootNode);
    }
}

void Model::LoadMaterials(const aiScene* scene)
{
    bool fromEmbedded = scene->HasTextures();

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        auto* material = scene->mMaterials[i];

        SharedRef<MaterialData> mMaterial = new MaterialData();
        mMaterial->baseColorTexture = LoadMaterialTexture(scene, material, aiTextureType_DIFFUSE, fromEmbedded);
        mMaterial->normalTexture = LoadMaterialTexture(scene, material, aiTextureType_NORMALS, fromEmbedded);

        materialDatas.emplace_back(std::move(mMaterial));
    }
}

UniqueRef<TextureData> Model::LoadMaterialTexture(const aiScene* scene, const aiMaterial* mat, aiTextureType type, bool fromEmbedded) const
{
    if (mat->GetTextureCount(type) == 0) {
        return nullptr;
    }

    // though a material can has multiple textures of one type, here we get the first texture of each type
    aiString fileName;
    mat->GetTexture(type, 0, &fileName);

    void* data;
    int width;
    int height;
    int comp;
    if (fromEmbedded) {
        const auto* texData = scene->GetEmbeddedTexture(fileName.C_Str());
        data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(texData->pcData), texData->mWidth, &width, &height, &comp,0);
    } else {
        auto filePath = directory + '/' + std::string(fileName.C_Str());
        data = stbi_load(filePath.c_str(), &width, &height, &comp,0);
    }

    UniqueRef<TextureData> mTexData = new TextureData();

    // Most device don`t support RGB only on Vulkan, so convert if necessary
    if (comp == 3) {
        mTexData->buffer.resize(width * height * 4);
        auto* rgba = mTexData->buffer.data();
        auto* rgb = static_cast<unsigned char*>(data);
        for (uint32_t i = 0; i < width * height; i++) {
            for (uint32_t j = 0; j < 3; j++) {
                rgba[j] = rgb[j];
            }
            rgba += 4;
            rgb += 3;
        }
    } else {
        mTexData->buffer = std::vector<unsigned char> (static_cast<unsigned char*>(data), static_cast<unsigned char*>(data) + (width * height * comp));
    }

    mTexData->width = width;
    mTexData->height = height;
    mTexData->component = 4;

    stbi_image_free(data);

    return mTexData;
}

void Model::LoadNode(const aiScene* scene, aiNode* node, SharedRef<Node>& parent)
{
    SharedRef<Node> mNode = new Node(parent);
    mNode->matrix = glm::transpose(glm::make_mat4x4(&node->mTransformation.a1));

    for (unsigned int m = 0; m < node->mNumChildren; m++) {
        LoadNode(scene, node->mChildren[m], mNode);
    }

    const auto localMatrix = mNode->GetMatrix();
    for (unsigned int k = 0; k < node->mNumMeshes; k++) {
        auto* mesh = scene->mMeshes[node->mMeshes[k]];

        auto indexStart = static_cast<uint32_t>(raw_index_buffer.size());
        auto vertexStart = static_cast<uint32_t>(raw_vertex_buffer.size());
        uint32_t indexCount = 0;
        uint32_t vertexCount = 0;

        glm::vec3 posMin {};
        glm::vec3 posMax {};

        // node indices
        {
            auto indexOffset = static_cast<uint32_t>(raw_vertex_buffer.size());

            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                const auto& face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    raw_index_buffer.emplace_back(face.mIndices[j] + indexOffset);
                    indexCount++;
                }
            }
        }

        // node vertices
        {
            vertexCount = static_cast<uint32_t>(mesh->mNumVertices);

            for (uint32_t i = 0; i < vertexCount; i++) {
                Vertex vert {};
                glm::vec3 vector;

                // pos
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;

                vert.pos = glm::vec4(vector, 1.0f);

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
                } else {
                    vert.color = glm::vec4(1.0f);
                }

                // pre transform vert
                vert.pos = localMatrix * vert.pos;
                vert.normal = glm::normalize(glm::mat3(localMatrix) * vert.normal);

                raw_vertex_buffer.emplace_back(vert);
            }
        }

        UniqueRef<Mesh> mMesh = new Mesh(indexStart, indexCount, vertexStart, vertexCount, materialDatas[mesh->mMaterialIndex]);
        mMesh->SetDimensions(
                 glm::make_vec3(&mesh->mAABB.mMin.x),
                 glm::make_vec3(&mesh->mAABB.mMax.x)
                 );

        // cache meshes in model instead of node, for the convenience of reading them
        meshes.emplace_back(std::move(mMesh));
    }

    parent->children.emplace_back(std::move(mNode));
}
