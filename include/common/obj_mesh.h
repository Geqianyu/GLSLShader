#ifndef __GLSL_SHADER_COMMON_OBJ_MESH_H__
#define __GLSL_SHADER_COMMON_OBJ_MESH_H__

#include "common/triangle_mesh.h"
#include "common/bounding_box.h"

#include "glm/glm.hpp"

#include <vector>
#include <string>
#include <memory>

namespace glsl_shader
{
    class ObjMesh
    {
    public:
        ObjMesh();
        ~ObjMesh();

        void Init
        (
            std::vector<GLuint>* indices,
            std::vector<GLfloat>* positions,
            std::vector<GLfloat>* normals,
            std::vector<GLfloat>* uvs = nullptr,
            std::vector<GLfloat>* tangents = nullptr
        );
        void Terminate();
        void Render() const;

    public:
        static std::unique_ptr<ObjMesh> Load(const char* filename, bool center = false, bool gen_tangents = false);
        static std::unique_ptr<ObjMesh> LoadWithAdjacency(const char* filename, bool center = false);

    private:
        struct MeshData
        {
            std::vector <GLfloat> positions;
            std::vector <GLfloat> normals;
            std::vector <GLfloat> uvs;
            std::vector <GLuint> faces;
            std::vector <GLfloat> tangents;

            void Clear();
            void Center(BoundingBox& bounding_box);
            void ConvertFacesToAdjancencyFormat();
        };

        struct ObjMeshData
        {
            struct ObjVertex
            {
                int position_index;
                int normal_index;
                int uv_index;

                ObjVertex();
                ObjVertex(std::string& vertex_string, ObjMeshData* mesh);
                std::string ToString();
            };

            std::vector <glm::vec3> positions;
            std::vector <glm::vec3> normals;
            std::vector <glm::vec2> uvs;
            std::vector <ObjVertex> faces;
            std::vector <glm::vec4> tangents;

            ObjMeshData();

            void GenerateNormalsIfNeeded();
            void GenerateTangents();
            void Load(const char* filename, BoundingBox& bounding_box);
            void ToMesh(MeshData& data);
        };

    private:
        bool m_is_draw_adj;
        BoundingBox m_bounding_box;
        GLuint m_vao;
        GLuint m_vertex_count;
        std::vector<GLuint> m_buffers;
    };
}

#endif // !__GLSL_SHADER_COMMON_OBJ_MESH_H__
