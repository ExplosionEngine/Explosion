//
// Created by johnk on 2023/4/28.
//

#include <gtest/gtest.h>

#include <assimp/Importer.hpp>
#include <assimp/Scene.h>

TEST(AssimpTest, BasicTest)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("TestRes/box.fbx", 0);
    ASSERT_NE(scene, nullptr);
    ASSERT_EQ(scene->mNumMeshes, 1);
}
