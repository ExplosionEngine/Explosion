#ifndef EXPLOSION_SHADER_H
#define EXPLOSION_SHADER_H

#include <glad/glad.h>

static const int INFO_BUFFER_SIZE = 1024;

namespace Explosion {
class Shader {
private:
    Shader();
    ~Shader();

    void DeleteVertex();
    void DeleteFragment();
    void DeleteProgram();

    GLuint programId = GL_NONE;
    GLuint vertexShaderId = GL_NONE;
    GLuint fragmentShaderId = GL_NONE;
    char info[INFO_BUFFER_SIZE] {};
protected:
public:
    static Shader* Create();
    static void Destroy(Shader* shader);

    bool CompileVertex(const char* source);
    bool CompileFragment(const char* source);
    bool Link();
    char* GetInfo();
};
}

#endif
