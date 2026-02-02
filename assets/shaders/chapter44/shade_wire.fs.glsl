#version 460

in vec3 g_normal;
in vec3 g_position;
noperspective in vec3 edge_distance;

layout (location = 0) out vec4 fragment_color;

uniform struct LightInfo
{
    vec4 position; 
    vec3 intensity;
} u_light;

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
} u_material;

uniform struct LineInfo
{
    float width;
    vec4 color;
} u_line;

vec3 CalculatePhong(vec3 position, vec3 normal)
{
    vec3 s = normalize(vec3(u_light.position) - position);

    vec3 ambient_color = u_light.intensity * u_material.Ka;

    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_light.intensity * u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if (s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s, normal);
        specular_color = u_light.intensity * u_material.Ks * pow(max(dot(r, v), 0.0), u_material.shininess);
    }

    return ambient_color + diffuse_color + specular_color;
}

void main()
{
    vec4 color = vec4(CalculatePhong(g_position, g_normal), 1.0);

    float d = min(edge_distance.x, edge_distance.y);
    d = min(d, edge_distance.z);

    float mix_val = 0.0;
    if (d < u_line.width - 1.0)
    {
        mix_val = 1.0;
    }
    else if (d > u_line.width + 1.0)
    {
        mix_val = 0.0;
    }
    else
    {
        float x = d - (u_line.width - 1.0);
        mix_val = exp2(-2.0 * (x * x));
    }
    fragment_color = mix(color, u_line.color, mix_val);
}