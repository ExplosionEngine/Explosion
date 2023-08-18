//
// Created by johnk on 2023/7/25.
//

#pragma once

#include <clipp.h>

#include <string>
#include <unordered_map>

#include <Core/Api.h>

namespace Core {
    class CORE_API CmdlineArg {
    public:
        virtual ~CmdlineArg();

        template <typename T>
        T GetValue()
        {
            return static_cast<const T*>(Value());
        }

    protected:
        friend class Cli;

        virtual const void* Value() = 0;
        virtual clipp::group CreateClippParameter() = 0;
    };

    class CORE_API Cli {
    public:
        static Cli& Get();
        ~Cli();

        std::pair<bool, std::string> Parse(int argc, char* argv[]);

    private:
        template <typename T> friend class CmdlineArgValue;
        template <typename T> friend class CmdlineArgRef;

        Cli();

        bool parsed;
        std::vector<CmdlineArg*> args;
    };

    template <typename T>
    class CmdlineArgValue : public CmdlineArg {
    public:
        CmdlineArgValue(std::string inName, std::string inOption, T inDefaultValue, std::string inDoc)
            : value(inDefaultValue)
            , name(std::move(inName))
            , option(std::move(inOption))
            , doc(std::move(inDoc))
        {
            Cli::Get().args.emplace_back(this);
        }

        ~CmdlineArgValue() override = default;

        T GetValue() const
        {
            return value;
        }

    protected:
        const void* Value() override
        {
            return &value;
        }

        clipp::group CreateClippParameter() override
        {
            if constexpr (std::is_same_v<T, bool>) {
                return clipp::group(clipp::option(option).set(value).doc(doc));
            } else {
                return clipp::option(option).doc(doc) & clipp::value(name, value);
            }
        }

    private:
        T value;
        std::string name;
        std::string option;
        std::string doc;
    };

    template <typename T>
    class CmdlineArgRef : public CmdlineArg {
    public:
        CmdlineArgRef(std::string inName, std::string inOption, T& inValueRef, std::string inDoc)
            : valueRef(inValueRef)
            , name(std::move(inName))
            , option(std::move(inOption))
            , doc(std::move(inDoc))
        {
            Cli::Get().args.emplace_back(this);
        }

        ~CmdlineArgRef() override = default;

        T GetValue() const
        {
            return valueRef;
        }

    protected:
        const void* Value() override
        {
            return &valueRef;
        }

        clipp::group CreateClippParameter() override
        {
            if constexpr (std::is_same_v<T, bool>) {
                return clipp::group(clipp::option(option).set(valueRef).doc(doc));
            } else {
                return clipp::option(option).doc(doc) & clipp::value(name, valueRef);
            }
        }

    private:
        T& valueRef;
        std::string name;
        std::string option;
        std::string doc;
    };
}
