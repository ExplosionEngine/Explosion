//
// Created by johnk on 2022/9/10.
//

#include <Mirror/Registry.h>
#include <Common/Debug.h>

#include <utility>

namespace Mirror {
    Field::Field(std::string inName) : name(std::move(inName)) {}

    Field::~Field() = default;

    void Field::SetMeta(const std::string& key, const std::string& value)
    {
        metas[key] = value;
    }

    const std::string& Field::GetMeta(const std::string& key) const
    {
        auto iter = metas.find(key);
        Assert(iter != metas.end());
        return iter->second;
    }

    bool Field::HasMeta(const std::string& key) const
    {
        return metas.find(key) != metas.end();
    }

    Variable::Variable(std::string inName, Variable::Setter inSetter, Variable::Getter inGetter)
        : Field(std::move(inName))
        , setter(std::move(inSetter))
        , getter(std::move(inGetter)) {}

    Variable::~Variable() = default;

    void Variable::Set(Any* value)
    {
        setter(value);
    }

    Any Variable::Get()
    {
        return getter();
    }
}
