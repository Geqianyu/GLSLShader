#ifndef __GLSL_SHADER_COMMON_RANDOM_H__
#define __GLSL_SHADER_COMMON_RANDOM_H__

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include <random>
#include <algorithm>

namespace glsl_shader
{
    class Random
    {
    public:
        Random();
        ~Random();

        float GetNext();

        glm::vec3 UniformHemisphere();
        glm::vec3 UniformCircle();

    public:
        static void Shuffle(std::vector<float>& v);

    private:
        std::mt19937 m_generator;
        std::uniform_real_distribution<float> m_distribution;
    };
}

#endif // !__GLSL_SHADER_COMMON_RANDOM_H__
