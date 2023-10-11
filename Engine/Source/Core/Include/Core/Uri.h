//
// Created by johnk on 2023/10/11.
//

#pragma once

#include <Core/Paths.h>

namespace Core {
    enum class UriProtocol {
        asset,
        max
    };

    class Uri {
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

        const std::string& Value() const;
        UriProtocol GetProtocal() const;
        bool Empty() const;

    private:
        std::string value;
    };

    class AssetUriParser {
    public:
        AssetUriParser(const Uri& inUri);
        bool IsEngineAsset() const;
        bool IsGameAsset() const;
        bool IsPluginAsset() const;
        std::filesystem::path AbsoluteFilePath() const;

    private:
        const Uri& uri;
    };
}

namespace std {
    template <>
    struct hash<Core::Uri> {
        size_t operator()(const Core::Uri& uri) const
        {
            return hash<std::string>{}(uri.Value());
        }
    };
}
