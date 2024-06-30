//
// Created by johnk on 2024/6/30.
//

#include <filesystem>

#include <gtest/gtest.h>

#include <GameObjectTest.h>

TEST(GameObjectTest, SerializeTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Runtime/GameObjectTest.SerializeTest.bin";
    std::filesystem::create_directories(fileName.parent_path());

    const auto& clazz = Mirror::Class::Get<GameObject>();
    {
        GameObject gameObject(nullptr, "testGameObject");
        gameObject.Emplace<Position>(1, 2);
        gameObject.Emplace<Velocity>(3, 4);

        Common::BinaryFileSerializeStream stream(fileName.string());
        Mirror::Any ref = std::ref(gameObject);
        clazz.Serialize(stream, &ref);
    }
    {
        GameObject gameObject;
        Common::BinaryFileDeserializeStream stream(fileName.string());
        Mirror::Any ref = std::ref(gameObject);
        clazz.Deserailize(stream, &ref);

        auto* position = gameObject.Get<Position>();
        ASSERT_NE(position, nullptr);
        ASSERT_EQ(position->x, 1);
        ASSERT_EQ(position->y, 2);

        auto* velocity = gameObject.Get<Velocity>();
        ASSERT_NE(velocity, nullptr);
        ASSERT_EQ(velocity->x, 3);
        ASSERT_EQ(velocity->y, 4);
    }
}
