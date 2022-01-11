//
// Created by johnk on 2021/12/25.
//

#ifndef EXPLOSION_CONSOLE_H
#define EXPLOSION_CONSOLE_H

#include <unordered_map>
#include <string>
#include <utility>

namespace Console {
    class IConsoleVariable;

    class Console {
    public:
        static Console& Singleton()
        {
            static Console instance;
            return instance;
        }

        void RegisterVariable(const std::string& name, IConsoleVariable* var)
        {
            variables[name] = var;
        }

        IConsoleVariable* GetVariable(const std::string& name)
        {
            auto iter = variables.find(name);
            return iter == variables.end() ? nullptr : iter->second;
        }

    private:
        std::unordered_map<std::string, IConsoleVariable*> variables;
    };
}

#endif //EXPLOSION_CONSOLE_H
