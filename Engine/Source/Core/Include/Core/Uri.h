//
// Created by johnk on 2023/10/11.
//

#pragma once

#include <Core/Paths.h>
#include <Common/Serialization.h>

namespace Core {
    enum class UriProtocol {
        asset,
        max
    };

    class CORE_API Uri {
    public:
        Uri();
        explicit Uri(std::string inValue);
        Uri(const Uri& other);
        Uri(Uri&& other) noexcept;
        ~Uri();
        Uri& operator=(const Uri& other);
        Uri& operator=(const std::string& inValue);
        Uri& operator=(Uri&& other) noexcept;
        bool operator==(const Uri& rhs) const;

        const std::string& Str() const;
        UriProtocol Protocal() const;
        std::string Content() const;
        bool Empty() const;

    private:
        std::string value;
    };

    class CORE_API AssetUriParser {
    public:
        explicit AssetUriParser(const Uri& inUri);
        bool IsEngineAsset() const;
        bool IsProjectAsset() const;
        bool IsEnginePluginAsset() const;
        bool IsProjectPluginAsset() const;
        std::filesystem::path AbsoluteFilePath() const;

#if BUILD_TEST
        bool IsEngineTestAsset() const;
#endif

    private:
        std::string content;
    };
}

namespace std { // NOLINT
    template <>
    struct hash<Core::Uri> {
        size_t operator()(const Core::Uri& uri) const noexcept
        {
            return hash<std::string>{}(uri.Str());
        }
    };
}

namespace Common { // NOLINT
    template <>
    struct Serializer<Core::Uri> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = HashUtils::StrCrc32("Core::Uri");

        static void Serialize(SerializeStream& stream, const Core::Uri& value)
        {
            TypeIdSerializer<std::string>::Serialize(stream);

            Serializer<std::string>::Serialize(stream, value.Str());
        }

        static bool Deserialize(DeserializeStream& stream, Core::Uri& value)
        {
            if (!TypeIdSerializer<std::string>::Deserialize(stream)) {
                return false;
            }

            std::string str;
            Serializer<std::string>::Deserialize(stream, str);
            value = str;
            return true;
        }
    };
}
