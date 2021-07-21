//
// Created by LiZhen on 2021/7/21.
//

#include <sstream>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <FileSystem/FileReader.h>
#include <Reflection/JsonSerialization.h>

using namespace rapidjson;

namespace Explosion {

    static void WriteBasicType(const ReflAny& any, PrettyWriter<StringBuffer>& writer)
    {
        auto type = any.type();
        if (type == Reflection::Resolve<bool>()) {
            writer.Bool(any.cast<bool>());
        } else if (type == Reflection::Resolve<char>()) {
            writer.Bool(any.cast<char>());
        } else if (type == Reflection::Resolve<int8_t>()) {
            writer.Int(any.cast<int8_t>());
        } else if (type == Reflection::Resolve<uint8_t>()) {
            writer.Uint(any.cast<uint8_t>());
        } else if (type == Reflection::Resolve<int16_t>()) {
            writer.Int(any.cast<int16_t>());
        } else if (type == Reflection::Resolve<uint16_t>()) {
            writer.Uint(any.cast<uint16_t>());
        } else if (type == Reflection::Resolve<int32_t>()) {
            writer.Int(any.cast<int32_t>());
        } else if (type == Reflection::Resolve<uint32_t>()) {
            writer.Uint(any.cast<uint32_t>());
        } else if (type == Reflection::Resolve<int64_t>()) {
            writer.Int(any.cast<int64_t>());
        } else if (type == Reflection::Resolve<uint64_t>()) {
            writer.Uint(any.cast<uint64_t>());
        } else if (type == Reflection::Resolve<float>()) {
            writer.Double(any.cast<float>());
        } else if (type == Reflection::Resolve<double>()) {
            writer.Double(any.cast<double>());
        }
    }

    static void WriteObject(const ReflAny& any, PrettyWriter<StringBuffer>& writer)
    {
        writer.StartObject();
        auto type = any.type();
        auto datas = type.data();
        for (auto curr : datas) {
            std::stringstream ss;
            ss << curr.id();
            writer.String(ss.str().c_str(), static_cast<rapidjson::SizeType>(ss.str().length()), false);
            WriteBasicType(curr.get(any), writer);
        }
        writer.EndObject();
    }

    void JsonSerialization::ToJson(const std::string& path, const ReflAny& any)
    {
        if (!any) {
            return;
        }

        StringBuffer buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        WriteObject(any, writer);
        std::string str(buffer.GetString());
        printf("%s", str.c_str());
    }

    ReflAny JsonSerialization::FromJson(const std::string& path)
    {
        std::string jsonContent;
        jsonContent = FileSystem::FileReader::Read(path);
        return {};
    }

}