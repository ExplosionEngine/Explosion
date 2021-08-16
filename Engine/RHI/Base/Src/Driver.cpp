//
// Created by John Kindem on 2021/5/16 0016.
//

#include <RHI/Driver.h>

namespace Explosion::RHI {
    Driver::Driver() = default;

    Driver::~Driver() = default;

    DriverFactory& DriverFactory::Singleton()
    {
        static DriverFactory instance;
        return instance;
    }

    Driver* DriverFactory::CreateFromLib(const std::string& name)
    {
        using CreateDriverFunc = Explosion::RHI::Driver*(*)();

        auto* dynLib = FetchOrLoadLib(name);
        if (dynLib == nullptr) {
            return nullptr;
        }
        CreateDriverFunc createDriverFunc = reinterpret_cast<CreateDriverFunc>(dynLib->GetSymbol("CreateDriver"));
        if (createDriverFunc == nullptr) {
            return nullptr;
        }
        return createDriverFunc();
    }

    DynLib* DriverFactory::FetchOrLoadLib(const std::string& name)
    {
        auto iter = libs.find(name);
        if (iter != libs.end()) {
            return iter->second.get();
        }
        libs[name] = std::make_unique<DynLib>(name);
        auto& lib = libs[name];
        lib->Load();
        return lib.get();
    }
}
