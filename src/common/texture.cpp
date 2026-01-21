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

    GLuint Texture::LoadCubeMap(const std::string& base_name, const std::string& extension)
    {
        GLuint texture = 0;
        const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
        GLint width = 0;
        GLint height = 0;
        int channels = 0;

        std::string texture_name = base_name + "_" + suffixes[0] + extension;
        stbi_set_flip_vertically_on_load(0);
        unsigned char* data = stbi_load(texture_name.c_str(), &width, &height, &channels, 4);
        if (data)
        {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

            glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, width, height);
            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
            data = nullptr;

            for (int i = 1; i < 6; ++i)
            {
                texture_name = base_name + "_" + suffixes[i] + extension;
                data = stbi_load(texture_name.c_str(), &width, &height, &channels, 4);
                if (data)
                {
                    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                }
                stbi_image_free(data);
                data = nullptr;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texture;
    }

    GLuint Texture::LoadHdrCubeMap(const std::string& base_name)
    {
        GLuint texture = 0;
        const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
        GLint width = 0;
        GLint height = 0;
        int channels = 0;

        std::string texture_name = base_name + "_" + suffixes[0] + ".hdr";
        float* data = stbi_loadf(texture_name.c_str(), &width, &height, &channels, 3);
        if (data)
        {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
            glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB32F, width, height);
            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);
            stbi_image_free(data);
            data = nullptr;

            for (int i = 1; i < 6; ++i)
            {
                texture_name = base_name + "_" + suffixes[i] + ".hdr";
                data = stbi_loadf(texture_name.c_str(), &width, &height, NULL, 3);
                glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);
                stbi_image_free(data);
                data = nullptr;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texture;
    }
}