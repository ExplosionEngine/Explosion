#include <Render/Shader/Shader.h>
#include <Common/Utils/FileUtil.h>
using namespace Explosion;

Shader::Shader()
{
    DeleteProgram();
    programId = glCreateProgram();
}

Shader::~Shader()
{
    DeleteVertex();
    DeleteFragment();
    DeleteProgram();
}

Shader * Shader::Create()
{
    return new Shader();
}

void Shader::Destroy(Shader *shader)
{
    delete shader;
}

void Shader::DeleteVertex()
{
    if (vertexShaderId != GL_NONE) {
        glDeleteShader(vertexShaderId);
    }
}

void Shader::DeleteFragment()
{
    if (fragmentShaderId != GL_NONE) {
        glDeleteShader(fragmentShaderId);
    }
}

void Shader::DeleteProgram()
{
    if (programId != GL_NONE) {
        glDeleteProgram(programId);
    }
}

bool Shader::CompileVertex(const char* source)
{
    DeleteVertex();
    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &source, nullptr);
    glCompileShader(vertexShaderId);

    GLint success = GL_FALSE;
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShaderId, INFO_BUFFER_SIZE, nullptr, info);
    }
    return success;
}

bool Shader::CompileFragment(const char* source)
{
    DeleteFragment();
    fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &source, nullptr);
    glCompileShader(fragmentShaderId);

    GLint success = GL_FALSE;
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderId, INFO_BUFFER_SIZE, nullptr, info);
    }
    return success;
}

bool Shader::Link()
{
    DeleteProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    GLint success = GL_FALSE;
    glGetProgramiv(programId, INFO_BUFFER_SIZE, &success);
    if (!success) {
        glGetProgramInfoLog(programId, INFO_BUFFER_SIZE, nullptr, info);
    }
    return success;
}

char * Shader::GetInfo()
{
    return info;
}