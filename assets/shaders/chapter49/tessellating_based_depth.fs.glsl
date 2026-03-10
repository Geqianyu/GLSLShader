#version 460

uniform float u_line_width;
uniform vec4 u_line_color;
uniform vec4 u_light_position;
uniform vec3 u_light_intensity;
uniform vec3 u_Kd;

noperspective in vec3 edge_distance;
in vec3 normal;
in vec4 position;

layout (location = 0) out vec4 fragment_color;

vec3 DiffuseModel(vec3 pos, vec3 norm)
{
    vec3 s = normalize(vec3(u_light_position) - pos);
    float s_dot_n = max(dot(s, norm), 0.0);
    vec3 diffuse = u_light_intensity * u_Kd * s_dot_n;

    return diffuse;
}

float EdgeMix()
{
    float d = min(min(edge_distance.x, edge_distance.y), edge_distance.z);

    if (d < u_line_width - 1)
    {
        return 1.0;
    }
    else if (d > u_line_width + 1)
    {
        return 0.0;
    }
    else
    {
        float x = d - (u_line_width - 1);
        return exp2(-2.0 * (x * x));
    }
}

void main()
{
    float mix_val = EdgeMix();
    vec4 color = vec4(DiffuseModel(position.xyz, normalize(normal)), 1.0);
    color = pow(color, vec4(1.0 / 2.2));
    fragment_color = mix(color, u_line_color, mix_val);
}