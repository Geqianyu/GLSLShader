#include "common/plane.h"

namespace glsl_shader
{
    Plane::Plane(float xsize, float zsize, int xdivs, int zdivs, float smax, float tmax)
    {
        int position_count = (xdivs + 1) * (zdivs + 1);
        std::vector<GLfloat> positions(3 * position_count);
        std::vector<GLfloat> normals(3 * position_count);
        std::vector<GLfloat> uvs(2 * position_count);
        std::vector<GLfloat> tangents(4 * position_count);
        std::vector<GLuint> indices(6 * xdivs * zdivs);

        float x2 = xsize / 2.0f;
        float z2 = zsize / 2.0f;
        float i_factor = static_cast<float>(zsize) / zdivs;
        float j_factor = static_cast<float>(xsize) / xdivs;
        float uv_i = smax / xdivs;
        float uv_j = tmax / zdivs;
        float x = 0.0f;
        float z = 0.0f;
        int vidx = 0;
        int tidx = 0;
        for (int i = 0; i <= zdivs; ++i)
        {
            z = i_factor * i - z2;
            for (int j = 0; j <= xdivs; ++j)
            {
                x = j_factor * j - x2;
                positions[vidx] = x;
                positions[vidx + 1] = 0.0f;
                positions[vidx + 2] = z;
                normals[vidx] = 0.0f;
                normals[vidx + 1] = 1.0f;
                normals[vidx + 2] = 0.0f;

                uvs[tidx] = j * uv_i;
                uvs[tidx + 1] = (zdivs - i) * uv_j;

                vidx += 3;
                tidx += 2;
            }
        }

        for (int i = 0; i < position_count; ++i)
        {
            tangents[i * 4 + 0] = 1.0f;
            tangents[i * 4 + 1] = 0.0f;
            tangents[i * 4 + 2] = 0.0f;
            tangents[i * 4 + 3] = 1.0f;
        }

        GLuint row_start = 0;
        GLuint next_row_start = 0;
        int idx = 0;
        for (int i = 0; i < zdivs; ++i)
        {
            row_start = static_cast<GLuint>(i * (xdivs + 1));
            next_row_start = static_cast<GLuint>((i + 1) * (xdivs + 1));
            for (int j = 0; j < xdivs; ++j)
            {
                indices[idx] = row_start + j;
                indices[idx + 1] = next_row_start + j;
                indices[idx + 2] = next_row_start + j + 1;
                indices[idx + 3] = row_start + j;
                indices[idx + 4] = next_row_start + j + 1;
                indices[idx + 5] = row_start + j + 1;
                idx += 6;
            }
        }

        m_mesh.Init(&indices, &positions, &normals, &uvs, &tangents);
    }

    Plane::~Plane()
    {
        m_mesh.Terminate();
    }

    void Plane::Render()
    {
        m_mesh.Render();
    }
}