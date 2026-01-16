#ifndef __GLSL_SHADER_COMMON_TEAPOT_PATCH_H__
#define __GLSL_SHADER_COMMON_TEAPOT_PATCH_H__

#include "glad/gl.h"
#include "glm/glm.hpp"

namespace glsl_shader
{
    class TeapotPatch
    {
    public:
        TeapotPatch();
        ~TeapotPatch();

        void Render();

    private:
        void GeneratePatch(std::vector<GLfloat>& points);
        void BuildPatchReflect(int patch_count, std::vector<GLfloat>& points, int& index, bool reflect_x, bool reflect_y);
        void BuildPatch(glm::vec3& patch[][4], std::vector<GLfloat>& points, int& index, const glm::vec3& reflect);
        void GetPatch(int patch_count, glm::vec3 patch[][4], bool reverse_v);
        void Init(std::vector<GLfloat>& points);

    private:
        GLuint m_vbo;
        GLuint m_vao;
    };
}

#endif // !__GLSL_SHADER_COMMON_TEAPOT_PATCH_H__