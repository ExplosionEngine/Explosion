//
// Created by johnk on 2022/9/10.
//

#pragma once

#include <vector>
#include <unordered_map>

#include <Mirror/Api.h>
#include <Mirror/Type.h>
#include <Mirror/Any.h>

namespace Mirror {
    class Field {
    public:
        virtual ~Field();

        const std::string& GetMeta(const std::string& key) const;
        bool HasMeta(const std::string& key) const;

    protected:
        explicit Field(std::string inName);

    private:
        friend class Registry;

        void SetMeta(const std::string& key, const std::string& value);

        std::string name;
        std::unordered_map<std::string, std::string> metas;
    };

    class Variable : public Field {
    public:
        ~Variable() override;

        template <typename T>
        void Set(T&& value)
        {
            Any ref = Any(std::ref(std::forward<T>(value)));
            Set(&ref);
        }

        void Set(Any* value);
        Any Get();

    private:
        using Setter = std::function<void(Any*)>;
        using Getter = std::function<Any()>;

        Variable(std::string inName, Setter inSetter, Getter inGetter);

        Setter setter;
        Getter getter;
    };
}

namespace Mirror {
    class MIRROR_API Registry {
    public:
        static Registry Get()
        {
            static Registry instance;
            return instance;
        }

        // TODO

    private:
    };
}
