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

    Common::Path FileUriParser::Parse() const
    {
        return Paths::Translate(content);
    }

    AssetUriParser::AssetUriParser(const Uri& inUri)
        : content(inUri.Content())
    {
        Assert(inUri.Protocol() == UriProtocol::asset);
    }

    Common::Path AssetUriParser::Parse() const
    {
        return Paths::TranslateAsset(content) + ".expa";
    }
}
