#include "common/bounding_box.h"

namespace glsl_shader
{
    BoundingBox::BoundingBox()
    {
        Reset();
    }

    BoundingBox::BoundingBox(const glm::vec3& point)
    {
        Reset();
        Add(point);
    }

    BoundingBox::BoundingBox(const BoundingBox& other)
        : min(other.min),
          max(other.max)
    {

    }

    BoundingBox::~BoundingBox()
    {

    }

    BoundingBox& BoundingBox::operator = (const BoundingBox& other)
    {
        min = other.min;
        max = other.max;
        return *this;
    }

    void BoundingBox::Add(const glm::vec3& point)
    {
        min.x = std::fmin(min.x, point.x);
        min.y = std::fmin(min.y, point.y);
        min.z = std::fmin(min.z, point.z);

        max.x = std::fmax(max.x, point.x);
        max.y = std::fmax(max.y, point.y);
        max.z = std::fmax(max.z, point.z);
    }

    void BoundingBox::Add(const BoundingBox& other)
    {
        min.x = std::fmin(min.x, other.min.x);
        min.y = std::fmin(min.y, other.min.y);
        min.z = std::fmin(min.z, other.min.z);

        max.x = std::fmax(max.x, other.max.x);
        max.y = std::fmax(max.y, other.max.y);
        max.z = std::fmax(max.z, other.max.z);
    }

    void BoundingBox::Reset()
    {
        min = glm::vec3(std::numeric_limits<float>::max());
        max = glm::vec3(-std::numeric_limits<float>::max());
    }

    glm::vec3 BoundingBox::GetDiagonal() const
    {
        return max - min;
    }

    std::string BoundingBox::ToString() const
    {
        std::stringstream stream;
        stream << "BoundingBox: min= (" << min.x << ", " << min.y << ", " << min.z << ") " <<
                  "max = (" << max.x << ", " << max.y << ", " << max.z << ")";
        return stream.str();
    }
}