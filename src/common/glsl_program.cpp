#include "common/glsl_program.h"

#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

namespace glsl_shader
{
    static std::unordered_map<std::string, ShaderType> s_extension_map
    {
        { ".vs.glsl", ShaderType::Vertex },
        { ".fs.glsl", ShaderType::Fragment},
        { ".gs.glsl", ShaderType::Geometry },
        { ".tcs.glsl", ShaderType::TessControl},
        { ".tes.glsl", ShaderType::TessEvaluation },
        { ".cs.glsl", ShaderType::Compute},
    };

    GLSLProgramException::GLSLProgramException(const std::string& message)
        : std::runtime_error(message)
    {

    }

    GLSLProgram::GLSLProgram()
        : m_handle(0),
          m_is_linked(false)
    {

    }

    GLSLProgram::~GLSLProgram()
    {
        if (m_handle == 0)
        {
            return;
        }

        DetachAndDeleteShaderObjects();
        glDeleteProgram(m_handle);
    }

    void GLSLProgram::CompileShader(const std::filesystem::path& shader_file_path)
    {
        std::string extension = shader_file_path.extension().string();
        std::filesystem::path temp = shader_file_path.stem();
        extension = temp.extension().string() + extension;
        std::unordered_map<std::string, ShaderType>::iterator shader_type_it = s_extension_map.find(extension);
        if (shader_type_it != s_extension_map.end())
        {
            CompileShader(shader_file_path, shader_type_it->second);
        }
        else
        {
            std::string message = "无效的着色器文件后缀: " + extension;
            throw GLSLProgramException(message);
        }
    }

    void GLSLProgram::CompileShader(const std::filesystem::path& shader_file_path, ShaderType shader_type)
    {
        if (!FileExists(shader_file_path.string()))
        {
            std::string message = "着色器文件不存在: " + shader_file_path.string();
            throw GLSLProgramException(message);
        }

        if (m_handle <= 0)
        {
            m_handle = glCreateProgram();
            if (m_handle <= 0)
            {
                throw GLSLProgramException("创建着色器程序失败");
            }
        }

        std::ifstream shader_file(shader_file_path, std::ios::in);
        if (!shader_file.is_open())
        {
            std::string message = "无法打开着色器文件: " + shader_file_path.string();
            throw GLSLProgramException(message);
        }

        std::stringstream code;
        code << shader_file.rdbuf();
        shader_file.close();

        CompileShader(code.str(), shader_type);
    }

    void GLSLProgram::CompileShader(const std::string& source, ShaderType shader_type)
    {
        if (m_handle <= 0)
        {
            m_handle = glCreateProgram();
            if (m_handle <= 0)
            {
                throw GLSLProgramException("创建着色器程序失败");
            }
        }

        GLuint shader = glCreateShader(static_cast<unsigned int>(shader_type));
        const char* code = source.c_str();
        glShaderSource(shader, 1, &code, nullptr);

        glCompileShader(shader);
        int result = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            std::string message = "编译着色器失败";
            GLint log_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
            if (log_length > 0)
            {
                std::string log(log_length, '\0');
                GLsizei written_length = 0;
                glGetShaderInfoLog(shader, log_length, &written_length, log.data());
                message += log;
            }
            throw GLSLProgramException(message);
        }
        else
        {
            glAttachShader(m_handle, shader);
        }
    }

    void GLSLProgram::Link()
    {
        if (m_is_linked)
        {
            return;
        }

        if (m_handle <= 0)
        {
            throw GLSLProgramException("着色器程序不完整");
        }

        glLinkProgram(m_handle);
        int result = 0;
        std::string message = "链接着色器失败";
        glGetProgramiv(m_handle, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            GLint log_length = 0;
            glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &log_length);
            if (log_length > 0)
            {
                std::string log(log_length, '\0');
                GLsizei written_length = 0;
                glGetProgramInfoLog(m_handle, log_length, &written_length, log.data());
                message += log;
            }
        }
        else
        {
            FindUniformLocations();
            m_is_linked = true;
        }

        DetachAndDeleteShaderObjects();

        if (result == GL_FALSE)
        {
            throw GLSLProgramException(message);
        }
    }

    void GLSLProgram::Validate()
    {
        if (!IsLinked())
        {
            throw GLSLProgramException("着色器程序未链接");
        }

        GLint status = 0;
        glValidateProgram(m_handle);
        glGetProgramiv(m_handle, GL_VALIDATE_STATUS, &status);

        if (GL_FALSE == status)
        {
            int length = 0;
            std::string message;

            glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);

            if (length > 0)
            {
                char* log = new char[length];
                int written = 0;
                glGetProgramInfoLog(m_handle, length, &written, log);
                message = log;
                delete[] log;
            }

            throw GLSLProgramException("Program failed to validate:\n    " + message);
        }
    }

    void GLSLProgram::Use()
    {
        if (m_handle <= 0 || m_is_linked == false)
        {
            throw GLSLProgramException("着色器程序还未链接");
        }
        glUseProgram(m_handle);
    }

    int GLSLProgram::GetHandle()
    {
        return m_handle;
    }

    bool GLSLProgram::IsLinked()
    {
        return m_is_linked;
    }

    void GLSLProgram::BindAttribLocation(GLuint location, const char* name)
    {
        glBindAttribLocation(m_handle, location, name);
    }

    void GLSLProgram::BindFragDataLocation(GLuint location, const char* name)
    {
        glBindFragDataLocation(m_handle, location, name);
    }

    void GLSLProgram::SetUniform(const char* name, float x, float y, float z)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniform3f(location, x, y, z);
    }

    void GLSLProgram::SetUniform(const char* name, const glm::vec2& value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void GLSLProgram::SetUniform(const char* name, const glm::vec3& value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void GLSLProgram::SetUniform(const char* name, const glm::vec4& value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void GLSLProgram::SetUniform(const char* name, const glm::mat3& value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void GLSLProgram::SetUniform(const char* name, const glm::mat4& value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void GLSLProgram::SetUniform(const char* name, float value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniform1f(location, value);
    }

    void GLSLProgram::SetUniform(const char* name, bool value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniform1i(location, value);
    }

    void GLSLProgram::SetUniform(const char* name, int value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniform1i(location, value);
    }

    void GLSLProgram::SetUniform(const char* name, GLuint value)
    {
        GLint location = glGetUniformLocation(m_handle, name);
        glUniform1ui(location, value);
    }

    void GLSLProgram::FindUniformLocations()
    {
        m_uniform_locations.clear();

        GLint uniforms_num = 0;
        glGetProgramInterfaceiv(m_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniforms_num);

        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
        for (GLint i = 0; i < uniforms_num; ++i)
        {
            GLint result[4] = { 0, 0, 0, 0 };
            glGetProgramResourceiv(m_handle, GL_UNIFORM, i, 4, properties, 4, nullptr, result);

            // 跳过在 uniform block 中的 uniform 变量
            if (result[3] != -1)
            {
                continue;
            }
            GLint name_buffer_size = result[0] + 1;
            char* name = new char[name_buffer_size];
            glGetProgramResourceName(m_handle, GL_UNIFORM, i, name_buffer_size, nullptr, name);
            m_uniform_locations[name] = result[2];
            delete[] name;
        }
    }

    void GLSLProgram::PrintActiveUniforms()
    {
        GLint uniforms_num = 0;
        glGetProgramInterfaceiv(m_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniforms_num);

        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

        std::cout << "Active uniforms:" << std::endl;
        for (int i = 0; i < uniforms_num; ++i)
        {
            GLint result[4] = { 0, 0, 0, 0 };
            glGetProgramResourceiv(m_handle, GL_UNIFORM, i, 4, properties, 4, nullptr, result);

            // 跳过在 uniform block 中的 uniform 变量
            if (result[3] != -1)
            {
                continue;
            }
            GLint name_buffer_size = result[0] + 1;
            char* name = new char[name_buffer_size];
            glGetProgramResourceName(m_handle, GL_UNIFORM, i, name_buffer_size, nullptr, name);
            std::cout << "    location: " << result[2] << ", name: " << name << ", type: " << GetTypeString(result[1]) << std::endl;
            delete[] name;
        }
    }

    void GLSLProgram::PrintActiveUniformBlocks()
    {
        GLint blocks_num = 0;
        glGetProgramInterfaceiv(m_handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &blocks_num);

        GLenum block_properties[] = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
        GLenum block_index[] = { GL_ACTIVE_VARIABLES };
        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };

        std::cout << "Active uniform blocks:" << std::endl;;
        for (int block = 0; block < blocks_num; ++block)
        {
            GLint block_info[2] = { 0, 0 };
            glGetProgramResourceiv(m_handle, GL_UNIFORM_BLOCK, block, 2, block_properties, 2, nullptr, block_info);
            GLint uniforms_num = block_info[0];

            char* block_name = new char[block_info[1] + 1];
            glGetProgramResourceName(m_handle, GL_UNIFORM_BLOCK, block, block_info[1] + 1, nullptr, block_name);
            std::cout << "    Uniform block: " << block_name << std::endl;
            delete[] block_name;

            GLint* uniform_indices = new GLint[uniforms_num];
            glGetProgramResourceiv(m_handle, GL_UNIFORM_BLOCK, block, 1, block_index, uniforms_num, nullptr, uniform_indices);

            for (int i = 0; i < uniforms_num; ++i)
            {
                GLint uniform_index = uniform_indices[i];
                GLint result[3] = { 0, 0, 0 };
                glGetProgramResourceiv(m_handle, GL_UNIFORM, uniform_index, 3, properties, 3, nullptr, result);

                GLint name_buffer_size = result[0] + 1;
                char* name = new char[name_buffer_size];
                glGetProgramResourceName(m_handle, GL_UNIFORM, uniform_index, name_buffer_size, nullptr, name);
                std::cout << "        name: " << name << ", type: " << GetTypeString(result[1]) << std::endl;
                delete[] name;
            }

            delete[] uniform_indices;
        }
    }

    void GLSLProgram::PrintActiveAttribs()
    {
        GLint attribs_num = 0;
        glGetProgramInterfaceiv(m_handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &attribs_num);

        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

        std::cout << "Active attributes:" << std::endl;
        for (int i = 0; i < attribs_num; ++i)
        {
            GLint result[3] = { 0, 0, 0 };
            glGetProgramResourceiv(m_handle, GL_PROGRAM_INPUT, i, 3, properties, 3, nullptr, result);

            GLint name_buffer_size = result[0] + 1;
            char* name = new char[name_buffer_size];
            glGetProgramResourceName(m_handle, GL_PROGRAM_INPUT, i, name_buffer_size, NULL, name);
            std::cout << "    location: " << result[2] << ", name: " << name << ", type: " << GetTypeString(result[1]) << std::endl;
            delete[] name;
        }
    }

    GLint GLSLProgram::GetUniformLocation(const char* name)
    {
        std::unordered_map<std::string, int>::iterator position = m_uniform_locations.find(name);

        if (position == m_uniform_locations.end()) {
            GLint location = glGetUniformLocation(m_handle, name);
            m_uniform_locations[name] = location;
            return location;
        }

        return position->second;
    }

    void GLSLProgram::DetachAndDeleteShaderObjects()
    {
        GLint shaders_num = 0;
        glGetProgramiv(m_handle, GL_ATTACHED_SHADERS, &shaders_num);
        std::vector<GLuint> shaders(shaders_num);
        glGetAttachedShaders(m_handle, shaders_num, nullptr, shaders.data());
        for (GLuint shader : shaders)
        {
            glDetachShader(m_handle, shader);
            glDeleteShader(shader);
        }
    }

    bool GLSLProgram::FileExists(const std::string& filename)
    {
        struct stat info;
        int ret = -1;

        ret = stat(filename.c_str(), &info);
        return 0 == ret;
    }

    const char* GLSLProgram::GetTypeString(GLenum type)
    {
        switch (type)
        {
          case GL_FLOAT:
            return "float";
          case GL_FLOAT_VEC2:
            return "vec2";
          case GL_FLOAT_VEC3:
            return "vec3";
          case GL_FLOAT_VEC4:
            return "vec4";
          case GL_DOUBLE:
            return "double";
          case GL_INT:
            return "int";
          case GL_UNSIGNED_INT:
            return "unsigned int";
          case GL_BOOL:
            return "bool";
          case GL_FLOAT_MAT2:
            return "mat2";
          case GL_FLOAT_MAT3:
            return "mat3";
          case GL_FLOAT_MAT4:
            return "mat4";
          default:
            return "?";
        }
    }
}