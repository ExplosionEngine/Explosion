// Created by Kindem on 2025/8/17.
//

#pragma once

#include <QString>
#include <QList>
#include <QSet>
#include <QMap>

#include <Common/Hash.h>
#include <Common/Serialization.h>

namespace Common {
    template <>
    struct Serializer<QString> {
        static constexpr size_t typeId = HashUtils::StrCrc32("QString");

        static size_t Serialize(BinarySerializeStream& outStream, const QString& inString)
        {
            return Serializer<std::string>::Serialize(outStream, inString.toStdString());
        }

        static size_t Deserialize(BinaryDeserializeStream& inStream, QString& outString)
        {
            std::string stdString;
            const size_t deserialized = Serializer<std::string>::Deserialize(inStream, stdString);
            outString = QString::fromStdString(stdString);
            return deserialized;
        }
    };

    template <Serializable T>
    struct Serializer<QList<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("QList") +
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& outStream, const QList<T>& inList)
        {
            size_t serialized = Serializer<uint64_t>::Serialize(outStream, inList.size());
            for (const auto& element : inList) {
                serialized += Serializer<T>::Serialize(outStream, element);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& inStream, QList<T>& outList)
        {
            size_t deserialized = 0;

            outList.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(inStream, size);

            outList.reserve(size);
            for (auto i = 0; i < size; i++) {
                T element;
                deserialized += Serializer<T>::Deserialize(inStream, element);
                outList.emplaceBack(std::move(element));
            }
            return deserialized;
        }
    };

    template <Serializable T>
    struct Serializer<QSet<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("QSet")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& outStream, const QSet<T>& inSet)
        {
            size_t serialized = Serializer<uint64_t>::Serialize(outStream, inSet.size());
            for (const auto& element : inSet) {
                serialized += Serializer<T>::Serialize(outStream, element);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& inStream, QSet<T>& outSet)
        {
            size_t deserialized = 0;

            outSet.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(inStream, size);

            outSet.reserve(size);
            for (auto i = 0; i < size; i++) {
                T temp;
                deserialized += Serializer<T>::Deserialize(inStream, temp);
                outSet.insert(std::move(temp));
            }
            return deserialized;
        }
    };

    template <Serializable K, Serializable V>
    struct Serializer<QMap<K, V>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("QMap")
            + Serializer<K>::typeId
            + Serializer<V>::typeId;

        static size_t Serialize(BinarySerializeStream& outStream, const QMap<K, V>& inMap)
        {
            size_t serialized = Serializer<uint64_t>::Serialize(outStream, inMap.size());
            for (const auto& [key, value] : inMap) {
                serialized += Serializer<K>::Serialize(outStream, key);
                serialized += Serializer<V>::Serialize(outStream, value);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& inStream, QMap<K, V>& outMap)
        {
            size_t deserialized = 0;

            outMap.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(inStream, size);

            for (auto i = 0; i < size; i++) {
                K key;
                V value;
                deserialized += Serializer<K>::Deserialize(inStream, key);
                deserialized += Serializer<V>::Deserialize(inStream, value);
                outMap.insert(std::move(key), std::move(value));
            }
            return deserialized;
        }
    };

    template <>
    struct JsonSerializer<QString> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const QString& inValue)
        {
            const std::string stdString = inValue.toStdString();
            JsonSerializer<std::string>::JsonSerialize(outJsonValue, inAllocator, stdString);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, QString& outValue)
        {
            std::string stdString;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue, stdString);
            outValue = QString::fromStdString(stdString);
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<QList<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const QList<T>& inList)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inList.size(), inAllocator);
            for (const auto& element : inList) {
                rapidjson::Value jsonElement;
                JsonSerializer<T>::JsonSerialize(jsonElement, inAllocator, element);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, QList<T>& outList)
        {
            outList.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            outList.reserve(inJsonValue.Size());
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                T element;
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], element);
                outList.emplaceBack(std::move(element));
            }
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<QSet<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const QSet<T>& inSet)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inSet.size(), inAllocator);
            for (const auto& element : inSet) {
                rapidjson::Value jsonElement;
                JsonSerializer<T>::JsonSerialize(jsonElement, inAllocator, element);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, QSet<T>& outSet)
        {
            outSet.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            outSet.reserve(inJsonValue.Size());
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                T element;
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], element);
                outSet.insert(std::move(element));
            }
        }
    };

    template <JsonSerializable K, JsonSerializable V>
    struct JsonSerializer<QMap<K, V>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const QMap<K, V>& inMap)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inMap.size(), inAllocator);
            for (const auto& [key, value] : inMap) {
                rapidjson::Value jsonElement;
                jsonElement.SetObject();

                rapidjson::Value jsonKey;
                JsonSerializer<K>::JsonSerialize(jsonElement, inAllocator, key);

                rapidjson::Value jsonValue;
                JsonSerializer<V>::JsonSerialize(jsonValue, inAllocator, value);

                outJsonValue.AddMember("key", jsonKey, inAllocator);
                outJsonValue.AddMember("value", jsonValue, inAllocator);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, QMap<K, V>& outMap)
        {
            outMap.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                K key;
                V value;

                const auto& jsonElement = inJsonValue[i];
                if (!jsonElement.IsObject()) {
                    continue;
                }
                if (jsonElement.HasMember("key")) {
                    JsonSerializer<K>::JsonDeserialize(jsonElement["key"], key);
                }
                if (jsonElement.HasMember("value")) {
                    JsonSerializer<V>::JsonDeserialize(jsonElement["value"], value);
                }
                outMap.insert(std::move(key), std::move(value));
            }
        }
    };
}

