//
// Created by johnk on 2022/9/4.
//

#include <Mirror/Type.h>
#include <Common/Hash.h>

namespace Mirror {
    TypeId ComputeTypeId(std::string_view sigName)
    {
        return Common::HashUtils::CityHash(sigName.data(), sigName.size());
    }
}
