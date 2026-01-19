#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;

layout (location = 0) out vec4 fragment_color;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 L;
} u_lights[3];

uniform struct MaterialInfo
{
    float roughness;
    bool is_metal;
    vec3 color;
} u_material;

const float PI = 3.14159265358979323846;

float CalculateGXXDistriubtion(float n_dot_h)
{
    float alpha2 = u_material.roughness * u_material.roughness * u_material.roughness * u_material.roughness;
    float d = (n_dot_h * n_dot_h) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

float CalculateGeometrySmith(float dot_product)
{
    float k = (u_material.roughness + 1.0) * (u_material.roughness + 1.0) / 8.0;
    float denom = dot_product * (1.0 - k) + k;
    return 1.0 / denom;
}

vec3 CalculateSchlickFresnel(float l_dot_h)
{
    vec3 f0 = vec3(0.04);
    if (u_material.is_metal)
    {
        f0 = u_material.color;
    }
    return f0 + (1 - f0) * pow(1.0 - l_dot_h, 5);
}

vec3 CalculateMicrofacetModel(int light_index, vec3 position, vec3 normal)
{
    vec3 diffuse_brdf = vec3(0.0);
    if (!u_material.is_metal)
    {
        diffuse_brdf = u_material.color;
    }

    vec3 l = vec3(0.0);
    vec3 light_I = u_lights[light_index].L;
    if (u_lights[light_index].position_in_view.w == 0)
    {
        // 方向光
        l = normalize(u_lights[light_index].position_in_view.xyz);
    }
    else
    {
        l = u_lights[light_index].position_in_view.xyz - position;
        float dist = length(l);
        l = normalize(l);
        light_I /= (dist * dist);
    }

    vec3 v = normalize(-position);
    vec3 h = normalize(v + l);
    float n_dot_h = dot(normal, h);
    float l_dot_h = dot(l, h);
    float n_dot_l = max(dot(normal, l), 0.0);
    float n_dot_v = dot(normal, v);
    vec3 specular_brdf = 0.25 * CalculateGXXDistriubtion(n_dot_h) * CalculateSchlickFresnel(l_dot_h) * CalculateGeometrySmith(n_dot_l) * CalculateGeometrySmith(n_dot_v);

    return (diffuse_brdf + PI * specular_brdf) * light_I * n_dot_h;
}

void main()
{
    vec3 sum = vec3(0.0);
    vec3 normal = normalize(normal_in_view);
    for(int i = 0; i < 3; ++i)
    {
        sum += CalculateMicrofacetModel(i, position_in_view, normal);
    }

    // Gamma 
    sum = pow(sum, vec3(1.0/2.2));

    fragment_color = vec4(sum, 1.0);
}