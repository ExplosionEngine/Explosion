#include "PassLoader.h"
#include "Explosion/Common/FileReader.h"
#include "Explosion/Common/Logger.h"
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace Explosion;

template <typename T>
struct TypeTraits {
    static constexpr auto VALUE = json::value_t::null;
};

template<>
struct TypeTraits<std::string> {
    static constexpr auto VALUE = json::value_t::string;
};

template<>
struct TypeTraits<uint32_t> {
    static constexpr auto VALUE = json::value_t::number_unsigned;
};

template<>
struct TypeTraits<int32_t> {
    static constexpr auto VALUE = json::value_t::number_integer;
};

template<>
struct TypeTraits<float> {
    static constexpr auto VALUE = json::value_t::number_float;
};

template<>
struct TypeTraits<bool> {
    static constexpr auto VALUE = json::value_t::boolean;
};

template <typename F>
static bool ForEachInArray(const json& v, const char* member, const F& f)
{
    auto iter = v.find(member);
    if (iter == v.end() || !iter.value().is_array()) {
        return false;
    }
    const json &root = iter.value();
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (!f(*it)) return false;
    }
    return true;
}

template<typename T>
static bool ParseProperty(T& ret, const json &o, const std::string& property)
{
    auto iter = o.find(property);
    if (iter == o.end()) {
        return false;
    }

    const json& val = iter.value();
    if (val.type() != TypeTraits<T>::VALUE) {
        return false;
    }
    ret = val.get<T>();
    return true;
}

static bool ParseTargets(json& root, GraphInfo& graph)
{
    bool ret = ForEachInArray(root, "targets", [&graph](const json& v) -> bool {
        TargetInfo target = {};
        ParseProperty(target.width, v, "width");
        ParseProperty(target.height, v, "height");
        graph.targets.emplace_back(target);
        return true;
    });

    Logger::Info("parse target success.");
    return true;
}

GraphInfo PassLoader::Load(const std::string& url)
{
    GraphInfo ret;

    auto data = FileReader::Read(url);
    json root = json::parse(data);
    ParseTargets(root, ret);
    for(auto& t : ret.targets) {
        printf("target width-%u, height-%u\n", t.width, t.height);
    }

    return ret;
}