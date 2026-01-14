#include "common/triangle_mesh.h"

namespace glsl_shader
{
    TriangleMesh::TriangleMesh()
        : m_vao(0),
          m_vertex_count(0)
    {

    }

    TriangleMesh::~TriangleMesh()
    {
        Terminate();
    }

    void TriangleMesh::Init
    (
        std::vector<GLuint>* indices,
        std::vector<GLfloat>* positions,
        std::vector<GLfloat>* normals,
        std::vector<GLfloat>* uvs,
        std::vector<GLfloat>* tangents
    )
    {
        if (indices == nullptr || positions == nullptr || normals == nullptr)
        {
            return;
        }

        Terminate();

        m_vertex_count = static_cast<GLuint>(indices->size());

        GLuint index_buffer_object = 0;
        GLuint position_buffer_object = 0;
        GLuint normal_buffer_object = 0;
        GLuint uv_buffer_object = 0;
        GLuint tangent_buffer_object = 0;
        glGenBuffers(1, &index_buffer_object);
        m_buffers.push_back(index_buffer_object);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(GLuint), indices->data(), GL_STATIC_DRAW);

        glGenBuffers(1, &position_buffer_object);
        m_buffers.push_back(position_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, position_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(GLfloat), positions->data(), GL_STATIC_DRAW);

        glGenBuffers(1, &normal_buffer_object);
        m_buffers.push_back(normal_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, normals->size() * sizeof(GLfloat), normals->data(), GL_STATIC_DRAW);

        if (uvs != nullptr)
        {
            glGenBuffers(1, &uv_buffer_object);
            m_buffers.push_back(uv_buffer_object);
            glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_object);
            glBufferData(GL_ARRAY_BUFFER, uvs->size() * sizeof(GLfloat), uvs->data(), GL_STATIC_DRAW);
        }

        if (tangents != nullptr)
        {
            glGenBuffers(1, &tangent_buffer_object);
            m_buffers.push_back(tangent_buffer_object);
            glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_object);
            glBufferData(GL_ARRAY_BUFFER, tangents->size() * sizeof(GLfloat), tangents->data(), GL_STATIC_DRAW);
        }

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);

        glBindBuffer(GL_ARRAY_BUFFER, position_buffer_object);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        if (uvs != nullptr)
        {
            glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_object);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(2);
        }

        if (tangents != nullptr)
        {
            glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_object);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(3);
        }

        glBindVertexArray(0);
    }

    void TriangleMesh::Terminate()
    {
        if (!m_buffers.empty())
        {
            glDeleteBuffers(static_cast<GLsizei>(m_buffers.size()), m_buffers.data());
            m_buffers.clear();
        }

        if (m_vao != 0)
        {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }
    }

    void TriangleMesh::Render()
    {
        if (m_vao == 0)
        {
            return;
        }

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_vertex_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    GLuint TriangleMesh::GetVAO() const
    {
        return m_vao;
    }

    GLuint TriangleMesh::GetIndexBufferObject()
    {
        return m_buffers[0];
    }

    GLuint TriangleMesh::GetPositionBufferObject()
    {
        return m_buffers[1];
    }

    GLuint TriangleMesh::GetNormalBufferObject()
    {
        return m_buffers[2];
    }

    GLuint TriangleMesh::GetUvBufferObject()
    {
        if (m_buffers.size() >= 3)
        {
            return m_buffers[3];
        }
        return 0;
    }

    GLuint TriangleMesh::GetVertexCount()
    {
        return m_vertex_count;
    }
}