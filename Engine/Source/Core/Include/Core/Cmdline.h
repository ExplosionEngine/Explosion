//
// Created by johnk on 2023/7/25.
//

#pragma once

#include <clipp.h>

#include <string>
#include <unordered_map>

#include <Core/Api.h>

namespace Core {
    class CORE_API CmdlineArgBase {
    public:
        virtual ~CmdlineArgBase();

    protected:
        friend class Cli;

        virtual clipp::group CreateClippParameter() = 0;
    };

    class CORE_API Cli {
    public:
        static Cli& Get();
        ~Cli();

        std::pair<bool, std::string> Parse(int argc, char* argv[]);

    private:
        template <typename T> friend class CmdlineArg;
        template <typename T> friend class CmdlineArgRef;

        Cli();

        bool parsed;
        std::vector<CmdlineArgBase*> args;
    };

    template <typename T>
    class CmdlineArg : public CmdlineArgBase {
    public:
        CmdlineArg(std::string inName, std::string inOption, T inDefaultValue, std::string inDoc)
            : value(inDefaultValue)
            , name(std::move(inName))
            , option(std::move(inOption))
            , doc(std::move(inDoc))
        {
            Cli::Get().args.emplace_back(this);
        }

        ~CmdlineArg() override = default;

        T GetValue() const
        {
            return value;
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
    class CmdlineArgRef : public CmdlineArgBase {
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
