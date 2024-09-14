//
// Created by johnk on 2024/8/23.
//

#pragma once

#include <QString>

#include <Core/Paths.h>

namespace Editor {
    class Resource {
    public:
        explicit Resource(std::string inFile);

        std::filesystem::path Path() const;
        std::string String() const;
        operator std::filesystem::path() const; // NOLINT
        operator std::string() const; // NOLINT
        operator QString() const; // NOLINT

    private:
        std::string file;
    };

    class StaticResources {
    public:
        static Resource picLogo;
    };
}
