//
// Created by Zach Lee on 2022/5/29.
//

#pragma once

#include <Common/Math/MathUtil.h>

namespace Common {

    struct Box {
        Vector3 min;
        Vector3 max;

        Box Combine(const Box &rhs) const
        {
            Box res = {};
            res.min = Min(min, rhs.min);
            res.max = Max(max, rhs.max);
            return res;
        }

        template <class Archive>
        void serialize(Archive &ar)
        {
            ar(min, max);
        }
    };

} // namespace Common
