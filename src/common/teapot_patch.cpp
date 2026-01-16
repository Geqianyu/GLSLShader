#include "common/teapot_patch.h"
#include "common/teapot_data.h"

#include "glm/gtc/matrix_transform.hpp"

namespace glsl_shader
{
    TeapotPatch::TeapotPatch()
        : m_vbo(0),
          m_vao(0)
    {
        std::vector<GLfloat> points(32 * 16 * 3);
        GeneratePatch(points);
        Init(points);
    }

    TeapotPatch::~TeapotPatch()
    {
        if (m_vao > 0)
        {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }

        if (m_vbo > 0)
        {
            glDeleteBuffers(1, &m_vbo);
            m_vbo = 0;
        }
    }

    void TeapotPatch::Render()
    {
        glPatchParameteri(GL_PATCH_VERTICES, 16);

        glBindVertexArray(m_vao);
        glDrawArrays(GL_PATCHES, 0, 512);
        glBindVertexArray(0);
    }

    void TeapotPatch::GeneratePatch(std::vector<GLfloat>& points)
    {
        int index = 0;

        // The rim
        BuildPatchReflect(0, points, index, true, true);
        // The body
        BuildPatchReflect(1, points, index, true, true);
        BuildPatchReflect(2, points, index, true, true);
        // The lid
        BuildPatchReflect(3, points, index, true, true);
        BuildPatchReflect(4, points, index, true, true);
        // The bottom
        BuildPatchReflect(5, points, index, true, true);
        // The handle
        BuildPatchReflect(6, points, index, false, true);
        BuildPatchReflect(7, points, index, false, true);
        // The spout
        BuildPatchReflect(8, points, index, false, true);
        BuildPatchReflect(9, points, index, false, true);
    }

    void TeapotPatch::BuildPatchReflect(int patch_count, std::vector<GLfloat>& points, int& index, bool reflect_x, bool reflect_y)
    {
        glm::vec3 patch[4][4];
        glm::vec3 patch_reverse_v[4][4];
        GetPatch(patch_count, patch, false);
        GetPatch(patch_count, patch_reverse_v, true);

        // Patch without modification
        BuildPatch(patch_reverse_v, points, index, glm::mat3(1.0f));

        // Patch reflected in x
        if (reflect_x)
        {
            BuildPatch
            (
                patch,
                points,
                index,
                glm::mat3
                (
                    glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                )
            );
        }

        // Patch reflected in y
        if (reflect_y) {
            BuildPatch
            (
                patch,
                points,
                index,
                glm::mat3
                (
                    glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                )
            );
        }

        // Patch reflected in x and y
        if (reflect_x && reflect_y)
        {
            BuildPatch
            (
                patch_reverse_v,
                points,
                index,
                glm::mat3
                (
                    glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                )
            );
        }
    }

    void TeapotPatch::BuildPatch(glm::vec3& patch[][4], std::vector<GLfloat>& points, int& index, const glm::vec3& reflect)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; j++)
            {
                glm::vec3 point = reflect * patch[i][j];

                points[index] = point.x;
                points[index + 1] = point.y;
                points[index + 2] = point.z;

                index += 3;
            }
        }
    }

    void TeapotPatch::GetPatch(int patch_count, glm::vec3 patch[][4], bool reverse_v)
    {
        for (int u = 0; u < 4; ++u)
        {
            for (int v = 0; v < 4; ++v)
            {
                if (reverse_v)
                {
                    patch[u][v] = glm::vec3
                    (
                        TeapotData::cpdata[TeapotData::patch_data[patch_count][u * 4 + (3 - v)]][0],
                        TeapotData::cpdata[TeapotData::patch_data[patch_count][u * 4 + (3 - v)]][1],
                        TeapotData::cpdata[TeapotData::patch_data[patch_count][u * 4 + (3 - v)]][2]
                    );
                }
                else {
                    patch[u][v] = glm::vec3
                    (
                        TeapotData::cpdata[TeapotData::patch_data[patch_count][u * 4 + v]][0],
                        TeapotData::cpdata[TeapotData::patch_data[patch_count][u * 4 + v]][1],
                        TeapotData::cpdata[TeapotData::patch_data[patch_count][u * 4 + v]][2]
                    );
                }
            }
        }
    }

    void TeapotPatch::Init(std::vector<GLfloat>& points)
    {
        if (m_vao > 0)
        {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }

        if (m_vbo > 0)
        {
            glDeleteBuffers(1, &m_vbo);
            m_vbo = 0;
        }

        glGenBuffers(1, &m_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
}