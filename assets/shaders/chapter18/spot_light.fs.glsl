#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;

layout (location = 0) out vec4 fragment_color;

uniform struct LightInfo
{
    vec3 position_in_view;
    vec3 La;
    vec3 L;
    vec3 direction;
    float exponent;
    float cut_off;
} u_light;

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
} u_material;

vec3 CalculateBlinnPhongModel(vec3 position, vec3 normal)
{
    vec3 ambient_color = u_light.La * u_material.Ka;

    vec3 diffuse_color = vec3(0.0);
    vec3 specular_color = vec3(0.0);
    vec3 s = normalize(u_light.position_in_view - position);
    float cos_angle = dot(-s, normalize(u_light.direction));
    float angle = acos(cos_angle);
    float spot_scale = 0.0;
    if(angle >= 0.0 && angle < u_light.cut_off)
    {
        spot_scale = pow(cos_angle, u_light.exponent);
        float s_dot_n = max(dot(s, normal), 0.0);
        diffuse_color = u_material.Kd * s_dot_n;
        if(s_dot_n > 0.0)
        {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v + s);
            specular_color = u_material.Ks * pow(max(dot(h, normal), 0.0), u_material.shininess);
        }
    }

    return ambient_color + spot_scale * u_light.L * (diffuse_color + specular_color);
}

void main()
{
    fragment_color = vec4(CalculateBlinnPhongModel(position_in_view, normalize(normal_in_view)), 1.0);
}