//
// Created by johnk on 2023/10/16.
//

#pragma once

#include <Runtime/Meta.h>
#include <Runtime/Asset/Asset.h>
using namespace Common;
using namespace Runtime;

struct EClass() TestAsset : public Asset {
    EClassBody(TestAsset)

    explicit TestAsset(Core::Uri uri)
        : Asset(std::move(uri))
        , a(0)
        , b()
    {
    }

    TestAsset(Core::Uri inUri, uint32_t inA, std::string inB)
        : Asset(std::move(inUri))
        , a(inA)
        , b(std::move(inB))
    {
    }

    EProperty()
    uint32_t a;

    EProperty()
    std::string b;
};
