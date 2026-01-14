#include "common/torus.h"

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

namespace glsl_shader
{
    Torus::Torus(GLfloat outer_radius, GLfloat inner_radius, GLuint sides_count, GLuint rings_count)
    {
        GLuint faces = sides_count * rings_count;
        int vertex_count = sides_count * (rings_count + 1);

        std::vector<GLfloat> positions(3 * vertex_count);
        std::vector<GLfloat> normals(3 * vertex_count);
        std::vector<GLfloat> uvs(2 * vertex_count);
        std::vector<GLuint> indices(6 * faces);

        float ring_factor = glm::two_pi<float>() / rings_count;
        float side_factor = glm::two_pi<float>() / sides_count;
        int index = 0;
        int uv_index = 0;
        for (GLuint ring = 0; ring <= rings_count; ++ring)
        {
            float u = ring * ring_factor;
            float cu = glm::cos(u);
            float su = glm::sin(u);
            for (GLuint side = 0; side < sides_count; side++)
            {
                float v = side * side_factor;
                float cv = glm::cos(v);
                float sv = glm::sin(v);
                float r = (outer_radius + inner_radius * cv);
                positions[index] = r * cu;
                positions[index + 1] = r * su;
                positions[index + 2] = inner_radius * sv;
                normals[index] = cv * cu * r;
                normals[index + 1] = cv * su * r;
                normals[index + 2] = sv * r;
                uvs[uv_index] = u / glm::two_pi<float>();
                uvs[uv_index + 1] = v / glm::two_pi<float>();
                uv_index += 2;

                float len = glm::sqrt(normals[index] * normals[index] + normals[index + 1] * normals[index + 1] + normals[index + 2] * normals[index + 2]);
                normals[index] /= len;
                normals[index + 1] /= len;
                normals[index + 2] /= len;
                index += 3;
            }
        }

        index = 0;
        for (GLuint ring = 0; ring < rings_count; ring++)
        {
            GLuint ring_start = ring * sides_count;
            GLuint next_ring_start = (ring + 1) * sides_count;
            for (GLuint side = 0; side < sides_count; ++side)
            {
                int next_side = (side + 1) % sides_count;

                indices[index] = ring_start + side;
                indices[index + 1] = next_ring_start + side;
                indices[index + 2] = next_ring_start + next_side;
                indices[index + 3] = ring_start + side;
                indices[index + 4] = next_ring_start + next_side;
                indices[index + 5] = ring_start + next_side;
                index += 6;
            }
        }

        m_mesh.Init(&indices, &positions, &normals, &uvs);
    }

    Torus::~Torus()
    {
        m_mesh.Terminate();
    }

    void Torus::Render()
    {
        m_mesh.Render();
    }
}