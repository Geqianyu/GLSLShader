#ifndef __GLSL_SHADER_COMMON_TORUS_H__
#define __GLSL_SHADER_COMMON_TORUS_H__

#include "common/triangle_mesh.h"

namespace glsl_shader
{
    class Torus
    {
    public:
        Torus(GLfloat outer_radius, GLfloat inner_radius, GLuint sides_count, GLuint rings_count);
        ~Torus();

        void Render();

    private:
        TriangleMesh m_mesh;
    };
}

#endif // !__GLSL_SHADER_COMMON_TORUS_H__
