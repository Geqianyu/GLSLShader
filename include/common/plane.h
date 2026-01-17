#ifndef __GLSL_SHADER_COMMON_PLANE_H__
#define __GLSL_SHADER_COMMON_PLANE_H__

#include "common/triangle_mesh.h"

namespace glsl_shader
{
    class Plane
    {
    public:
        Plane(float xsize, float zsize, int xdivs, int zdivs, float smax = 1.0f, float tmax = 1.0f);
        ~Plane();

        void Render();

    private:
        TriangleMesh m_mesh;
    };
}

#endif // !__GLSL_SHADER_COMMON_PLANE_H__
