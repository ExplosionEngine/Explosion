#ifndef EXPLOSION_SHADER_H
#define EXPLOSION_SHADER_H

#include <glad/glad.h>

namespace Explosion {
class Shader {
private:
    Shader() = default;
    ~Shader();

    GLuint programId = GL_NONE;
    GLuint vertexShaderId = GL_NONE;
    GLuint fragmentShaderId = GL_NONE;
    char info[1024] {};
protected:
public:
    static Shader* Create();
    static void Destroy(Shader* shader);

    bool CompileVertex(const char *file);
    bool CompileFragment(const char * file);
    bool Link();
    char* GetInfo();
};
}

#endif
