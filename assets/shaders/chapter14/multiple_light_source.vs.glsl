#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

layout (location = 0) out vec3 color;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 La;
    vec3 L;
} u_lights[5];

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
} u_material;

uniform mat4 u_view_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;

void GetViewSpace(out vec3 normal, out vec3 position)
{
    normal = normalize(u_normal_matrix * vertex_normal);
    position = (u_view_model_matrix * vec4(vertex_position, 1.0)).xyz;
}

vec3 CalculatePhongModel(int light_index, vec3 position, vec3 normal)
{
    vec3 ambient_color = u_lights[light_index].La * u_material.Ka;

    vec3 s = normalize(u_lights[light_index].position_in_view.xyz - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_lights[light_index].L * u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if(s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s, normal);
        specular_color = u_lights[light_index].L * u_material.Ks * pow(max(dot(r, v), 0.0), u_material.shininess);
    }

    return ambient_color + diffuse_color + specular_color;
}

void main()
{
    vec3 view_normal;
    vec3 view_position;

    GetViewSpace(view_normal, view_position);

    color = vec3(0.0);
    for (int i = 0; i < 5; ++i)
    {
        color += CalculatePhongModel(i, view_position, view_normal);
    }

    gl_Position = u_mvp_matrix * vec4(vertex_position,1.0);
}