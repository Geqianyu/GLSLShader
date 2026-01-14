#ifndef __GLSL_SHADER_COMMON_TRIANGLE_MESH_H__
#define __GLSL_SHADER_COMMON_TRIANGLE_MESH_H__

#include "glad/gl.h"

#include <vector>

namespace glsl_shader
{
    class TriangleMesh
    {
    public:
        TriangleMesh();
        ~TriangleMesh();

        void Init
        (
            std::vector<GLuint>* indices,
            std::vector<GLfloat>* positions,
            std::vector<GLfloat>* normals,
            std::vector<GLfloat>* uvs = nullptr,
            std::vector<GLfloat>* tangents = nullptr
        );

        void Terminate();

        void Render();

        GLuint GetVAO() const;
        GLuint GetIndexBufferObject();
        GLuint GetPositionBufferObject();
        GLuint GetNormalBufferObject();
        GLuint GetUvBufferObject();
        GLuint GetVertexCount();

    private:
        GLuint m_vao;
        GLuint m_vertex_count;
        std::vector<GLuint> m_buffers;
    };
}

#endif // !__GLSL_SHADER_COMMON_TRIANGLE_MESH_H__