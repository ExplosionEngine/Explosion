//
// Created by Junkang on 2023/3/9.
//
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/postprocess.h>

#include <GLTFParser.h>

using namespace Common;

FMat4x4 Node::LocalMatrix() const
{
    return matrix;
}

FMat4x4 Node::GetMatrix()
{
    FMat4x4 m = LocalMatrix();
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

    const bool success = scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE);
    Assert(success);

    directory = path.substr(0, path.find_last_of('/'));

    Assert(scene->HasMaterials() && scene->HasMeshes());

    LoadMaterials(scene);

    SharedPtr<Node> parentForRoot = nullptr;
    rootNode = SharedPtr<Node>(new Node(parentForRoot));
    for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; i++) {
        LoadNode(scene, scene->mRootNode->mChildren[i], rootNode);
    }
}

void Model::LoadMaterials(const aiScene* scene)
{
    const bool fromEmbedded = scene->HasTextures();

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        const auto* material = scene->mMaterials[i];

        SharedPtr mMaterial = new MaterialData();
        mMaterial->baseColorTexture = LoadMaterialTexture(scene, material, aiTextureType_DIFFUSE, fromEmbedded);
        mMaterial->normalTexture = LoadMaterialTexture(scene, material, aiTextureType_NORMALS, fromEmbedded);

        materialDatas.emplace_back(std::move(mMaterial));
    }
}

UniquePtr<TextureData> Model::LoadMaterialTexture(const aiScene* scene, const aiMaterial* mat, aiTextureType type, bool fromEmbedded) const
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

    UniquePtr mTexData = new TextureData();

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
        mTexData->buffer = std::vector(static_cast<unsigned char*>(data), static_cast<unsigned char*>(data) + (width * height * comp));
    }

    mTexData->width = width;
    mTexData->height = height;
    mTexData->component = 4;

    stbi_image_free(data);

    return mTexData;
}

void Model::LoadNode(const aiScene* scene, aiNode* node, SharedPtr<Node>& parent)
{
    SharedPtr mNode = new Node(parent);

    // the layout of matrix in assimp math lib is row major, which is equal to expolsion math lib
    memcpy(mNode->matrix.data, &node->mTransformation.a1, sizeof(Common::FMat4x4));

    for (unsigned int m = 0; m < node->mNumChildren; m++) {
        LoadNode(scene, node->mChildren[m], mNode);
    }

    const auto localMatrix = mNode->GetMatrix();
    for (unsigned int k = 0; k < node->mNumMeshes; k++) {
        auto* mesh = scene->mMeshes[node->mMeshes[k]];

        auto indexStart = static_cast<uint32_t>(rawIndBuffer.size());
        auto vertexStart = static_cast<uint32_t>(rawVertBuffer.size());
        uint32_t indexCount = 0;
        uint32_t vertexCount = 0;

        // node indices
        {
            auto vertexOffset = static_cast<uint32_t>(rawVertBuffer.size());

            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                const auto& face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    rawIndBuffer.emplace_back(face.mIndices[j] + vertexOffset);
                    indexCount++;
                }
            }
        }

        // node vertices
        {
            vertexCount = mesh->mNumVertices;

            for (uint32_t i = 0; i < vertexCount; i++) {
                Vertex vert {};

                vert.pos.x = mesh->mVertices[i].x;
                vert.pos.y = mesh->mVertices[i].y;
                vert.pos.z = mesh->mVertices[i].z;
                vert.pos.w = 1.0f;

                if (mesh->HasNormals()) {
                    vert.normal.x = mesh->mNormals[i].x;
                    vert.normal.y = mesh->mNormals[i].y;
                    vert.normal.z = mesh->mNormals[i].z;
                }

                if (mesh->mTextureCoords[0]) {
                    vert.uv.x = mesh->mTextureCoords[0][i].x;
                    vert.uv.y = mesh->mTextureCoords[0][i].y;
                }

                if (mesh->HasVertexColors(0)) {
                    vert.color.x = mesh->mColors[0][i].r;
                    vert.color.y = mesh->mColors[0][i].g;
                    vert.color.z = mesh->mColors[0][i].b;
                    vert.color.w = mesh->mColors[0][i].a;
                } else {
                    vert.color = Common::FVec4(1.0f);
                }

                // pre transform vert
                vert.pos = localMatrix * vert.pos;
                vert.normal = (localMatrix.SubMatrix<3, 3>() * vert.normal).Normalized();

                rawVertBuffer.emplace_back(vert);
            }
        }

        UniquePtr<Mesh> mMesh = new Mesh(indexStart, indexCount, vertexStart, vertexCount, materialDatas[mesh->mMaterialIndex]);

        // cache meshes in model instead of node, for the convenience of reading them
        meshes.emplace_back(std::move(mMesh));
    }

    parent->children.emplace_back(std::move(mNode));
}
