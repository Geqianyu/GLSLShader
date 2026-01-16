#include "common/obj_mesh.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

namespace glsl_shader
{
    static void trim_string(std::string& str)
    {
        const char* white_space = " \t\n\r";
        size_t location = str.find_first_not_of(white_space);
        str.erase(0, location);
        location = str.find_last_not_of(white_space);
        str.erase(location + 1);
    }

    void ObjMesh::MeshData::Clear()
    {
        positions.clear();
        normals.clear();
        uvs.clear();
        faces.clear();
        tangents.clear();
    }

    void ObjMesh::MeshData::Center(BoundingBox& bounding_box)
    {
        if (positions.empty())
        {
            return;
        }

        glm::vec3 center = 0.5f * (bounding_box.max + bounding_box.min);

        for (int i = 0; i < positions.size(); i += 3)
        {
            positions[i] -= center.x;
            positions[i + 1] -= center.y;
            positions[i + 2] -= center.z;
        }

        bounding_box.max = bounding_box.max - center;
        bounding_box.min = bounding_box.min - center;
    }

    void ObjMesh::MeshData::ConvertFacesToAdjancencyFormat()
    {
        std::vector<GLuint> index_adj(faces.size() * 2);

        for (GLuint i = 0; i < faces.size(); i += 3)
        {
            index_adj[i * 2 + 0] = faces[i];
            index_adj[i * 2 + 1] = std::numeric_limits<GLuint>::max();
            index_adj[i * 2 + 2] = faces[i + 1];
            index_adj[i * 2 + 3] = std::numeric_limits<GLuint>::max();
            index_adj[i * 2 + 4] = faces[i + 2];
            index_adj[i * 2 + 5] = std::numeric_limits<GLuint>::max();
        }

        for (GLuint i = 0; i < index_adj.size(); i += 6)
        {
            GLuint a1 = index_adj[i];
            GLuint b1 = index_adj[i + 2];
            GLuint c1 = index_adj[i + 4];

            for (GLuint j = i + 6; j < index_adj.size(); j += 6)
            {
                GLuint a2 = index_adj[j];
                GLuint b2 = index_adj[j + 2];
                GLuint c2 = index_adj[j + 4];

                if ((a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2))
                {
                    index_adj[i + 1] = c2;
                    index_adj[j + 1] = c1;
                }

                if ((a1 == b2 && b1 == c2) || (a1 == c2 && b1 == b2))
                {
                    index_adj[i + 1] = a2;
                    index_adj[j + 3] = c1;
                }

                if ((a1 == c2 && b1 == a2) || (a1 == a2 && b1 == c2))
                {
                    index_adj[i + 1] = b2;
                    index_adj[j + 5] = c1;
                }

                if ((b1 == a2 && c1 == b2) || (b1 == b2 && c1 == a2))
                {
                    index_adj[i + 3] = c2;
                    index_adj[j + 1] = a1;
                }

                if ((b1 == b2 && c1 == c2) || (b1 == c2 && c1 == b2))
                {
                    index_adj[i + 3] = a2;
                    index_adj[j + 3] = a1;
                }

                if ((b1 == c2 && c1 == a2) || (b1 == a2 && c1 == c2))
                {
                    index_adj[i + 3] = b2;
                    index_adj[j + 5] = a1;
                }

                if ((c1 == a2 && a1 == b2) || (c1 == b2 && a1 == a2))
                {
                    index_adj[i + 5] = c2;
                    index_adj[j + 1] = b1;
                }

                if ((c1 == b2 && a1 == c2) || (c1 == c2 && a1 == b2))
                {
                    index_adj[i + 5] = a2;
                    index_adj[j + 3] = b1;
                }

                if ((c1 == c2 && a1 == a2) || (c1 == a2 && a1 == c2))
                {
                    index_adj[i + 5] = b2;
                    index_adj[j + 5] = b1;
                }
            }
        }

        for (GLuint i = 0; i < index_adj.size(); i += 6)
        {
            if (index_adj[i + 1] == std::numeric_limits<GLuint>::max()) index_adj[i + 1] = index_adj[i + 4];
            if (index_adj[i + 3] == std::numeric_limits<GLuint>::max()) index_adj[i + 3] = index_adj[i];
            if (index_adj[i + 5] == std::numeric_limits<GLuint>::max()) index_adj[i + 5] = index_adj[i + 2];
        }

        faces = index_adj;
    }

    ObjMesh::ObjMeshData::ObjVertex::ObjVertex()
        : position_index(-1),
          normal_index(-1),
          uv_index(-1)
    {

    }

    ObjMesh::ObjMeshData::ObjVertex::ObjVertex(std::string& vertex_string, ObjMesh::ObjMeshData* mesh)
        : position_index(-1),
          normal_index(-1),
          uv_index(-1)
    {
        size_t slash1 = 0;
        size_t slash2 = 0;
        slash1 = vertex_string.find("/");
        position_index = std::stoi(vertex_string.substr(0, slash1));
        if (position_index < 0)
        {
            position_index += mesh->positions.size();
        }
        else
        {
            --position_index;
        }

        if (slash1 != std::string::npos)
        {
            slash2 = vertex_string.find("/", slash1 + 1);
            if (slash2 > slash1 + 1)
            {
                uv_index = std::stoi(vertex_string.substr(slash1 + 1, slash2 - slash1 - 1));
                if (uv_index < 0)
                {
                    uv_index += mesh->uvs.size();
                }
                else
                {
                    --uv_index;
                }
            }

            normal_index = std::stoi(vertex_string.substr(slash2 + 1));
            if (normal_index < 0)
            {
                normal_index += mesh->normals.size();
            }
            else
            {
                --normal_index;
            }
        }
    }

    std::string ObjMesh::ObjMeshData::ObjVertex::ToString()
    {
        return std::to_string(position_index) + "/" + std::to_string(normal_index) + "/" + std::to_string(uv_index);
    }

    ObjMesh::ObjMeshData::ObjMeshData()
    {

    }

    void ObjMesh::ObjMeshData::GenerateNormalsIfNeeded()
    {
        if (normals.size() != 0)
        {
            return;
        }

        normals.resize(positions.size());

        for (GLuint i = 0; i < faces.size(); i += 3)
        {
            const glm::vec3& p1 = positions[faces[i].position_index];
            const glm::vec3& p2 = positions[faces[i + 1].position_index];
            const glm::vec3& p3 = positions[faces[i + 2].position_index];

            glm::vec3 a = p2 - p1;
            glm::vec3 b = p3 - p1;
            glm::vec3 n = glm::normalize(glm::cross(a, b));

            normals[faces[i].position_index] += n;
            normals[faces[i + 1].position_index] += n;
            normals[faces[i + 2].position_index] += n;

            faces[i].normal_index = faces[i].position_index;
            faces[i + 1].normal_index = faces[i + 1].position_index;
            faces[i + 2].normal_index = faces[i + 2].position_index;
        }

        for (GLuint i = 0; i < normals.size(); ++i)
        {
            normals[i] = glm::normalize(normals[i]);
        }
    }

    void ObjMesh::ObjMeshData::GenerateTangents()
    {
        std::vector<glm::vec3> tan1_accum(positions.size());
        std::vector<glm::vec3> tan2_accum(positions.size());
        tangents.resize(positions.size());

        for (GLuint i = 0; i < faces.size(); i += 3)
        {
            const glm::vec3& p1 = positions[faces[i].position_index];
            const glm::vec3& p2 = positions[faces[i + 1].position_index];
            const glm::vec3& p3 = positions[faces[i + 2].position_index];

            const glm::vec2& tc1 = uvs[faces[i].uv_index];
            const glm::vec2& tc2 = uvs[faces[i + 1].uv_index];
            const glm::vec2& tc3 = uvs[faces[i + 2].uv_index];

            glm::vec3 q1 = p2 - p1;
            glm::vec3 q2 = p3 - p1;
            float s1 = tc2.x - tc1.x, s2 = tc3.x - tc1.x;
            float t1 = tc2.y - tc1.y, t2 = tc3.y - tc1.y;
            float r = 1.0f / (s1 * t2 - s2 * t1);
            glm::vec3 tan1
            (
                (t2 * q1.x - t1 * q2.x) * r,
                (t2 * q1.y - t1 * q2.y) * r,
                (t2 * q1.z - t1 * q2.z) * r
            );
            glm::vec3 tan2
            (
                (s1 * q2.x - s2 * q1.x) * r,
                (s1 * q2.y - s2 * q1.y) * r,
                (s1 * q2.z - s2 * q1.z) * r
            );
            tan1_accum[faces[i].position_index] += tan1;
            tan1_accum[faces[i + 1].position_index] += tan1;
            tan1_accum[faces[i + 2].position_index] += tan1;
            tan2_accum[faces[i].position_index] += tan2;
            tan2_accum[faces[i + 1].position_index] += tan2;
            tan2_accum[faces[i + 2].position_index] += tan2;
        }

        for (GLuint i = 0; i < positions.size(); ++i)
        {
            const glm::vec3& n = normals[i];
            glm::vec3& t1 = tan1_accum[i];
            glm::vec3& t2 = tan2_accum[i];

            tangents[i] = glm::vec4(glm::normalize(t1 - (glm::dot(n, t1) * n)), 0.0f);
            tangents[i].w = (glm::dot(glm::cross(n, t1), t2) < 0.0f) ? -1.0f : 1.0f;
        }
    }

    void ObjMesh::ObjMeshData::Load(const char* filename, BoundingBox& bounding_box)
    {
        std::ifstream obj_file(filename, std::ios::in);

        if (!obj_file.is_open())
        {
            std::cerr << "打开 .obj 文件失败: " << filename << std::endl;
            std::exit(1);
        }

        bounding_box.Reset();
        std::string line;
        std::string token;
        std::getline(obj_file, line);
        while (!obj_file.eof())
        {
            size_t pos = line.find_first_of("#");
            if (pos != std::string::npos)
            {
                line = line.substr(0, pos);
            }
            trim_string(line);

            if (line.length() > 0)
            {
                std::istringstream line_stream(line);

                line_stream >> token;

                if (token == "v")
                {
                    float x = 0.0f;
                    float y = 0.0f;
                    float z = 0.0f;
                    line_stream >> x >> y >> z;
                    glm::vec3 p(x, y, z);
                    positions.push_back(p);
                    bounding_box.Add(p);
                }
                else if (token == "vt")
                {
                    float s = 0.0f;
                    float t = 0.0f;
                    line_stream >> s >> t;
                    uvs.push_back(glm::vec2(s, t));
                }
                else if (token == "vn")
                {
                    float x = 0.0f;
                    float y = 0.0f;
                    float z = 0.0f;
                    line_stream >> x >> y >> z;
                    normals.push_back(glm::vec3(x, y, z));
                }
                else if (token == "f")
                {
                    std::vector<std::string> parts;
                    while (line_stream.good())
                    {
                        std::string s;
                        line_stream >> s;
                        parts.push_back(s);
                    }

                    if (parts.size() > 2)
                    {
                        ObjVertex first_vertex(parts[0], this);
                        for (int i = 2; i < parts.size(); i++)
                        {
                            faces.push_back(first_vertex);
                            faces.push_back(ObjVertex(parts[i - 1], this));
                            faces.push_back(ObjVertex(parts[i], this));
                        }
                    }
                }
            }
            std::getline(obj_file, line);
        }
        obj_file.close();
    }

    void ObjMesh::ObjMeshData::ToMesh(MeshData& data)
    {
        data.Clear();

        std::unordered_map<std::string, GLuint> vertex_map;
        for (ObjVertex& vertex : faces)
        {
            std::string vertex_str = vertex.ToString();
            std::unordered_map<std::string, GLuint>::iterator it = vertex_map.find(vertex_str);
            if (it == vertex_map.end())
            {
                size_t vertex_index = data.positions.size() / 3;

                glm::vec3& position = positions[vertex.position_index];
                data.positions.push_back(position.x);
                data.positions.push_back(position.y);
                data.positions.push_back(position.z);

                glm::vec3& n = normals[vertex.normal_index];
                data.normals.push_back(n.x);
                data.normals.push_back(n.y);
                data.normals.push_back(n.z);

                if (!uvs.empty())
                {
                    glm::vec2& uv = uvs[vertex.uv_index];
                    data.uvs.push_back(uv.x);
                    data.uvs.push_back(uv.y);
                }

                if (!tangents.empty())
                {
                    glm::vec4& tangent = tangents[vertex.position_index];
                    data.tangents.push_back(tangent.x);
                    data.tangents.push_back(tangent.y);
                    data.tangents.push_back(tangent.z);
                    data.tangents.push_back(tangent.w);
                }

                data.faces.push_back(static_cast<GLuint>(vertex_index));
                vertex_map[vertex_str] = static_cast<GLuint>(vertex_index);
            }
            else
            {
                data.faces.push_back(it->second);
            }
        }
    }

    ObjMesh::ObjMesh()
        : m_is_draw_adj(false),
          m_vao(0),
          m_vertex_count(0)
    {

    }

    ObjMesh::~ObjMesh()
    {
        Terminate();
    }

    void ObjMesh::Init
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

    void ObjMesh::Terminate()
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

    void ObjMesh::Render() const
    {
        if (m_is_draw_adj) {
            glBindVertexArray(m_vao);
            glDrawElements(GL_TRIANGLES_ADJACENCY, m_vertex_count, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        else
        {
            glBindVertexArray(m_vao);
            glDrawElements(GL_TRIANGLES, m_vertex_count, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    std::unique_ptr<ObjMesh> ObjMesh::Load(const char* filename, bool center, bool gen_tangents)
    {
        std::unique_ptr<ObjMesh> mesh(new ObjMesh());

        ObjMeshData obj_mesh_data;
        obj_mesh_data.Load(filename, mesh->m_bounding_box);

        obj_mesh_data.GenerateNormalsIfNeeded();

        if (gen_tangents)
        {
            obj_mesh_data.GenerateTangents();
        }

        ObjMesh::MeshData mesh_data;
        obj_mesh_data.ToMesh(mesh_data);

        if (center)
        {
            mesh_data.Center(mesh->m_bounding_box);
        }

        mesh->Init
        (
            &(mesh_data.faces),
            &(mesh_data.positions),
            &(mesh_data.normals),
            mesh_data.uvs.empty() ? nullptr : &(mesh_data.uvs),
            mesh_data.tangents.empty() ? nullptr : &(mesh_data.tangents)
        );

        std::cout << "加载模型文件: " << filename << std::endl;
        std::cout << " vertices = " << (mesh_data.positions.size() / 3) << std::endl;
        std::cout << " triangles = " << (mesh_data.faces.size() / 3) << std::endl;
        std::cout << " bounding box = " << mesh->m_bounding_box.ToString() << std::endl;

        return mesh;
    }

    std::unique_ptr<ObjMesh> ObjMesh::LoadWithAdjacency(const char* filename, bool center)
    {
        std::unique_ptr<ObjMesh> mesh(new ObjMesh());

        ObjMeshData obj_mesh_data;
        obj_mesh_data.Load(filename, mesh->m_bounding_box);

        obj_mesh_data.GenerateNormalsIfNeeded();

        MeshData mesh_data;
        obj_mesh_data.ToMesh(mesh_data);

        if (center)
        {
            mesh_data.Center(mesh->m_bounding_box);
        }

        mesh->m_is_draw_adj = true;
        mesh_data.ConvertFacesToAdjancencyFormat();

        mesh->Init
        (
            &(mesh_data.faces),
            &(mesh_data.positions),
            &(mesh_data.normals),
            mesh_data.uvs.empty() ? nullptr : &(mesh_data.uvs),
            mesh_data.tangents.empty() ? nullptr : &(mesh_data.tangents)
        );

        std::cout << "加载模型文件: " << filename << std::endl;
        std::cout << " vertices = " << (mesh_data.positions.size() / 3) << std::endl;
        std::cout << " triangles = " << (mesh_data.faces.size() / 3) << std::endl;

        return mesh;
    }
}