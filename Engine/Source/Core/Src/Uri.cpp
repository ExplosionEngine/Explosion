//
// Created by johnk on 2023/10/11.
//

#include <unordered_map>

#include <Core/Uri.h>
#include <Common/String.h>

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

    const std::string& Uri::Value() const
    {
        return value;
    }

    UriProtocol Uri::GetProtocal() const
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

    bool Uri::Empty() const
    {
        return value.empty();
    }
}
