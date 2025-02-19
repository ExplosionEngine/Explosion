//
// Created by johnk on 2023/10/11.
//

#include <unordered_map>

#include <Core/Uri.h>
#include <Common/String.h>
#include <Common/Debug.h>

namespace Core {
    Uri::Uri() = default;

    Uri::Uri(std::string inValue)
        : value(std::move(inValue))
    {
    }

    const std::string& Uri::Str() const
    {
        return value;
    }

    UriProtocol Uri::Protocol() const // NOLINT
    {
        static const std::unordered_map<std::string, UriProtocol> protocolMap = {
            { "file", UriProtocol::file },
            { "asset", UriProtocol::asset }
        };

        const auto splits = Common::StringUtils::Split(value, "://");
        if (splits.size() != 2) {
            return UriProtocol::max;
        }
        return protocolMap.at(splits[0]);
    }

    std::string Uri::Content() const
    {
        return Common::StringUtils::AfterFirst(value, "://");
    }

    bool Uri::Empty() const
    {
        return value.empty();
    }

    bool Uri::operator==(const Uri& rhs) const
    {
        return value == rhs.value;
    }

    FileUriParser::FileUriParser(const Uri& inUri)
        : content(inUri.Content())
    {
        Assert(inUri.Protocol() == UriProtocol::file);
    }

    bool FileUriParser::IsEngineFile() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Engine/.*)");
    }

    bool FileUriParser::IsGameFile() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Game/.*)");
    }

    bool FileUriParser::IsRegularFile() const
    {
        return !IsEngineFile() && !IsGameFile();
    }

    Common::Path FileUriParser::Parse() const
    {
        Common::Path path;
        if (IsEngineFile()) {
            path = Paths::EngineRootDir() / Common::StringUtils::AfterFirst(content, "Engine/");
        } else if (IsGameFile()) {
            path = Paths::GameRootDir() / Common::StringUtils::AfterFirst(content, "Game/");
        } else {
            path = content;
        }
        return path;
    }

    AssetUriParser::AssetUriParser(const Uri& inUri)
        : content(inUri.Content())
    {
        Assert(inUri.Protocol() == UriProtocol::asset);
    }

    bool AssetUriParser::IsEngineAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Engine/.*)");
    }

    bool AssetUriParser::IsGameAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Game/.*)");
    }

    bool AssetUriParser::IsEnginePluginAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Engine/Plugin/.*)");
    }

    bool AssetUriParser::IsGamePluginAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Game/Plugin/.*)");
    }

    Common::Path AssetUriParser::Parse() const
    {
        Common::Path path;
#if BUILD_TEST
        if (IsEngineTestAsset()) {
            path = Paths::EngineTest() / Common::StringUtils::AfterFirst(content, "Engine/Test/");
        } else if (IsEnginePluginAsset()) {
#else
        if (IsEnginePluginAsset()) {
#endif
            const std::string pathWithPluginName = Common::StringUtils::AfterFirst(content, "Engine/Plugin/");
            path = Paths::EnginePluginAssetDir(Common::StringUtils::BeforeFirst(pathWithPluginName, "/")) / Common::StringUtils::AfterFirst(pathWithPluginName, "/");
        } else if (IsGamePluginAsset()) {
            const std::string pathWithPluginName = Common::StringUtils::AfterFirst(content, "Game/Plugin/");
            path = Paths::GamePluginAssetDir(Common::StringUtils::BeforeFirst(pathWithPluginName, "/")) / Common::StringUtils::AfterFirst(pathWithPluginName, "/");
        } else if (IsEngineAsset()) {
            path = Paths::EngineAssetDir() / Common::StringUtils::AfterFirst(content, "Engine/");
        } else if (IsGameAsset()) {
            path = Paths::GameAssetDir() / Common::StringUtils::AfterFirst(content, "Game/");
        } else {
            AssertWithReason(false, "bad asset uri");
        }
        return path + ".expa";
    }

#if BUILD_TEST
    bool AssetUriParser::IsEngineTestAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Engine/Test/.*)");
    }
#endif
}
