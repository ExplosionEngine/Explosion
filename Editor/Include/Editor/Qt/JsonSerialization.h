//
// Created by Kindem on 2025/8/18.
//

#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <optional>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <tuple>
#include <variant>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

#include <Common/Utility.h>
#include <Common/Math/Math.h>
#include <Mirror/Mirror.h>
#include <Editor/Qt/MirrorTemplateView.h>

// TODO
#pragma optimize("", off)

namespace Editor {
    template <typename T> struct QtJsonSerializer {};
    template <typename T> concept QtJsonSerializable = requires(
        const T& inValue, T& outValue,
        const QJsonValue& inJsonValue, QJsonValue& outJsonValue)
    {
        QtJsonSerializer<T>::QtJsonSerialize(outJsonValue, inValue);
        QtJsonSerializer<T>::QtJsonDeserialize(inJsonValue, outValue);
    };

    template <typename T> void QtJsonSerialize(QJsonValue& outJsonValue, const T& inValue);
    template <typename T> void QtJsonDeserialize(const QJsonValue& inJsonValue, T& outValue);
}

namespace Editor {
    template <typename T>
    void QtJsonSerialize(QJsonValue& outJsonValue, const T& inValue)
    {
        if constexpr (QtJsonSerializable<T>) {
            QtJsonSerializer<T>::QtJsonSerialize(outJsonValue, inValue);
        } else {
            QuickFailWithReason("your type is not support json serialization");
        }
    }

    template <typename T>
    void QtJsonDeserialize(const QJsonValue& inJsonValue, T& outValue)
    {
        if constexpr (QtJsonSerializable<T>) {
            QtJsonSerializer<T>::QtJsonDeserialize(inJsonValue, outValue);
        } else {
            QuickFailWithReason("your type is not support json serialization");
        }
    }

    template <>
    struct QtJsonSerializer<bool> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, bool inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, bool& outValue)
        {
            if (!inJsonValue.isBool()) {
                return;
            }
            outValue = inJsonValue.toBool();
        }
    };

    template <>
    struct QtJsonSerializer<int8_t> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, int8_t inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, int8_t& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = static_cast<int8_t>(inJsonValue.toInt());
        }
    };

    template <>
    struct QtJsonSerializer<uint8_t> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, uint8_t inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, uint8_t& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = static_cast<uint8_t>(inJsonValue.toInt());
        }
    };

    template <>
    struct QtJsonSerializer<int16_t> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, int16_t inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, int16_t& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = static_cast<int16_t>(inJsonValue.toInt());
        }
    };

    template <>
    struct QtJsonSerializer<uint16_t> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, uint16_t inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, uint16_t& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = static_cast<uint16_t>(inJsonValue.toInt());
        }
    };

    template <>
    struct QtJsonSerializer<int32_t> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, int32_t inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, int32_t& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = inJsonValue.toInt();
        }
    };

    template <>
    struct QtJsonSerializer<uint32_t> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, uint32_t inValue)
        {
            outJsonValue = static_cast<qint64>(inValue);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, uint32_t& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = static_cast<uint32_t>(inJsonValue.toInteger());
        }
    };

    template <>
    struct QtJsonSerializer<int64_t> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, int64_t inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, int64_t& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = inJsonValue.toInteger();
        }
    };

    template <>
    struct QtJsonSerializer<uint64_t> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, uint64_t inValue)
        {
            outJsonValue = static_cast<int64_t>(inValue);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, uint64_t& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = static_cast<uint64_t>(inJsonValue.toInteger());
        }
    };

    template <>
    struct QtJsonSerializer<float> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, float inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, float& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = static_cast<float>(inJsonValue.toDouble());
        }
    };

    template <>
    struct QtJsonSerializer<double> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, double inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, double& outValue)
        {
            if (!inJsonValue.isDouble()) {
                return;
            }
            outValue = inJsonValue.toDouble();
        }
    };

    template <>
    struct QtJsonSerializer<std::string> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::string& inValue)
        {
            outJsonValue = QString::fromStdString(inValue);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::string& outValue)
        {
            if (!inJsonValue.isString()) {
                return;
            }
            outValue = inJsonValue.toString().toStdString();
        }
    };

    template <>
    struct QtJsonSerializer<std::wstring> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::wstring& inValue)
        {
            outJsonValue = QString::fromStdWString(inValue);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::wstring& outValue)
        {
            if (!inJsonValue.isString()) {
                return;
            }
            outValue = inJsonValue.toString().toStdWString();
        }
    };

    template <QtJsonSerializable T>
    struct QtJsonSerializer<std::optional<T>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::optional<T>& inValue)
        {
            if (inValue.has_value()) {
                QtJsonSerializer<T>::QtJsonSerialize(outJsonValue, inValue.value());
            } else {
                outJsonValue = QJsonValue::Null;
            }
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::optional<T>& outValue)
        {
            outValue.reset();
            if (inJsonValue.isNull()) {
                return;
            }
            T& outValueRef = outValue.emplace();
            QtJsonSerializer<T>::QtJsonDeserialize(inJsonValue, outValueRef);
        }
    };

    template <QtJsonSerializable K, QtJsonSerializable V>
    struct QtJsonSerializer<std::pair<K, V>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::pair<K, V>& inValue)
        {
            QJsonValue jsonKey;
            QtJsonSerializer<K>::QtJsonSerialize(jsonKey, inValue.first);

            QJsonValue jsonValue;
            QtJsonSerializer<V>::QtJsonSerialize(jsonValue, inValue.second);

            QJsonObject jsonObject;
            jsonObject["key"] = jsonKey;
            jsonObject["value"] = jsonValue;
            outJsonValue = std::move(jsonObject);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::pair<K, V>& outValue)
        {
            if (!inJsonValue.isObject()) {
                return;
            }
            const QJsonObject jsonObject = inJsonValue.toObject();
            if (jsonObject.contains("key")) {
                const QJsonValue jsonKey = jsonObject["key"];
                QtJsonSerializer<K>::QtJsonDeserialize(jsonKey, outValue.first);
            }
            if (jsonObject.contains("value")) {
                const QJsonValue jsonValue = jsonObject["value"];
                QtJsonSerializer<K>::QtJsonDeserialize(jsonValue, outValue.second);
            }
        }
    };

    template <QtJsonSerializable T, size_t N>
    struct QtJsonSerializer<std::array<T, N>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::array<T, N>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& element : inValue) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonSerialize(jsonElement, element);
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::array<T, N>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            if (jsonArray.size() != N) {
                return;
            }
            for (auto i = 0; i < N; i++) {
                QtJsonSerializer<T>::QtJsonDeserialize(jsonArray[i], outValue[i]);
            }
        }
    };

    template <QtJsonSerializable T>
    struct QtJsonSerializer<std::vector<T>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::vector<T>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& element : inValue) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonSerialize(jsonElement, element);
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::vector<T>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }

            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            outValue.reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                T element;
                QtJsonSerializer<T>::QtJsonDeserialize(jsonElement, element);
                outValue.emplaceback(std::move(element));
            }
        }
    };

    template <QtJsonSerializable T>
    struct QtJsonSerializer<std::list<T>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::list<T>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& element : inValue) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonSerialize(jsonElement, element);
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::list<T>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }

            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            outValue.reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                T element;
                QtJsonSerializer<T>::QtJsonDeserialize(jsonElement, element);
                outValue.emplace_back(std::move(element));
            }
        }
    };

    template <QtJsonSerializable T>
    struct QtJsonSerializer<std::unordered_set<T>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::unordered_set<T>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& element : inValue) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonSerialize(jsonElement, element);
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::unordered_set<T>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }

            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            outValue.reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                T element;
                QtJsonSerializer<T>::QtJsonDeserialize(jsonElement, element);
                outValue.emplace(std::move(element));
            }
        }
    };

    template <QtJsonSerializable T>
    struct QtJsonSerializer<std::set<T>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::set<T>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& element : inValue) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonSerialize(jsonElement, element);
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::set<T>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }

            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            outValue.reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                T element;
                QtJsonSerializer<T>::QtJsonDeserialize(jsonElement, element);
                outValue.emplace(std::move(element));
            }
        }
    };

    template <QtJsonSerializable K, QtJsonSerializable V>
    struct QtJsonSerializer<std::unordered_map<K, V>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::unordered_map<K, V>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& pair : inValue) {
                QJsonValue jsonPair;
                QtJsonSerializer<std::pair<K, V>>::QtJsonSerialize(jsonPair, pair);
                jsonArray.append(jsonPair);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::unordered_map<K, V>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }

            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            outValue.reserve(jsonArray.size());
            for (const auto& jsonPair : jsonArray) {
                std::pair<K, V> pair;
                QtJsonSerializer<std::pair<K, V>>::QtJsonSerialize(jsonPair, pair);
                outValue.emplace(std::move(pair));
            }
        }
    };

    template <QtJsonSerializable K, QtJsonSerializable V>
    struct QtJsonSerializer<std::map<K, V>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::map<K, V>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& pair : inValue) {
                QJsonValue jsonPair;
                QtJsonSerializer<std::pair<K, V>>::QtJsonSerialize(jsonPair, pair);
                jsonArray.append(jsonPair);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::map<K, V>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }

            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            outValue.reserve(jsonArray.size());
            for (const auto& jsonPair : jsonArray) {
                std::pair<K, V> pair;
                QtJsonSerializer<std::pair<K, V>>::QtJsonSerialize(jsonPair, pair);
                outValue.emplace(std::move(pair));
            }
        }
    };

    template <QtJsonSerializable... T>
    struct QtJsonSerializer<std::tuple<T...>> {
        template <size_t... I>
        static void QtJsonSerializeInternal(QJsonObject& outJsonObject, const std::tuple<T...>& inValue, std::index_sequence<I...>)
        {
            (void) std::initializer_list<int> { ([&]() -> void {
                const auto key = std::to_string(I);

                QJsonValue jsonValue;
                QtJsonSerializer<T>::QtJsonSerialize(jsonValue, std::get<I>(inValue));

                outJsonObject[QString::fromStdString(key)] = jsonValue;
            }(), 0)... };
        }

        template <size_t... I>
        static void QtJsonDeserializeInternal(const QJsonObject& inJsonObject, std::tuple<T...>& outValue, std::index_sequence<I...>)
        {
            (void) std::initializer_list<int> { ([&]() -> void {
                const auto key = std::to_string(I);
                const QJsonValue jsonValue = inJsonObject[key];
                if (jsonValue.isNull()) {
                    return;
                }
                QtJsonSerializer<T>::QtJsonDeserialize(jsonValue, std::get<I>(outValue));
            }(), 0)... };
        }

        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::tuple<T...>& inValue)
        {
            QJsonObject jsonObject;
            QtJsonSerializeInternal(jsonObject, inValue, std::make_index_sequence<sizeof...(T)> {});
            outJsonValue = std::move(jsonObject);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::tuple<T...>& outValue)
        {
            outValue = {};
            if (!inJsonValue.isObject()) {
                return;
            }
            QtJsonDeserializeInternal(inJsonValue.toObject(), outValue, std::make_index_sequence<sizeof...(T)> {});
        }
    };

    template <QtJsonSerializable... T>
    struct QtJsonSerializer<std::variant<T...>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const std::variant<T...>& inValue)
        {
            QJsonValue jsonType;
            QtJsonSerializer<uint64_t>::QtJsonSerialize(jsonType, inValue.index());

            QJsonValue jsonContent;
            std::visit([&]<typename T0>(T0&& v) -> void {
                QtJsonSerializer<std::decay_t<T0>>::QtJsonSerialize(jsonContent, v);
            }, inValue);

            QJsonObject jsonObject;
            jsonObject["type"] = jsonType;
            jsonObject["content"] = jsonContent;
            outJsonValue = std::move(jsonObject);
        }

        template <size_t... I>
        static void QtJsonDeserializeInternal(const QJsonValue& inContentJsonValue, std::variant<T...>& outValue, size_t inAspectIndex, std::index_sequence<I...>)
        {
            (void) std::initializer_list<int> { ([&]() -> void {
                if (I != inAspectIndex) {
                    return;
                }

                T temp;
                QtJsonSerializer<T>::QtJsonDeserialize(inContentJsonValue, temp);
                outValue = std::move(temp);
            }(), 0)... };
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, std::variant<T...>& outValue)
        {
            if (!inJsonValue.isObject()) {
                return;
            }

            const QJsonObject jsonObject = inJsonValue.toObject();
            if (!jsonObject.contains("type") || !jsonObject.contains("content")) {
                return;
            }
            const QJsonValue jsonType = jsonObject["type"];
            const QJsonValue jsonContent = jsonObject["content"];

            uint64_t aspectIndex;
            QtJsonSerializer<uint64_t>::QtJsonDeserialize(jsonType, aspectIndex);
            QtJsonDeserializeInternal(jsonContent, outValue, aspectIndex, std::make_index_sequence<sizeof...(T)> {});
        }
    };

    template <QtJsonSerializable T, uint8_t L>
    struct QtJsonSerializer<Common::Vec<T, L>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const Common::Vec<T, L>& inValue)
        {
            QJsonArray jsonArray;
            for (auto i = 0; i < L; i++) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonSerialize(jsonElement, inValue[i]);
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, Common::Vec<T, L>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            if (jsonArray.size() != L) {
                return;
            }
            for (auto i = 0; i < L; i++) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonDeserialize(jsonElement, jsonArray[i]);
            }
        }
    };

    template <QtJsonSerializable T, uint8_t R, uint8_t C>
    struct QtJsonSerializer<Common::Mat<T, R, C>> {
        static void QtJsonSerialize(QJsonValue& jsonValue, const Common::Mat<T, R, C>& inValue)
        {
            QJsonArray jsonArray;
            for (auto i = 0; i < R; i++) {
                for (auto j = 0; j < C; j++) {
                    QJsonValue jsonElement;
                    QtJsonSerializer<T>::QtJsonSerialize(jsonElement, inValue.At(i, j));
                    jsonArray.append(jsonElement);
                }
            }
            jsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& jsonValue, Common::Mat<T, R, C>& outValue)
        {
            if (!jsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = jsonValue.toArray();
            if (jsonArray.size() != R * C) {
                return;
            }
            for (auto i = 0; i < jsonArray.size(); i++) {
                auto row = i / C;
                auto col = i % C;
                QtJsonSerializer<T>::QtJsonDeserialize(jsonArray[i], outValue.At(row, col));
            }
        }
    };

    template <QtJsonSerializable T>
    struct QtJsonSerializer<Common::Quaternion<T>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const Common::Quaternion<T>& inValue)
        {
            QJsonValue jsonW;
            QtJsonSerializer<T>::QtJsonSerialize(jsonW, inValue.w);

            QJsonValue jsonX;
            QtJsonSerializer<T>::QtJsonSerialize(jsonX, inValue.x);

            QJsonValue jsonY;
            QtJsonSerializer<T>::QtJsonSerialize(jsonY, inValue.y);

            QJsonValue jsonZ;
            QtJsonSerializer<T>::QtJsonSerialize(jsonZ, inValue.z);

            QJsonArray jsonArray;
            jsonArray.append(jsonX);
            jsonArray.append(jsonY);
            jsonArray.append(jsonZ);
            jsonArray.append(jsonW);
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, Common::Quaternion<T>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            if (jsonArray.size() != 4) {
                return;
            }

            const QJsonValue jsonX = jsonArray[0];
            QtJsonSerializer<T>::QtJsonDeserialize(jsonX, outValue.x);

            const QJsonValue jsonY = jsonArray[1];
            QtJsonSerializer<T>::QtJsonDeserialize(jsonY, outValue.y);

            const QJsonValue jsonZ = jsonArray[2];
            QtJsonSerializer<T>::QtJsonDeserialize(jsonZ, outValue.z);

            const QJsonValue jsonW = jsonArray[3];
            QtJsonSerializer<T>::QtJsonDeserialize(jsonW, outValue.w);
        }
    };

    template <>
    struct QtJsonSerializer<Common::Color> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const Common::Color& inValue)
        {
            QJsonValue jsonR;
            QtJsonSerializer<uint8_t>::QtJsonSerialize(jsonR, inValue.r);

            QJsonValue jsonG;
            QtJsonSerializer<uint8_t>::QtJsonSerialize(jsonG, inValue.g);

            QJsonValue jsonB;
            QtJsonSerializer<uint8_t>::QtJsonSerialize(jsonB, inValue.b);

            QJsonValue jsonA;
            QtJsonSerializer<uint8_t>::QtJsonSerialize(jsonA, inValue.a);

            QJsonArray jsonArray;
            jsonArray.append(jsonR);
            jsonArray.append(jsonG);
            jsonArray.append(jsonB);
            jsonArray.append(jsonA);
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, Common::Color& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            if (jsonArray.size() != 4) {
                return;
            }

            const QJsonValue jsonR = jsonArray[0];
            QtJsonSerializer<uint8_t>::QtJsonDeserialize(jsonR, outValue.r);

            const QJsonValue jsonG = jsonArray[1];
            QtJsonSerializer<uint8_t>::QtJsonDeserialize(jsonG, outValue.g);

            const QJsonValue jsonB = jsonArray[2];
            QtJsonSerializer<uint8_t>::QtJsonDeserialize(jsonB, outValue.b);

            const QJsonValue jsonA = jsonArray[3];
            QtJsonSerializer<uint8_t>::QtJsonDeserialize(jsonA, outValue.a);
        }
    };

    template <>
    struct QtJsonSerializer<Common::LinearColor> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const Common::LinearColor& inValue)
        {
            QJsonValue jsonR;
            QtJsonSerializer<float>::QtJsonSerialize(jsonR, inValue.r);

            QJsonValue jsonG;
            QtJsonSerializer<float>::QtJsonSerialize(jsonG, inValue.g);

            QJsonValue jsonB;
            QtJsonSerializer<float>::QtJsonSerialize(jsonB, inValue.b);

            QJsonValue jsonA;
            QtJsonSerializer<float>::QtJsonSerialize(jsonA, inValue.a);

            QJsonArray jsonArray;
            jsonArray.append(jsonR);
            jsonArray.append(jsonG);
            jsonArray.append(jsonB);
            jsonArray.append(jsonA);
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, Common::LinearColor& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            if (jsonArray.size() != 4) {
                return;
            }

            const QJsonValue jsonR = jsonArray[0];
            QtJsonSerializer<float>::QtJsonDeserialize(jsonR, outValue.r);

            const QJsonValue jsonG = jsonArray[1];
            QtJsonSerializer<float>::QtJsonDeserialize(jsonG, outValue.g);

            const QJsonValue jsonB = jsonArray[2];
            QtJsonSerializer<float>::QtJsonDeserialize(jsonB, outValue.b);

            const QJsonValue jsonA = jsonArray[3];
            QtJsonSerializer<float>::QtJsonDeserialize(jsonA, outValue.a);
        }
    };

    template <>
    struct QtJsonSerializer<QString> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const QString& inValue)
        {
            outJsonValue = inValue;
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, QString& outValue)
        {
            if (!inJsonValue.isString()) {
                return;
            }
            outValue = inJsonValue.toString();
        }
    };

    template <QtJsonSerializable T>
    struct QtJsonSerializer<QList<T>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const QList<T>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& element : inValue) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonSerialize(jsonElement, element);
                jsonArray.push_back(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, QList<T>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            outValue.reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                T element;
                QtJsonSerializer<T>::QtJsonDeserialize(jsonElement, element);
                outValue.emplaceBack(std::move(element));
            }
        }
    };

    template <QtJsonSerializable T>
    struct QtJsonSerializer<QSet<T>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const QSet<T>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& element : inValue) {
                QJsonValue jsonElement;
                QtJsonSerializer<T>::QtJsonSerialize(jsonElement, element);
                jsonArray.push_back(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, QSet<T>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            outValue.reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                T element;
                QtJsonSerializer<T>::QtJsonDeserialize(jsonElement, element);
                outValue.emplaceBack(std::move(element));
            }
        }
    };

    template <QtJsonSerializable K, QtJsonSerializable V>
    struct QtJsonSerializer<QMap<K, V>> {
        static void QtJsonSerialize(QJsonValue& outJsonValue, const QMap<K, V>& inValue)
        {
            QJsonArray jsonArray;
            for (const auto& [key, value] : inValue) {
                QJsonValue jsonKey;
                QtJsonSerializer<K>::QtJsonSerialize(jsonKey, key);

                QJsonValue jsonValue;
                QtJsonSerializer<V>::QtJsonSerialize(jsonValue, value);

                QJsonObject jsonObject;
                jsonObject["key"] = jsonKey;
                jsonObject["value"] = jsonValue;
                jsonArray.append(std::move(jsonObject));
            }
            outJsonValue = std::move(jsonArray);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, QMap<K, V>& outValue)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            outValue.clear();
            for (const auto& jsonElement : jsonArray) {
                if (!jsonElement.isObject()) {
                    continue;
                }
                const QJsonObject jsonObject = jsonElement.toObject();

                K key;
                if (jsonObject.contains("key")) {
                    const QJsonValue jsonKey = jsonObject["key"];
                    QtJsonSerializer<K>::QtJsonDeserialize(jsonKey, key);
                }

                V value;
                if (jsonObject.contains("value")) {
                    const QJsonValue jsonValue = jsonObject["value"];
                    QtJsonSerializer<V>::QtJsonDeserialize(jsonValue, value);
                }

                outValue.emplace(std::move(key), std::move(value));
            }
        }
    };

    template <Mirror::MetaClass T>
    struct QtJsonSerializer<T> {
        static void QtJsonSerializeDyn(QJsonObject& outJsonObject, const Mirror::Class& inClass, const Mirror::Argument& inValue)
        {
            const auto* baseClass = inClass.GetBaseClass();
            const auto& memberVariables = inClass.GetMemberVariables();
            const auto defaultObject = inClass.GetDefaultObject();

            if (baseClass != nullptr) {
                QJsonObject baseContentJson;
                QtJsonSerializeDyn(baseContentJson, *baseClass, inValue);
                outJsonObject["_base"] = std::move(baseContentJson);
            }

            for (const auto& memberVariable : memberVariables | std::views::values) {
                if (memberVariable.IsTransient()) {
                    continue;
                }

                const bool sameAsDefault = defaultObject.Empty() || !memberVariable.GetTypeInfo()->equalComparable
                    ? false
                    : memberVariable.GetDyn(inValue) == memberVariable.GetDyn(defaultObject);

                if (sameAsDefault) {
                    continue;
                }
                QJsonValue memberContentJson;
                Mirror::Any memberRef = memberVariable.GetDyn(inValue);
                SerializeValueDyn(memberContentJson, memberRef);
                outJsonObject[QString::fromStdString(memberVariable.GetName())] = memberContentJson;
            }
        }

        static void QtJsonDeserializeDyn(const QJsonObject& inJsonObject, const Mirror::Class& inClass, const Mirror::Argument& outValue)
        {
            const auto* baseClass = inClass.GetBaseClass();
            const auto defaultObject = inClass.GetDefaultObject();

            if (inJsonObject.contains("_base")) {
                if (const QJsonValue baseContentJson = inJsonObject["_base"];
                    baseClass != nullptr && baseContentJson.isObject()) {
                    QtJsonDeserializeDyn(baseContentJson.toObject(), *baseClass, outValue);
                }
            }

            for (const auto& memberVariable : inClass.GetMemberVariables() | std::views::values) {
                const auto& memberName = memberVariable.GetName();
                if (const QJsonValue memberContentJson = inJsonObject[QString::fromStdString(memberName)];
                    !memberContentJson.isNull()) {
                    DeserializeValueDyn(memberContentJson, memberVariable.GetDyn(outValue));
                } else {
                    if (!defaultObject.Empty()) {
                        memberVariable.SetDyn(outValue, memberVariable.GetDyn(defaultObject));
                    }
                }
            }
        }

        static void QtJsonSerialize(QJsonValue& outJsonValue, const T& inValue)
        {
            QJsonObject jsonObject;
            QtJsonSerializeDyn(jsonObject, Mirror::Class::Get<T>(), Mirror::ForwardAsArg(inValue));
            outJsonValue = std::move(jsonObject);
        }

        static void QtJsonDeserialize(const QJsonValue& inJsonValue, T& outValue)
        {
            if (inJsonValue.isNull()) {
                return;
            }
            QtJsonDeserializeDyn(inJsonValue.toObject(), Mirror::Class::Get<T>(), Mirror::ForwardAsArg(outValue));
        }

    private:
        using SupportedSimpleTypes = std::tuple<
            bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string, std::wstring,
            Common::FVec2, Common::FVec3, Common::FVec4, Common::FMat4x4, Common::FQuat, Common::Color, Common::LinearColor,
            QString
        >;

        using SupportedTemplateViews = std::tuple<
            Common::Wrap<Mirror::StdOptionalView>, Common::Wrap<Mirror::StdPairView>, Common::Wrap<Mirror::StdArrayView>, Common::Wrap<Mirror::StdVectorView>,
            Common::Wrap<Mirror::StdListView>, Common::Wrap<Mirror::StdUnorderedSetView>, Common::Wrap<Mirror::StdSetView>, Common::Wrap<Mirror::StdUnorderedMapView>,
            Common::Wrap<Mirror::StdMapView>, Common::Wrap<Mirror::StdTupleView>, Common::Wrap<Mirror::StdVariantView>, Common::Wrap<QListMetaView>, Common::Wrap<QMapMetaView>
        >;

        static auto BuildSimpleTypeDynSerializers()
        {
            std::unordered_map<Mirror::TypeId, std::function<void(QJsonValue&, const Mirror::Any&)>> result;
            Common::TupleTypeTraverser<SupportedSimpleTypes>::Each([&]<typename T0>(T0&&) -> void {
                using RawType = std::decay_t<T0>;
                result.emplace(Mirror::GetTypeId<RawType>(), [](QJsonValue& outJsonValue, const Mirror::Any& inRef) -> void {
                    QtJsonSerializer<T0>::QtJsonSerialize(outJsonValue, inRef.As<const T0&>());
                });
            });
            return result;
        }

        static auto BuildSimpleTypeDynDeserializers()
        {
            std::unordered_map<Mirror::TypeId, std::function<void(const QJsonValue&, const Mirror::Any&)>> result;
            Common::TupleTypeTraverser<SupportedSimpleTypes>::Each([&]<typename T0>(T0&&) -> void {
                using RawType = std::decay_t<T0>;
                result.emplace(Mirror::GetTypeId<RawType>(), [](const QJsonValue& inJsonValue, const Mirror::Any& outRef) -> void {
                    QtJsonSerializer<T0>::QtJsonDeserialize(inJsonValue, outRef.As<T0&>());
                });
            });
            return result;
        }

        static auto BuildTemplateViewDynSerializers()
        {
            std::unordered_map<Mirror::TemplateViewId, std::function<void(QJsonValue&, const Mirror::Any&)>> result;
            Common::TupleTypeTraverser<SupportedTemplateViews>::Each([&]<typename T0>(T0&&) -> void {
                using RawType = typename std::decay_t<T0>::RawType;
                result.emplace(RawType::id, [](QJsonValue& outJsonValue, const Mirror::Any& inRef) -> void {
                    const RawType view(inRef);
                    SerializeDynWithView(outJsonValue, view);
                });
            });
            return result;
        }

        static auto BuildTemplateViewDynDeserializers()
        {
            std::unordered_map<Mirror::TemplateViewId, std::function<void(const QJsonValue&, const Mirror::Any&)>> result;
            Common::TupleTypeTraverser<SupportedTemplateViews>::Each([&]<typename T0>(T0&&) -> void {
                using RawType = typename std::decay_t<T0>::RawType;
                result.emplace(RawType::id, [](const QJsonValue& inJsonValue, const Mirror::Any& inRef) -> void {
                    const RawType view(inRef);
                    DeserializeDynWithView(inJsonValue, view);
                });
            });
            return result;
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdOptionalView& inView)
        {
            if (inView.HasValue()) {
                SerializeValueDyn(outJsonValue, inView.ConstValue());
            } else {
                outJsonValue = QJsonValue::Null;
            }
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdOptionalView& inView)
        {
            inView.Reset();
            if (inJsonValue.isNull()) {
                return;
            }
            const Mirror::Any newValueRef = inView.EmplaceDefault();
            DeserializeValueDyn(inJsonValue, newValueRef);
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdPairView& inView)
        {
            QJsonValue jsonKey;
            SerializeValueDyn(jsonKey, inView.ConstKey());

            QJsonValue jsonValue;
            SerializeValueDyn(jsonValue, inView.ConstValue());

            QJsonObject jsonObject;
            jsonObject["key"] = jsonKey;
            jsonObject["value"] = jsonValue;
            outJsonValue = std::move(jsonValue);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdPairView& inView)
        {
            if (!inJsonValue.isObject()) {
                return;
            }
            inView.Reset();
            const QJsonObject jsonObject = inJsonValue.toObject();
            if (jsonObject.contains("key")) {
                const QJsonValue jsonKey = jsonObject["key"];
                DeserializeValueDyn(jsonKey, inView.Key());
            }
            if (jsonObject.contains("value")) {
                const QJsonValue jsonValue = jsonObject["value"];
                DeserializeValueDyn(jsonValue, inView.Value());
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdArrayView& inView)
        {
            QJsonArray jsonArray;
            for (auto i = 0; i < inView.Size(); i++) {
                QJsonValue jsonElement;
                SerializeValueDyn(jsonElement, inView.ConstAt(i));
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdArrayView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            if (jsonArray.size() != inView.Size()) {
                return;
            }
            for (auto i = 0; i < jsonArray.size(); i++) {
                const QJsonValue jsonElement = jsonArray[i];
                DeserializeValueDyn(jsonElement, inView.At(i));
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdVectorView& inView)
        {
            QJsonArray jsonArray;
            for (auto i = 0; i < inView.Size(); i++) {
                QJsonValue jsonElement;
                SerializeValueDyn(jsonElement, inView.ConstAt(i));
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdVectorView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            inView.Clear();
            inView.Reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                DeserializeValueDyn(jsonElement, inView.EmplaceDefaultBack());
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdListView& inView)
        {
            QJsonArray jsonArray;
            inView.ConstTraverse([&](const Mirror::Any& elementRef) -> void {
                QJsonValue jsonElement;
                SerializeValueDyn(jsonElement, elementRef);
                jsonArray.append(jsonElement);
            });
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdListView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            inView.Clear();
            for (const auto& jsonElement : jsonArray) {
                DeserializeValueDyn(jsonElement, inView.EmplaceDefaultBack());
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdUnorderedSetView& inView)
        {
            QJsonArray jsonArray;
            inView.ConstTraverse([&](const Mirror::Any& elementRef) -> void {
                QJsonValue jsonElement;
                SerializeValueDyn(jsonElement, elementRef);
                jsonArray.append(jsonElement);
            });
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdUnorderedSetView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            inView.Clear();
            inView.Reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                Mirror::Any element = inView.CreateElement();
                DeserializeValueDyn(jsonElement, element);
                inView.Emplace(element);
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdSetView& inView)
        {
            QJsonArray jsonArray;
            inView.ConstTraverse([&](const Mirror::Any& elementRef) -> void {
                QJsonValue jsonElement;
                SerializeValueDyn(jsonElement, elementRef);
                jsonArray.append(jsonElement);
            });
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdSetView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            inView.Clear();
            for (const auto& jsonElement : jsonArray) {
                Mirror::Any element = inView.CreateElement();
                DeserializeValueDyn(jsonElement, element);
                inView.Emplace(element);
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdUnorderedMapView& inView)
        {
            QJsonArray jsonArray;
            inView.ConstTraverse([&](const Mirror::Any& inKey, const Mirror::Any& inValue) -> void {
                QJsonValue jsonKey;
                SerializeValueDyn(jsonKey, inKey);

                QJsonValue jsonValue;
                SerializeValueDyn(jsonValue, inValue);

                QJsonObject jsonObject;
                jsonObject["key"] = jsonKey;
                jsonObject["value"] = jsonValue;
                jsonArray.append(jsonObject);
            });
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdUnorderedMapView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            inView.Clear();
            inView.Reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                if (!jsonElement.isObject()) {
                    continue;
                }
                const QJsonObject jsonObject = jsonElement.toObject();
                Mirror::Any key = inView.CreateKey();
                if (jsonObject.contains("key")) {
                    QJsonValue jsonKey = jsonObject["key"];
                    DeserializeValueDyn(jsonKey, key);
                }
                Mirror::Any value = inView.CreateValue();
                if (jsonObject.contains("value")) {
                    QJsonValue jsonValue = jsonObject["value"];
                    DeserializeValueDyn(jsonValue, value);
                }
                inView.Emplace(key, value);
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdMapView& inView)
        {
            QJsonArray jsonArray;
            inView.ConstTraverse([&](const Mirror::Any& inKey, const Mirror::Any& inValue) -> void {
                QJsonValue jsonKey;
                SerializeValueDyn(jsonKey, inKey);

                QJsonValue jsonValue;
                SerializeValueDyn(jsonValue, inValue);

                QJsonObject jsonObject;
                jsonObject["key"] = jsonKey;
                jsonObject["value"] = jsonValue;
                jsonArray.append(jsonObject);
            });
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdMapView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            inView.Clear();
            for (const auto& jsonElement : jsonArray) {
                if (!jsonElement.isObject()) {
                    continue;
                }
                const QJsonObject jsonObject = jsonElement.toObject();
                Mirror::Any key = inView.CreateKey();
                if (jsonObject.contains("key")) {
                    QJsonValue jsonKey = jsonObject["key"];
                    DeserializeValueDyn(jsonKey, key);
                }
                Mirror::Any value = inView.CreateValue();
                if (jsonObject.contains("value")) {
                    QJsonValue jsonValue = jsonObject["value"];
                    DeserializeValueDyn(jsonValue, value);
                }
                inView.Emplace(key, value);
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdTupleView& inView)
        {
            QJsonObject jsonObject;
            inView.ConstTraverse([&](const Mirror::Any& inElementRef, size_t inIndex) -> void {
                QJsonValue jsonElement;
                SerializeValueDyn(jsonElement, inElementRef);
                jsonObject[QString::number(inIndex)] = jsonElement;
            });
            outJsonValue = std::move(jsonObject);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdTupleView& inView)
        {
            if (!inJsonValue.isObject()) {
                return;
            }
            const QJsonObject jsonObject = inJsonValue.toObject();
            for (const auto& key : jsonObject.keys()) {
                const auto index = key.toUInt();
                const QJsonValue jsonElement = jsonObject[key];
                const Mirror::Any elementRef = inView.Get(index);
                DeserializeValueDyn(jsonElement, elementRef);
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const Mirror::StdVariantView& inView)
        {
            QJsonValue typeJson;
            QtJsonSerializer<uint64_t>::QtJsonSerialize(typeJson, inView.Index());

            QJsonValue contentJson;
            inView.Visit([&](const Mirror::Any& inValueRef) -> void {
                SerializeValueDyn(contentJson, inValueRef);
            });

            QJsonObject jsonObject;
            jsonObject["type"] = typeJson;
            jsonObject["content"] = contentJson;
            outJsonValue = std::move(jsonObject);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const Mirror::StdVariantView& inView)
        {
            if (!inJsonValue.isObject()) {
                return;
            }
            const QJsonObject jsonObject = inJsonValue.toObject();
            if (!jsonObject.contains("type") || jsonObject.contains("content")) {
                return;
            }

            const QJsonValue typeJson = jsonObject["type"];
            const QJsonValue contentJson = jsonObject["content"];

            uint64_t type;
            QtJsonSerializer<uint64_t>::QtJsonDeserialize(typeJson, type);

            Mirror::Any tempObj = inView.CreateElement(type);
            DeserializeValueDyn(contentJson, tempObj);

            (void) inView.Emplace(type, tempObj);
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const QListMetaView& inView)
        {
            QJsonArray jsonArray;
            for (auto i = 0; i < inView.Size(); i++) {
                QJsonValue jsonElement;
                SerializeValueDyn(jsonElement, inView.ConstAt(i));
                jsonArray.append(jsonElement);
            }
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const QListMetaView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            inView.Clear();
            inView.Reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                DeserializeValueDyn(jsonElement, inView.EmplaceDefaultBack());
            }
        }

        static void SerializeDynWithView(QJsonValue& outJsonValue, const QMapMetaView& inView)
        {
            QJsonArray jsonArray;
            inView.ConstTraverse([&](const Mirror::Any& inKey, const Mirror::Any& inValue) -> void {
                QJsonValue jsonKey;
                SerializeValueDyn(jsonKey, inKey);

                QJsonValue jsonValue;
                SerializeValueDyn(jsonValue, inValue);

                QJsonObject jsonObject;
                jsonObject["key"] = jsonKey;
                jsonObject["value"] = jsonValue;
                jsonArray.append(jsonObject);
            });
            outJsonValue = std::move(jsonArray);
        }

        static void DeserializeDynWithView(const QJsonValue& inJsonValue, const QMapMetaView& inView)
        {
            if (!inJsonValue.isArray()) {
                return;
            }
            const QJsonArray jsonArray = inJsonValue.toArray();
            inView.Clear();
            inView.Reserve(jsonArray.size());
            for (const auto& jsonElement : jsonArray) {
                if (!jsonElement.isObject()) {
                    continue;
                }
                const QJsonObject jsonObject = jsonElement.toObject();
                Mirror::Any key = inView.CreateKey();
                if (jsonObject.contains("key")) {
                    QJsonValue jsonKey = jsonObject["key"];
                    DeserializeValueDyn(jsonKey, key);
                }
                Mirror::Any value = inView.CreateValue();
                if (jsonObject.contains("value")) {
                    QJsonValue jsonValue = jsonObject["value"];
                    DeserializeValueDyn(jsonValue, value);
                }
                inView.Emplace(key, value);
            }
        }

        static void SerializeValueDyn(QJsonValue& outJsonValue, const Mirror::Any& inValueRef)
        {
            static auto simpleTypeDynSerializers = BuildSimpleTypeDynSerializers();
            static auto templateViewDynSerializers = BuildTemplateViewDynSerializers();

            if (inValueRef.HasTemplateView()) {
                const Mirror::TemplateViewId templateViewId = inValueRef.GetTemplateViewId();
                AssertWithReason(templateViewDynSerializers.contains(templateViewId), std::format("QtJsonSerializeValueDyn not support type {}", inValueRef.Type()->name));
                templateViewDynSerializers.at(templateViewId)(outJsonValue, inValueRef);
            } else if (const Mirror::Class* clazz = inValueRef.GetDynamicClass(); clazz != nullptr) {
                AssertWithReason(!inValueRef.Type()->isPointer, "QtJsonSerializeValueDyn not support meta object pointer");
                QJsonObject jsonObject;
                QtJsonSerializeDyn(jsonObject, *clazz, inValueRef);
                outJsonValue = std::move(jsonObject);
            } else {
                const Mirror::TypeId typeId = inValueRef.TypeId();
                AssertWithReason(simpleTypeDynSerializers.contains(typeId), std::format("QtJsonSerializeValueDyn not support type {}", inValueRef.Type()->name));
                simpleTypeDynSerializers.at(typeId)(outJsonValue, inValueRef);
            }
        }

        static void DeserializeValueDyn(const QJsonValue& inJsonValue, const Mirror::Any& outValueRef)
        {
            static auto simpleTypeDynDeserializers = BuildSimpleTypeDynDeserializers();
            static auto templateViewDynDeserializers = BuildTemplateViewDynDeserializers();

            if (outValueRef.HasTemplateView()) {
                const Mirror::TemplateViewId templateViewId = outValueRef.GetTemplateViewId();
                AssertWithReason(templateViewDynDeserializers.contains(templateViewId), std::format("QtJsonSerializeValueDyn not support type {}", outValueRef.Type()->name));
                templateViewDynDeserializers.at(templateViewId)(inJsonValue, outValueRef);
            } else if (const Mirror::Class* clazz = outValueRef.GetDynamicClass(); clazz != nullptr) {
                AssertWithReason(!outValueRef.Type()->isPointer, "QtJsonSerializeValueDyn not support meta object pointer");
                if (!inJsonValue.isObject()) {
                    return;
                }
                const QJsonObject jsonObject = inJsonValue.toObject();
                QtJsonDeserializeDyn(jsonObject, *clazz, outValueRef);
            } else {
                const Mirror::TypeId typeId = outValueRef.TypeId();
                AssertWithReason(simpleTypeDynDeserializers.contains(typeId), std::format("QtJsonSerializeValueDyn not support type {}", outValueRef.Type()->name));
                simpleTypeDynDeserializers.at(typeId)(inJsonValue, outValueRef);
            }
        }
    };
}
