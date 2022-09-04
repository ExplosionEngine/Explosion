//
// Created by johnk on 2022/9/4.
//

#include <Mirror/Type.h>
#include <Common/Hash.h>

namespace Mirror {
    TypeHash CalculateTypeHash(const std::string& name)
    {
        return Common::HashUtils::CityHash(name.data(), name.size());
    }
}
