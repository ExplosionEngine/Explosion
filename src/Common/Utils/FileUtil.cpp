#include <Common/Utils/FileUtil.h>
using namespace Explosion;

#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

const char* FileUtil::ReadContent(const char *file)
{
    string content;
    ifstream stream;
    stream.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        stream.open(file);
        std::stringstream stringStream;
        stringStream << stream.rdbuf();
        stream.close();
        content = stringStream.str();
    } catch (ifstream::failure &e) {
        cout << "failed to read file: " << file << endl;
    }
    return content.c_str();
}