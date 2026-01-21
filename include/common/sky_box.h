#ifndef __GLSL_SHADER_COMMON_SKY_BOX_H__
#define __GLSL_SHADER_COMMON_SKY_BOX_H__

#include "common/triangle_mesh.h"

namespace glsl_shader
{
    class SkyBox
    {
    public:
        SkyBox(float size = 50.0f);
        ~SkyBox();

        void Render();

    private:
        TriangleMesh m_mesh;
    };
}

#endif // !__GLSL_SHADER_COMMON_SKY_BOX_H__