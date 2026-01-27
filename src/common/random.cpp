#include "common/random.h"

namespace glsl_shader
{
    Random::Random()
        : m_distribution(0.0f, 1.0f)
    {
        std::random_device rd;
        m_generator.seed(rd());
    }

    Random::~Random()
    {

    }

    float Random::GetNext()
    {
        return m_distribution(m_generator);
    }

    glm::vec3 Random::UniformHemisphere()
    {
        glm::vec3 result;
        float x1 = GetNext();
        float x2 = GetNext();
        float s = glm::sqrt(1.0f - x1 * x1);
        result.x = glm::cos(glm::two_pi<float>() * x2) * s;
        result.y = glm::sin(glm::two_pi<float>() * x2) * s;
        result.z = x1;
        return result;
    }

    glm::vec3 Random::UniformCircle()
    {
        glm::vec3 result(0.0f);
        float x = GetNext();
        result.x = glm::cos(glm::two_pi<float>() * x);
        result.y = glm::sin(glm::two_pi<float>() * x);
        return result;
    }

    void Random::Shuffle(std::vector<float>& v)
    {
        std::default_random_engine rng = std::default_random_engine{ };
        std::shuffle(v.begin(), v.end(), rng);
    }
}