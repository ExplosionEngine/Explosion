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

    Uri::Uri(const Uri& other) // NOLINT
        : value(other.value)
    {
    }

    Uri::Uri(Uri&& other) noexcept // NOLINT
        : value(std::move(other.value))
    {
    }

    Uri::~Uri() = default;

    Uri& Uri::operator=(const Uri& other) // NOLINT
    {
        value = other.value;
        return *this;
    }

    Uri& Uri::operator=(const std::string& inValue)
    {
        value = inValue;
        return *this;
    }

    Uri& Uri::operator=(Uri&& other) noexcept // NOLINT
    {
        value = std::move(other.value);
        return *this;
    }

    bool Uri::operator==(const Uri& rhs) const
    {
        return value == rhs.value;
    }

    const std::string& Uri::Str() const
    {
        return value;
    }

    UriProtocol Uri::Protocal() const
    {
        static std::unordered_map<std::string, UriProtocol> protocolMap = {
            { "asset", UriProtocol::asset }
        };

        auto splits = Common::StringUtils::Split(value, "://");
        if (splits.size() != 2) {
            return UriProtocol::max;
        }
        auto iter = protocolMap.find(splits[0]);
        return iter == protocolMap.end() ? UriProtocol::max : iter->second;
    }

    std::string Uri::Content() const
    {
        return Common::StringUtils::AfterFirst(value, "://");
    }

    bool Uri::Empty() const
    {
        return value.empty();
    }

    AssetUriParser::AssetUriParser(const Uri& inUri)
        : content(inUri.Content())
    {
        Assert(inUri.Protocal() == UriProtocol::asset);
    }

    bool AssetUriParser::IsEngineAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Engine/.*)");
    }

    bool AssetUriParser::IsProjectAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Project/.*)");
    }

    bool AssetUriParser::IsEnginePluginAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Engine/Plugin/.*)");
    }

    bool AssetUriParser::IsProjectPluginAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Project/Plugin/.*)");
    }

    std::filesystem::path AssetUriParser::AbsoluteFilePath() const
    {
        std::filesystem::path path;
#if BUILD_TEST
        if (IsEngineTestAsset()) {
            path = Paths::EngineTestPath() / Common::StringUtils::AfterFirst(content, "Engine/Test/");
        } else if (IsEnginePluginAsset()) {
#else
        if (IsEnginePluginAsset()) {
#endif
            std::string pathWithPluginName = Common::StringUtils::AfterFirst(content, "Engine/Plugin/");
            path = Paths::EnginePluginAssetPath(Common::StringUtils::BeforeFirst(pathWithPluginName, "/")) / Common::StringUtils::AfterFirst(pathWithPluginName, "/");
        } else if (IsProjectPluginAsset()) {
            std::string pathWithPluginName = Common::StringUtils::AfterFirst(content, "Project/Plugin/");
            path = Paths::ProjectPluginAssetPath(Common::StringUtils::BeforeFirst(pathWithPluginName, "/")) / Common::StringUtils::AfterFirst(pathWithPluginName, "/");
        } else if (IsEngineAsset()) {
            path = Paths::EngineAssetPath() / Common::StringUtils::AfterFirst(content, "Engine/");
        } else if (IsProjectAsset()) {
            path = Paths::ProjectAssetPath() / Common::StringUtils::AfterFirst(content, "Project/");
        } else {
            AssertWithReason(false, "bad asset uri");
        }
        return std::filesystem::absolute(path.concat(".expa"));
    }

#if BUILD_TEST
    bool AssetUriParser::IsEngineTestAsset() const
    {
        return Common::StringUtils::RegexMatch(content, R"(Engine/Test/.*)");
    }
#endif
}
