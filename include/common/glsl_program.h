#ifndef __GLSL_SHADER_COMMON_GLSL_PROGRAM_H__
#define __GLSL_SHADER_COMMON_GLSL_PROGRAM_H__

#include "glad/gl.h"

#include "glm/glm.hpp"

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>

namespace glsl_shader
{
    class GLSLProgramException : public std::runtime_error
    {
    public:
        GLSLProgramException(const std::string& message);
        virtual ~GLSLProgramException() = default;
    };

    enum class ShaderType : unsigned int
    {
        Vertex          = GL_VERTEX_SHADER,
        Fragment        = GL_FRAGMENT_SHADER,
        Geometry        = GL_GEOMETRY_SHADER,
        TessControl     = GL_TESS_CONTROL_SHADER,
        TessEvaluation  = GL_TESS_EVALUATION_SHADER,
        Compute         = GL_COMPUTE_SHADER,
    };

    class GLSLProgram
    {
    public:
        GLSLProgram();
        GLSLProgram(const GLSLProgram&) = delete;
        ~GLSLProgram();

        GLSLProgram& operator = (const GLSLProgram&) = delete;

        void CompileShader(const std::filesystem::path& shader_file_path);
        void CompileShader(const std::filesystem::path& shader_file_path, ShaderType shader_type);
        void CompileShader(const std::string& source, ShaderType shader_type);

        void Link();
        void Validate();
        void Use();

        int GetHandle();
        bool IsLinked();

        void BindAttribLocation(GLuint location, const char* name);
        void BindFragDataLocation(GLuint location, const char* name);

        void SetUniform(const char* name, float x, float y, float z);
        void SetUniform(const char* name, const glm::vec2& value);
        void SetUniform(const char* name, const glm::vec3& value);
        void SetUniform(const char* name, const glm::vec4& value);
        void SetUniform(const char* name, const glm::mat3& value);
        void SetUniform(const char* name, const glm::mat4& value);
        void SetUniform(const char* name, float value);
        void SetUniform(const char* name, bool value);
        void SetUniform(const char* name, int value);
        void SetUniform(const char* name, GLuint value);

        GLuint GetSubroutineIndex(ShaderType shader_type, const char* name);
        void SetSubroutineIndex(ShaderType shader_type, int count, GLuint* indices);

        void FindUniformLocations();
        void PrintActiveUniforms();
        void PrintActiveUniformBlocks();
        void PrintActiveAttribs();

    private:
        GLint GetUniformLocation(const char* name);
        void DetachAndDeleteShaderObjects();
        bool FileExists(const std::string& filename);

    public:
        static const char* GetTypeString(GLenum type);

    private:
        GLuint m_handle;
        bool m_is_linked;
        std::unordered_map<std::string, int> m_uniform_locations;
    };
}

#endif // !__GLSL_SHADER_COMMON_GLSL_PROGRAM_H__