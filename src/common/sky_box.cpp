#include "common/sky_box.h"

#include <vector>

namespace glsl_shader
{
    SkyBox::SkyBox(float size)
    {
        float side2 = size * 0.5f;
        std::vector<GLfloat> positions
        {
            // Front
            -side2, -side2,  side2,
             side2, -side2,  side2,
             side2,  side2,  side2,
            -side2,  side2,  side2,
            // Right
             side2, -side2,  side2,
             side2, -side2, -side2,
             side2,  side2, -side2,
             side2,  side2,  side2,
            // Back
            -side2, -side2, -side2,
            -side2,  side2, -side2,
             side2,  side2, -side2,
             side2, -side2, -side2,
            // Left
            -side2, -side2,  side2,
            -side2,  side2,  side2,
            -side2,  side2, -side2,
            -side2, -side2, -side2,
            // Bottom
            -side2, -side2,  side2,
            -side2, -side2, -side2,
             side2, -side2, -side2,
             side2, -side2,  side2,
            // Top
            -side2,  side2,  side2,
             side2,  side2,  side2,
             side2,  side2, -side2,
            -side2,  side2, -side2,
        };

        std::vector<GLfloat> normals(positions.size(), 0.0f);

        std::vector<GLuint> indices
        {
             0,  2,  1,  0,  3,  2,
             4,  6,  5,  4,  7,  6,
             8, 10,  9,  8, 11, 10,
            12, 14, 13, 12, 15, 14,
            16, 18, 17, 16, 19, 18,
            20, 22, 21, 20, 23, 22,
        };

        m_mesh.Init(&indices, &positions, &normals);
    }

    SkyBox::~SkyBox()
    {
        m_mesh.Terminate();
    }

    void SkyBox::Render()
    {
        m_mesh.Render();
    }
}