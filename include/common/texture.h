#ifndef __GLSL_SHADER_COMMON_TEXTURE_H__
#define __GLSL_SHADER_COMMON_TEXTURE_H__

#include "glad/gl.h"

#include <string>

namespace glsl_shader
{
    class Texture
    {
    public:
        static GLuint LoadTexture(const std::string& filename);
    };
}

#endif // !__GLSL_SHADER_COMMON_TEXTURE_H__
