#include "common/teapot.h"
#include "common/teapot_data.h"

#include "glm/gtc/matrix_transform.hpp"

#include <cstdio>

namespace glsl_shader
{
    Teapot::Teapot(int grid, const glm::mat4& transform)
    {
        int vertex_count = 32 * (grid + 1) * (grid + 1);
        int face_count = grid * grid * 32;
        std::vector<GLfloat> positions(vertex_count * 3);
        std::vector<GLfloat> normals(vertex_count * 3);
        std::vector<GLfloat> uvs(vertex_count * 2);
        std::vector<GLuint> indices(face_count * 6);

        GeneratePatches(positions, normals, uvs, indices, grid);
        MoveLid(grid, positions, transform);
        m_mesh.Init(&indices, &positions, &normals, &uvs);
    }

    Teapot::~Teapot()
    {

    }

    void Teapot::Render()
    {
        m_mesh.Render();
    }

    void Teapot::GeneratePatches
    (
        std::vector<GLfloat>& positions,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& uvs,
        std::vector<GLuint>& indices,
        int grid
    )
    {
        std::vector<GLfloat> buffer(4 * (grid + 1));
        std::vector<GLfloat> derivative_buffer(4 * (grid + 1));

        int index = 0;
        int index_index = 0;
        int uv_index = 0;

        ComputeBasisFunctions(buffer, derivative_buffer, grid);

        // Build each patch
        // The rim
        BuildPatchReflect(0, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, true, true);
        // The body
        BuildPatchReflect(1, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, true, true);
        BuildPatchReflect(2, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, true, true);
        // The lid
        BuildPatchReflect(3, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, true, true);
        BuildPatchReflect(4, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, true, true);
        // The bottom
        BuildPatchReflect(5, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, true, true);
        // The handle
        BuildPatchReflect(6, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, false, true);
        BuildPatchReflect(7, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, false, true);
        // The spout
        BuildPatchReflect(8, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, false, true);
        BuildPatchReflect(9, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, false, true);
    }

    void Teapot::BuildPatchReflect
    (
        int patch_num,
        std::vector<GLfloat>& buffer,
        std::vector<GLfloat>& derivative_buffer,
        std::vector<GLfloat>& positions,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& uvs,
        std::vector<GLuint>& indices,
        int& index,
        int& index_index,
        int& uv_index,
        int grid,
        bool reflect_x,
        bool reflect_y
    )
    {
        glm::vec3 patch[4][4];
        glm::vec3 patch_reverse_v[4][4];
        GetPatch(patch_num, patch, false);
        GetPatch(patch_num, patch_reverse_v, true);

        // Patch without modification
        BuildPatch(patch, buffer, derivative_buffer, positions, normals, uvs, indices, index, index_index, uv_index, grid, glm::mat3(1.0f), true);

        // Patch reflected in x
        if (reflect_x)
        {
            BuildPatch
            (
                patch_reverse_v,
                buffer,
                derivative_buffer,
                positions,
                normals,
                uvs,
                indices,
                index,
                index_index,
                uv_index,
                grid,
                glm::mat3
                (
                    glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                ),
                false
            );
        }

        // Patch reflected in y
        if (reflect_y) {
            BuildPatch
            (
                patch_reverse_v,
                buffer,
                derivative_buffer,
                positions,
                normals,
                uvs,
                indices,
                index,
                index_index,
                uv_index,
                grid,
                glm::mat3
                (
                    glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                ),
                false
            );
        }

        // Patch reflected in x and y
        if (reflect_x && reflect_y)
        {
            BuildPatch
            (
                patch,
                buffer,
                derivative_buffer,
                positions,
                normals,
                uvs,
                indices,
                index,
                index_index,
                uv_index,
                grid,
                glm::mat3
                (
                    glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                ),
                true
            );
        }
    }

    void Teapot::BuildPatch
    (
        glm::vec3 patch[][4],
        std::vector<GLfloat>& buffer,
        std::vector<GLfloat>& derivative_buffer,
        std::vector<GLfloat>& positions,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& uvs,
        std::vector<GLuint>& indices,
        int& index,
        int& index_index,
        int& uv_index,
        int grid,
        const glm::mat3& reflect,
        bool invert_normal
    )
    {
        int start_index = index / 3;
        float uv_factor = 1.0f / grid;

        for (int i = 0; i <= grid; ++i)
        {
            for (int j = 0; j <= grid; ++j)
            {
                glm::vec3 position = reflect * Evaluate(i, j, buffer, patch);
                glm::vec3 normal = reflect * EvaluateNormal(i, j, buffer, derivative_buffer, patch);
                if (invert_normal)
                {
                    normal = -normal;
                }

                positions[index] = position.x;
                positions[index + 1] = position.y;
                positions[index + 2] = position.z;

                normals[index] = normal.x;
                normals[index + 1] = normal.y;
                normals[index + 2] = normal.z;

                uvs[uv_index] = i * uv_factor;
                uvs[uv_index + 1] = j * uv_factor;

                index += 3;
                uv_index += 2;
            }
        }

        for (int i = 0; i < grid; ++i)
        {
            int i_start = i * (grid + 1) + start_index;
            int next_i_start = (i + 1) * (grid + 1) + start_index;
            for (int j = 0; j < grid; ++j)
            {
                indices[index_index] = i_start + j;
                indices[index_index + 1] = next_i_start + j + 1;
                indices[index_index + 2] = next_i_start + j;

                indices[index_index + 3] = i_start + j;
                indices[index_index + 4] = i_start + j + 1;
                indices[index_index + 5] = next_i_start + j + 1;

                index_index += 6;
            }
        }
    }

    void Teapot::GetPatch(int patch_num, glm::vec3 patch[][4], bool reverse_v)
    {
        for (int u = 0; u < 4; ++u)
        {
            for (int v = 0; v < 4; ++v)
            {
                if (reverse_v)
                {
                    patch[u][v] = glm::vec3
                    (
                        TeapotData::cpdata[TeapotData::patch_data[patch_num][u * 4 + (3 - v)]][0],
                        TeapotData::cpdata[TeapotData::patch_data[patch_num][u * 4 + (3 - v)]][1],
                        TeapotData::cpdata[TeapotData::patch_data[patch_num][u * 4 + (3 - v)]][2]
                    );
                }
                else
                {
                    patch[u][v] = glm::vec3
                    (
                        TeapotData::cpdata[TeapotData::patch_data[patch_num][u * 4 + v]][0],
                        TeapotData::cpdata[TeapotData::patch_data[patch_num][u * 4 + v]][1],
                        TeapotData::cpdata[TeapotData::patch_data[patch_num][u * 4 + v]][2]
                    );
                }
            }
        }
    }

    void Teapot::ComputeBasisFunctions(std::vector<GLfloat>& buffer, std::vector<GLfloat>& derivative_buffer, int grid)
    {
        float inc = 1.0f / grid;
        for (int i = 0; i <= grid; ++i)
        {
            float t = i * inc;
            float t_sqr = t * t;
            float one_minus_t = (1.0f - t);
            float one_minus_t2 = one_minus_t * one_minus_t;

            buffer[i * 4 + 0] = one_minus_t * one_minus_t2;
            buffer[i * 4 + 1] = 3.0f * one_minus_t2 * t;
            buffer[i * 4 + 2] = 3.0f * one_minus_t * t_sqr;
            buffer[i * 4 + 3] = t * t_sqr;

            derivative_buffer[i * 4 + 0] = -3.0f * one_minus_t2;
            derivative_buffer[i * 4 + 1] = -6.0f * t * one_minus_t + 3.0f * one_minus_t2;
            derivative_buffer[i * 4 + 2] = -3.0f * t_sqr + 6.0f * t * one_minus_t;
            derivative_buffer[i * 4 + 3] = 3.0f * t_sqr;
        }
    }

    glm::vec3 Teapot::Evaluate(int grid_u, int grid_v, std::vector<GLfloat>& buffer, glm::vec3 patch[][4])
    {
        glm::vec3 position(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                position += patch[i][j] * buffer[grid_u * 4 + i] * buffer[grid_v * 4 + j];
            }
        }
        return position;
    }

    glm::vec3 Teapot::EvaluateNormal(int grid_u, int grid_v, std::vector<GLfloat>& buffer, std::vector<GLfloat>& derivative_buffer, glm::vec3 patch[][4])
    {
        glm::vec3 du(0.0f, 0.0f, 0.0f);
        glm::vec3 dv(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                du += patch[i][j] * derivative_buffer[grid_u * 4 + i] * buffer[grid_v * 4 + j];
                dv += patch[i][j] * buffer[grid_u * 4 + i] * derivative_buffer[grid_v * 4 + j];
            }
        }

        glm::vec3 normal = glm::cross(du, dv);
        if (glm::length(normal) != 0.0f)
        {
            normal = glm::normalize(normal);
        }

        return normal;
    }

    void Teapot::MoveLid(int grid, std::vector<GLfloat>& positions, const glm::mat4& transform)
    {
        int start = 3 * 12 * (grid + 1) * (grid + 1);
        int end = 3 * 20 * (grid + 1) * (grid + 1);

        for (int i = start; i < end; i += 3)
        {
            glm::vec4 vertex = glm::vec4(positions[i], positions[i + 1], positions[i + 2], 1.0f);
            vertex = transform * vertex;
            positions[i] = vertex.x;
            positions[i + 1] = vertex.y;
            positions[i + 2] = vertex.z;
        }
    }
}