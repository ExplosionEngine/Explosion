#include <Render/Shader.h>
using namespace Explosion;

Shader::~Shader()
{
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    glDeleteShader(programId);
}

Shader * Shader::Create()
{
    return new Shader();
}

void Shader::Destroy(Shader *shader)
{
    delete shader;
}

bool Shader::CompileVertex(const char *file)
{
    // TODO
}

bool Shader::CompileFragment(const char *file)
{
    // TODO
}

bool Shader::Link()
{
    // TODO
}

char * Shader::GetInfo()
{
    return info;
}