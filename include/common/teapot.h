#ifndef __GLSL_SHADER_COMMON_TEAPOR_H__
#define __GLSL_SHADER_COMMON_TEAPOR_H__

#include "common/triangle_mesh.h"

#include "glm/glm.hpp"

namespace glsl_shader
{
    class Teapot
    {
    public:
        Teapot(int grid, const glm::mat4& transform);
        ~Teapot();

        void Render();

    private:
        void GeneratePatches
        (
            std::vector<GLfloat>& positions,
            std::vector<GLfloat>& normals,
            std::vector<GLfloat>& uvs,
            std::vector<GLuint>& indices,
            int grid
        );
        void BuildPatchReflect
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
        );
        void BuildPatch
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
        );
        void GetPatch(int patch_num, glm::vec3 patch[][4], bool reverse_v);
        void ComputeBasisFunctions(std::vector<GLfloat>& buffer, std::vector<GLfloat>& derivative_buffer, int grid);
        glm::vec3 Evaluate(int grid_u, int grid_v, std::vector<GLfloat>& buffer, glm::vec3 patch[][4]);
        glm::vec3 EvaluateNormal(int grid_u, int grid_v, std::vector<GLfloat>& buffer, std::vector<GLfloat>& derivative_buffer, glm::vec3 patch[][4]);
        void MoveLid(int grid, std::vector<GLfloat>& positions, const glm::mat4& transform);

    private:
        TriangleMesh m_mesh;
    };
}

#endif // !__GLSL_SHADER_COMMON_TEAPOR_H__
