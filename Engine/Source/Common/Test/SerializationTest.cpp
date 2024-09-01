//
// Created by johnk on 2023/7/13.
//

#include <filesystem>
#include <Common/Memory.h>

#include <Test/Test.h>
#include <Common/Serialization.h>

using namespace Common;

template <typename T>
void PerformTypedSerializationTest(const T& inValue, const std::function<bool(const T&, const T&)>& inCustomCompareFunc = {})
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.TypedSerializationTest";
    std::filesystem::create_directories(fileName.parent_path());

    {
        BinaryFileSerializeStream stream(fileName.string());
        Serialize<T>(stream, inValue);
    }

    {
        T value;
        BinaryFileDeserializeStream stream(fileName.string());
        Deserialize<T>(stream, value);

        if (inCustomCompareFunc) {
            ASSERT_TRUE(inCustomCompareFunc(inValue, value));
        } else {
            ASSERT_EQ(inValue, value);
        }
    }
}

TEST(SerializationTest, FileStreamTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.FileStreamTest.bin";
    std::filesystem::create_directories(fileName.parent_path());

    {
        const uint32_t value = 5; // NOLINT

        BinaryFileSerializeStream stream(fileName.string());
        stream.Write(&value, sizeof(uint32_t));
    }

    {
        uint32_t value;

        BinaryFileDeserializeStream stream(fileName.string());
        stream.Read(&value, sizeof(uint32_t));
        ASSERT_EQ(value, 5);
    }
}

TEST(SerializationTest, TypedSerializationTest)
{
    PerformTypedSerializationTest<bool>(false);
    PerformTypedSerializationTest<bool>(true);
    PerformTypedSerializationTest<int8_t>(-1);
    PerformTypedSerializationTest<uint8_t>(1);
    PerformTypedSerializationTest<int16_t>(-2);
    PerformTypedSerializationTest<uint16_t>(2);
    PerformTypedSerializationTest<int32_t>(-3);
    PerformTypedSerializationTest<uint32_t>(3);
    PerformTypedSerializationTest<int64_t>(-4);
    PerformTypedSerializationTest<uint64_t>(4);
    PerformTypedSerializationTest<float>(5.0f);
    PerformTypedSerializationTest<double>(6.0);
    PerformTypedSerializationTest<std::string>("hello");
    PerformTypedSerializationTest<std::optional<int>>({});
    PerformTypedSerializationTest<std::optional<int>>(15);
    PerformTypedSerializationTest<std::pair<int, bool>>({ 1, false });
    PerformTypedSerializationTest<std::vector<int>>({ 1, 2, 3 }, [](const std::vector<int>& inLhs, const std::vector<int>& inRhs) -> bool {
        if (inLhs.size() != inRhs.size()) {
            return false;
        }
        for (auto i = 0; i < inLhs.size(); i++) {
            if (inLhs[i] != inRhs[i]) {
                return false;
            }
        }
        return true;
    });
    PerformTypedSerializationTest<std::unordered_set<int>>({ 1, 2, 3 }, [](const std::unordered_set<int>& inLhs, const std::unordered_set<int>& inRhs) -> bool {
        if (inLhs.size() != inRhs.size()) {
            return false;
        }
        for (const auto& element : inLhs) {
            if (!inRhs.contains(element)) {
                return false;
            }
        }
        return true;
    });
    PerformTypedSerializationTest<std::unordered_map<int, bool>>({ { 1, false }, { 2, true } }, [](const std::unordered_map<int, bool>& inLhs, const std::unordered_map<int, bool>& inRhs) -> bool {
        if (inLhs.size() != inRhs.size()) {
            return false;
        }
        for (const auto& [key, value] : inLhs) {
            if (!inRhs.contains(key)) {
                return false;
            }
            if (inRhs.at(key) != value) {
                return false;
            }
        }
        return true;
    });
}

TEST(SerializationTest, JsonSerializeTest)
{
    // TODO
}
