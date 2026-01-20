#ifndef __GLSL_SHADER_COMMON_CUBE_H__
#define __GLSL_SHADER_COMMON_CUBE_H__

#include "common/triangle_mesh.h"

namespace glsl_shader
{
    class Cube
    {
    public:
        Cube(float size = 1.0f);
        ~Cube();

        void Render();

    private:
        TriangleMesh m_mesh;
    };
}

#endif // !__GLSL_SHADER_COMMON_CUBE_H__