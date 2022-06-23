//
// Created by johnk on 2022/6/23.
//

#pragma once

#include <cstdint>

#include <Common/Meta.h>

struct Meta(Class) TestShaderVariantSet {
    Meta(Property, ShaderVariant(Macro = "TEST_BOOL"))
    bool testBool = false;

    Meta(Property, ShaderVariant(Macro = "TEST_RANGED_INT", RangeFrom = 0, RangeTo = 2))
    int32_t testRangedInt = 0;
};
