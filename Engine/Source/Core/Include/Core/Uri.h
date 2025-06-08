//
// Created by johnk on 2023/10/11.
//

#pragma once

#include <Core/Paths.h>
#include <Common/Serialization.h>

namespace Core {
    enum class UriProtocol : uint8_t {
        file,
        asset,
        max
    };

    class CORE_API Uri {
    public:
        Uri();
        Uri(std::string inValue); // NOLINT

        const std::string& Str() const;
        UriProtocol Protocol() const;
        std::string Content() const;
        bool Empty() const;
        bool operator==(const Uri& rhs) const;

    private:
        std::string value;
    };

    class CORE_API FileUriParser {
    public:
        explicit FileUriParser(const Uri& inUri);
        Common::Path Parse() const;

    private:
        std::string content;
    };

    class CORE_API AssetUriParser {
    public:
        explicit AssetUriParser(const Uri& inUri);
        Common::Path Parse() const;

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
        static constexpr size_t typeId = HashUtils::StrCrc32("Core::Uri");

        static size_t Serialize(BinarySerializeStream& stream, const Core::Uri& value)
        {
            return Serializer<std::string>::Serialize(stream, value.Str());
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Core::Uri& value)
        {
            std::string str;
            const auto deserialized = Serializer<std::string>::Deserialize(stream, str);
            value = str;
            return deserialized;
        }
    };
}
