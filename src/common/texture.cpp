#include "common/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace glsl_shader
{
    GLuint Texture::LoadTexture(const std::string& filename)
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        stbi_set_flip_vertically_on_load(1);
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
        GLuint texture = 0;
        if (data != nullptr)
        {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            stbi_image_free(data);
        }

        return texture;
    }
}