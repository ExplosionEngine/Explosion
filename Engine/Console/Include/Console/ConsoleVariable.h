//
// Created by johnk on 2021/12/25.
//

#ifndef EXPLOSION_CONSOLE_CONSOLE_VARIABLE_H
#define EXPLOSION_CONSOLE_CONSOLE_VARIABLE_H

#include <string>
#include <utility>

#include <Console/Console.h>

namespace Console {
    class IConsoleVariable {
    public:
        virtual bool GetValueBool() = 0;
        virtual int GetValueInt() = 0;
        virtual float GetValueFloat() = 0;
        virtual void SetValueBool(bool v) = 0;
        virtual void SetValueInt(int v) = 0;
        virtual void SetValueFloat(float v) = 0;

        [[nodiscard]] const std::string& GetName() const noexcept
        {
            return name;
        }

        [[nodiscard]] const std::string& GetDescription() const noexcept
        {
            return description;
        }

    protected:
        IConsoleVariable(std::string name, std::string desc) : name(std::move(name)), description(std::move(desc))
        {
            Console::Singleton().RegisterVariable(name, this);
        }

        ~IConsoleVariable() = default;

        std::string name;
        std::string description;
    };

    template <typename T>
    class ConsoleVariable : public IConsoleVariable {
    public:
        ConsoleVariable(std::string name, T defaultValue, std::string desc)
            : IConsoleVariable(std::move(name), std::move(desc)), value(defaultValue) {}

        ~ConsoleVariable() = default;

        bool GetValueBool() override
        {
            return static_cast<bool>(value);
        }

        int GetValueInt() override
        {
            return static_cast<int>(value);
        }

        float GetValueFloat() override
        {
            return static_cast<float>(value);
        }

        void SetValueBool(bool v) override
        {
            value = v;
        }

        void SetValueInt(int v) override
        {
            value = v;
        }

        void SetValueFloat(float v) override
        {
            value = v;
        }

    private:
        T value;
    };

    template <typename T>
    class ConsoleVariableRef : public IConsoleVariable {
    public:
        ConsoleVariableRef(std::string name, T& value, std::string desc)
            : IConsoleVariable(std::move(name), std::move(desc)), value(value) {}

        ~ConsoleVariableRef() = default;

        bool GetValueBool() override
        {
            return static_cast<bool>(value);
        }

        int GetValueInt() override
        {
            return static_cast<int>(value);
        }

        float GetValueFloat() override
        {
            return static_cast<float>(value);
        }

        void SetValueBool(bool v) override
        {
            value = v;
        }

        void SetValueInt(int v) override
        {
            value = v;
        }

        void SetValueFloat(float v) override
        {
            value = v;
        }

    private:
        T& value;
    };
}

#endif //EXPLOSION_CONSOLE_CONSOLE_VARIABLE_H
