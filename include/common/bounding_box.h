#ifndef __GLSL_SHADER_COMMON_BOUNDING_BOX_H__
#define __GLSL_SHADER_COMMON_BOUNDING_BOX_H__

#include "glm/glm.hpp"

#include <string>
#include <sstream>
#include <limits>

namespace glsl_shader
{
    struct BoundingBox
    {
        glm::vec3 min;
        glm::vec3 max;

        BoundingBox();
        BoundingBox(const glm::vec3& point);
        BoundingBox(const BoundingBox& other);
        ~BoundingBox();

        BoundingBox& operator = (const BoundingBox& other);

        void Add(const glm::vec3& point);
        void Add(const BoundingBox& other);

        void Reset();

        glm::vec3 GetDiagonal() const;

        std::string ToString() const;
    };
}

#endif // !__GLSL_SHADER_COMMON_BOUNDING_BOX_H__
