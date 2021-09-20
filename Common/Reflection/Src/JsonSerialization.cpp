//
// Created by Zach Lee on 2021/7/21.
//

#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <IO/FileManager.h>
#include <Reflection/JsonSerialization.h>

using namespace rapidjson;

namespace Explosion {

    static void WriteBasicType(const ReflAny& any, PrettyWriter<StringBuffer>& writer)
    {
        auto type = any.type();
        if (type == Reflection::Resolve<bool>()) {
            writer.Bool(any.cast<bool>());
        } else if (type == Reflection::Resolve<char>()) {
            writer.Int(any.cast<char>());
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
            writer.Int64(any.cast<int64_t>());
        } else if (type == Reflection::Resolve<uint64_t>()) {
            writer.Uint64(any.cast<uint64_t>());
        } else if (type == Reflection::Resolve<float>()) {
            writer.Double(any.cast<float>());
        } else if (type == Reflection::Resolve<double>()) {
            writer.Double(any.cast<double>());
        }
    }

    static ReflAny ReadBasicType(ReflType type, const Value& obj)
    {
        if (type == Reflection::Resolve<bool>() && obj.IsBool()) {
            return ReflAny(obj.Get<bool>());
        } else if (type == Reflection::Resolve<char>() && obj.IsInt()) {
            return ReflAny(static_cast<char>(obj.GetInt()));
        } else if (type == Reflection::Resolve<int8_t>() && obj.IsInt()) {
            return ReflAny(static_cast<int8_t>(obj.GetInt()));
        } else if (type == Reflection::Resolve<uint8_t>() && obj.IsUint()) {
            return ReflAny(static_cast<uint8_t>(obj.GetUint()));
        } else if (type == Reflection::Resolve<int16_t>() && obj.IsInt()) {
            return ReflAny(static_cast<int16_t>(obj.GetInt()));
        } else if (type == Reflection::Resolve<uint16_t>() && obj.IsUint()) {
            return ReflAny(static_cast<uint16_t>(obj.GetUint()));
        } else if (type == Reflection::Resolve<int32_t>() && obj.IsInt()) {
            return ReflAny(obj.Get<int32_t>());
        } else if (type == Reflection::Resolve<uint32_t>() && obj.IsUint()) {
            return ReflAny(obj.Get<uint32_t>());
        } else if (type == Reflection::Resolve<int64_t>() && obj.IsInt64()) {
            return ReflAny(obj.Get<int64_t>());
        } else if (type == Reflection::Resolve<uint64_t>() && obj.IsUint64()) {
            return ReflAny(obj.Get<uint64_t>());
        } else if (type == Reflection::Resolve<float>() && obj.IsFloat()) {
            return ReflAny(obj.Get<float>());
        } else if (type == Reflection::Resolve<double>() && obj.IsDouble()) {
            return ReflAny(obj.Get<double>());
        }
        return {};
    }

    static void WriteString(const std::string_view & id, PrettyWriter<StringBuffer>& writer)
    {
        writer.String(id.data(), static_cast<rapidjson::SizeType>(id.length()), false);
    }

    static void WriteObject(const ReflAny& any, PrettyWriter<StringBuffer>& writer)
    {
        writer.StartObject();
        auto type = any.type();
        auto datas = type.data();
        for (auto curr : datas) {
            WriteString(ReflTypeNameMap::DataName(type.id(), curr.id()), writer);
            auto member = curr.get(any);
            if (curr.type().is_class()) {
                WriteObject(member, writer);
            } else {
                WriteBasicType(curr.get(any), writer);
            }
        }
        writer.EndObject();
    }

    static ReflAny ReadObject(const ReflType& type, Value& obj)
    {
        auto datas = type.data();
        std::vector<ReflAny> anyDatas;
        for (auto curr : datas) {
            auto iter = obj.FindMember(ReflTypeNameMap::DataName(type.id(), curr.id()).data());
            if (iter == obj.MemberEnd()) {
                continue;
            }
            if (curr.type().is_class()) {
                ReflAny sub = ReadObject(curr.type(), iter->value);
                anyDatas.insert(anyDatas.begin(), sub);
            } else {
                anyDatas.insert(anyDatas.begin(), ReadBasicType(curr.type(), iter->value));
            }
        }
        return type.construct(anyDatas.data(), anyDatas.size());
    }

    void JsonSerialization::ToJson(const std::string& path, const ReflAny& any)
    {
        if (!any) {
            return;
        }

        StringBuffer buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        writer.StartObject();
        WriteString(ReflTypeNameMap::TypeName(any.type().id()), writer);
        WriteObject(any, writer);
        writer.EndObject();
        IO::FileManager::WriteFile(path, buffer.GetString(), buffer.GetSize(), false);
    }

    ReflAny JsonSerialization::FromJson(const std::string& path)
    {
        std::string jsonContent;
        jsonContent = IO::FileManager::ReadFile(path);
        Document document;
        if (document.Parse(jsonContent.c_str()).HasParseError()) {
            return {};
        }

        auto& root = document;
        EXPLOSION_ASSERT(root.MemberCount() != 0, "invalid json document");
        auto iter = root.MemberBegin();
        return ReadObject(Reflection::Resolve(iter->name.GetString()), iter->value);;
    }

}