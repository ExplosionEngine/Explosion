//
// Created by johnk on 2023/7/25.
//

#pragma once

#include <clipp.h>

#include <string>
#include <unordered_map>

#include <Core/Api.h>
#include <Common/Debug.h>

namespace Core {
    class CORE_API CmdlineArg {
    public:
        virtual ~CmdlineArg();

        template <typename T>
        T GetValue();

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
        CmdlineArg* FindArg(const std::string& name) const;
        CmdlineArg& FindArgChecked(const std::string& name) const;

    private:
        template <typename T> friend class CmdlineArgValue;
        template <typename T> friend class CmdlineArgRef;

        Cli();

        bool parsed;
        std::unordered_map<std::string, CmdlineArg*> args;
    };

    template <typename T>
    class CmdlineArgValue final : public CmdlineArg {
    public:
        CmdlineArgValue(std::string inName, std::string inOption, T inDefaultValue, std::string inDoc);
        ~CmdlineArgValue() override;

        T GetValue() const;

    protected:
        const void* Value() override;
        clipp::group CreateClippParameter() override;

    private:
        T value;
        std::string name;
        std::string option;
        std::string doc;
    };

    template <typename T>
    class CmdlineArgRef final : public CmdlineArg {
    public:
        CmdlineArgRef(std::string inName, std::string inOption, T& inValueRef, std::string inDoc);
        ~CmdlineArgRef() override;

        T GetValue() const;

    protected:
        const void* Value() override;
        clipp::group CreateClippParameter() override;

    private:
        T& valueRef;
        std::string name;
        std::string option;
        std::string doc;
    };
}

namespace Core {
    template <typename T>
    T CmdlineArg::GetValue()
    {
        return static_cast<const T*>(Value());
    }

    template <typename T>
    CmdlineArgValue<T>::CmdlineArgValue(std::string inName, std::string inOption, T inDefaultValue, std::string inDoc)
        : value(inDefaultValue)
        , name(std::move(inName))
        , option(std::move(inOption))
        , doc(std::move(inDoc))
    {
        Assert(!Cli::Get().args.contains(name));
        Cli::Get().args.emplace(name, this);
    }

    template <typename T>
    CmdlineArgValue<T>::~CmdlineArgValue() = default;

    template <typename T>
    T CmdlineArgValue<T>::GetValue() const
    {
        return value;
    }

    template <typename T>
    const void* CmdlineArgValue<T>::Value()
    {
        return &value;
    }

    template <typename T>
    clipp::group CmdlineArgValue<T>::CreateClippParameter()
    {
        if constexpr (std::is_same_v<T, bool>) {
            return clipp::group(clipp::option(option).set(value).doc(doc));
        } else {
            return clipp::option(option).doc(doc) & clipp::value(name, value);
        }
    }

    template <typename T>
    CmdlineArgRef<T>::CmdlineArgRef(std::string inName, std::string inOption, T& inValueRef, std::string inDoc)
        : valueRef(inValueRef)
        , name(std::move(inName))
        , option(std::move(inOption))
        , doc(std::move(inDoc))
    {
        Assert(!Cli::Get().args.contains(name));
        Cli::Get().args.emplace(name, this);
    }

    template <typename T>
    CmdlineArgRef<T>::~CmdlineArgRef() = default;

    template <typename T>
    T CmdlineArgRef<T>::GetValue() const
    {
        return valueRef;
    }

    template <typename T>
    const void* CmdlineArgRef<T>::Value()
    {
        return &valueRef;
    }

    template <typename T>
    clipp::group CmdlineArgRef<T>::CreateClippParameter()
    {
        if constexpr (std::is_same_v<T, bool>) {
            return clipp::group(clipp::option(option).set(valueRef).doc(doc));
        } else {
            return clipp::option(option).doc(doc) & clipp::value(name, valueRef);
        }
    }
}
