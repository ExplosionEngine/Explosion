//
// Created by johnk on 2024/8/23.
//

#include <Editor/Resource.h>
#include <Common/Debug.h>

namespace Editor {
    Resource::Resource(std::string inFile)
        : file(std::move(inFile))
    {
    }

    std::filesystem::path Resource::Path() const
    {
        return Core::Paths::EngineRes() / file;
    }

    std::string Resource::String() const
    {
        return Path().string();
    }

    Resource::operator std::filesystem::path() const
    {
        return Path();
    }

    Resource::operator std::string() const
    {
        return String();
    }

    Resource::operator QString() const
    {
        return QString::fromStdString(String());
    }

    Resource StaticResources::picLogo = Resource("logo.png");
}
