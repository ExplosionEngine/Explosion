#include "PassLoader.h"

#include <FileSystem/FileReader.h>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace Explosion;

template <typename T>
struct TypeTraits {
    static constexpr rapidjson::Type TYPE = rapidjson::Type::kNullType;
};

template <>
struct TypeTraits<uint32_t> {
    static constexpr rapidjson::Type TYPE = rapidjson::Type::kNumberType;
};

template <typename F>
static bool ForEachInArray(rapidjson::Document& doc, const char* member, const F& f)
{
    if (!doc.HasMember(member)) {
        return false;
    }
    const rapidjson::Value &val = doc[member];
    if (!val.IsArray()) {
        return false;
    }

    for (size_t i = 0; i < val.Size(); i++) {
        f(val[i]);
    }
    return true;
}

template<typename T, typename U>
static bool ParseProperty(T& ret, const U& dft, const rapidjson::Value &parent, const char* property)
{
    ret = static_cast<T>(dft);
    if (!parent.HasMember(property)) {
        return false;
    }

    const rapidjson::Value& val = parent[property];
    if (val.GetType() != TypeTraits<T>::TYPE) {
        return false;
    }

    ret = val.Get<T>();
    return true;
}

static bool ParseTargets(rapidjson::Document& root, GraphInfo& graph)
{
    if (!root.HasMember("targets")) {
        return false;
    }

    rapidjson::Value& obj = root["targets"];
    ForEachInArray(root, "targets", [&graph](const rapidjson::Value& v) -> bool {
        TargetInfo target;
        ParseProperty(target.width, 0, v, "width");
        ParseProperty(target.height, 0, v, "height");
        graph.targets.emplace_back(target);
        return true;
    });
    return true;
}

GraphInfo PassLoader::Load(const std::string& url)
{
    GraphInfo ret;
    auto data = FileSystem::FileReader::Read(url, false);
    rapidjson::Document document;
    document.Parse(data.data());
    ParseTargets(document, ret);
    return ret;
}