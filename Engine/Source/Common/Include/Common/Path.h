//
// Created by johnk on 2022/7/25.
//

#include <unordered_map>

#include <Common/Utility.h>

namespace Common {
    class PathMapper {
    public:
        static PathMapper From(const std::unordered_map<std::string, std::string>& map)
        {
            // TODO
            return PathMapper();
        }

        ~PathMapper() = default;

        std::string Map(const std::string& path) const
        {
            // TODO
            return "";
        }

    private:
        PathMapper() = default;

        PathMapper(const std::vector<std::pair<std::string, std::string>>& inMappers)
        {
            // TODO
        }

        NON_COPYABLE(PathMapper)

        std::vector<std::pair<std::string, std::string>> mappers;
    };
}
