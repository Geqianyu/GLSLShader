#include "common/sphere.h"

#include "glm/gtc/constants.hpp"

#include <cstdio>
#include <cmath>

namespace glsl_shader
{
    Sphere::Sphere(float radius, GLuint slices_count, GLuint stacks_count)
    {
        int vertex_count = (slices_count + 1) * (stacks_count + 1);
        int index_count = (slices_count * 2 * (stacks_count - 1)) * 3;

        std::vector<GLfloat> positions(3 * vertex_count);
        std::vector<GLfloat> normals(3 * vertex_count);
        std::vector<GLfloat> uvs(2 * vertex_count);
        std::vector<GLuint> indices(index_count);

        GLfloat theta = 0.0f;
        GLfloat phi = 0.0f;
        GLfloat theta_factory = glm::two_pi<float>() / slices_count;
        GLfloat phi_factory = glm::pi<float>() / stacks_count;
        GLfloat nx = 0.0f;
        GLfloat ny = 0.0f;
        GLfloat nz = 0.0f;
        GLfloat s = 0.0f;
        GLfloat t = 0.0f;
        GLuint index = 0;
        GLuint uv_index = 0;
        for (GLuint i = 0; i <= slices_count; ++i)
        {
            theta = i * theta_factory;
            s = static_cast<GLfloat>(i) / slices_count;
            for (GLuint j = 0; j <= stacks_count; ++j)
            {
                phi = j * phi_factory;
                t = static_cast<GLfloat>(j) / stacks_count;
                nx = std::sinf(phi) * std::cosf(theta);
                ny = std::sinf(phi) * std::sinf(theta);
                nz = std::cosf(phi);
                positions[index] = radius * nx;
                positions[index + 1] = radius * ny;
                positions[index + 2] = radius * nz;
                normals[index] = nx;
                normals[index + 1] = ny;
                normals[index + 2] = nz;
                index += 3;

                uvs[uv_index] = s;
                uvs[uv_index + 1] = t;
                uv_index += 2;
            }
        }

        index = 0;
        for (GLuint i = 0; i < slices_count; ++i)
        {
            GLuint stack_start = i * (stacks_count + 1);
            GLuint next_stack_start = (i + 1) * (stacks_count + 1);
            for (GLuint j = 0; j < stacks_count; ++j)
            {
                if (j == 0)
                {
                    indices[index] = stack_start;
                    indices[index + 1] = stack_start + 1;
                    indices[index + 2] = next_stack_start + 1;
                    index += 3;
                }
                else if (j == stacks_count - 1)
                {
                    indices[index] = stack_start + j;
                    indices[index + 1] = stack_start + j + 1;
                    indices[index + 2] = next_stack_start + j;
                    index += 3;
                }
                else
                {
                    indices[index] = stack_start + j;
                    indices[index + 1] = stack_start + j + 1;
                    indices[index + 2] = next_stack_start + j + 1;
                    indices[index + 3] = next_stack_start + j;
                    indices[index + 4] = stack_start + j;
                    indices[index + 5] = next_stack_start + j + 1;
                    index += 6;
                }
            }
        }

        m_mesh.Init(&indices, &positions, &normals, &uvs);
    }

    Sphere::~Sphere()
    {
        m_mesh.Terminate();
    }

    void Sphere::Render()
    {
        m_mesh.Render();
    }
}