//
// Created by johnk on 2022/7/25.
//

#include <string>
#include <fstream>

#include <Common/Debug.h>

namespace Common {
    class FileUtils {
    public:
        static std::string ReadTextFile(const std::string& fileName)
        {
            std::string result;
            {
                std::ifstream file(fileName, std::ios::ate | std::ios::binary);
                Assert(file.is_open());
                size_t size = file.tellg();
                result.resize(size);
                file.seekg(0);
                file.read(result.data(), static_cast<std::streamsize>(size));
                file.close();
            }
            return result;
        }
    };
}
