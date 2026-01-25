#ifndef __GLSL_SHADER_COMMON_SPHERE_H__
#define __GLSL_SHADER_COMMON_SPHERE_H__

#include "common/triangle_mesh.h"

namespace glsl_shader
{
    class Sphere
    {
    public:
        Sphere(float radius, GLuint slices_count, GLuint stacks_count);
        ~Sphere();

        void Render();

    private:
        TriangleMesh m_mesh;
    };
}

#endif // !__GLSL_SHADER_COMMON_SPHERE_H__