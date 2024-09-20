//
// Created by johnk on 2022/5/2.
//

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

#define FMT_HEADER_ONLY 1
#include <fmt/format.h>

#include <Common/Debug.h>
#include <Common/Concepts.h>

namespace Common {
    class StringUtils {
    public:
        static std::wstring ToWideString(const std::string& src);
        static std::string ToByteString(const std::wstring& src);
        static std::string ToUpperCase(const std::string& src);
        static std::string ToLowerCase(const std::string& src);
        static std::string Replace(const std::string& src, const std::string& match, const std::string& replace);
        static std::vector<std::string> Split(const std::string& src, const std::string& split);
        static bool RegexMatch(const std::string& src, const std::string& regex);
        static std::string RegexSearchFirst(const std::string& src, const std::string& regex);
        static std::vector<std::string> RegexSearch(const std::string& src, const std::string& regex);
        static std::string AfterFirst(const std::string& src, const std::string& split);
        static std::string BeforeFirst(const std::string& src, const std::string& split);
        static std::string AfterLast(const std::string& src, const std::string& split);
        static std::string BeforeLast(const std::string& src, const std::string& split);
    };

    template <typename T> struct StringConverter {};
    template <typename T> concept StringConvertible = requires (T inValue) { { StringConverter<T>::ToString(inValue) } -> std::convertible_to<std::string>; };
    template <typename T> std::string ToString(const T& inValue);
}

namespace Common {
    template <typename T>
    std::string ToString(const T& inValue)
    {
        if constexpr (StringConvertible<T>) {
            return StringConverter<T>::ToString(inValue);
        } else {
            QuickFailWithReason("maybe you forget to specific string cast to your type ?");
            return "";
        }
    }

    template <>
    struct StringConverter<bool> {
        static std::string ToString(const bool& inValue)
        {
            return inValue ? "true" : "false";
        }
    };

    template <CppArithmeticNonBool T>
    struct StringConverter<T> {
        static std::string ToString(const T& inValue)
        {
            return std::to_string(inValue);
        }
    };

    template <>
    struct StringConverter<std::string> {
        static std::string ToString(const std::string& inValue)
        {
            return inValue;
        }
    };

    template <StringConvertible T>
    struct StringConverter<std::optional<T>> {
        static std::string ToString(const std::optional<T>& inValue)
        {
            return inValue.has_value()
                ? StringConverter<T>::ToString(inValue.value())
                : "nullopt";
        }
    };

    template <StringConvertible K, StringConvertible V>
    struct StringConverter<std::pair<K, V>> {
        static std::string ToString(const std::pair<K, V>& inValue)
        {
            return fmt::format(
                "{}: {}",
                StringConverter<K>::ToString(inValue.first),
                StringConverter<V>::ToString(inValue.second));
        }
    };

    template <StringConvertible T>
    struct StringConverter<std::vector<T>> {
        static std::string ToString(const std::vector<T>& inValue)
        {
            std::stringstream stream;
            stream << "(";
            for (auto i = 0; i < inValue.size(); i++) {
                stream << StringConverter<T>::ToString(inValue[i]);
                if (i != inValue.size() - 1) {
                    stream << ", ";
                }
            }
            stream << ")";
            return stream.str();
        }
    };

    template <StringConvertible T>
    struct StringConverter<std::unordered_set<T>> {
        static std::string ToString(const std::unordered_set<T>& inValue)
        {
            std::vector<const T*> temp;
            temp.reserve(inValue.size());
            for (auto iter = inValue.begin(); iter != inValue.end(); ++iter) {
                temp.emplace_back(&*iter);
            }

            std::stringstream stream;
            stream << "(";
            for (auto i = 0; i < temp.size(); i++) {
                stream << StringConverter<T>::ToString(*temp[i]);
                if (i != temp.size() - 1) {
                    stream << ", ";
                }
            }
            stream << ")";
            return stream.str();
        }
    };

    template <StringConvertible K, StringConvertible V>
    struct StringConverter<std::unordered_map<K, V>> {
        static std::string ToString(const std::unordered_map<K, V>& inValue)
        {
            std::vector<std::pair<const K*, const V*>> temp;
            temp.reserve(inValue.size());
            for (auto iter = inValue.begin(); iter != inValue.end(); ++iter) {
                temp.emplace_back(std::make_pair(&iter->first, &iter->second));
            }

            std::stringstream stream;
            stream << "{";
            for (auto i = 0; i < temp.size(); i++) {
                stream << fmt::format(
                    "{}: {}",
                    StringConverter<K>::ToString(*temp[i].first),
                    StringConverter<V>::ToString(*temp[i].second));
                if (i != temp.size() - 1) {
                    stream << ", ";
                }
            }
            stream << "}";
            return stream.str();
        }
    };
}
