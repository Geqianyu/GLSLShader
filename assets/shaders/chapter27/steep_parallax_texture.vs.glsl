#version 460

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;
layout (location = 3) in vec4 vertex_tangent;

layout (location = 0) out vec3 light_direction;
layout (location = 1) out vec2 uv_in_view;
layout (location = 2) out vec3 view_direction;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 L;
    vec3 La;
} u_light;

uniform mat4 u_view_model_matrix;
uniform mat3 u_normal_matrix;
uniform mat4 u_mvp_matrix;

void main()
{
    uv_in_view = vertex_uv;

    vec3 normal = normalize(u_normal_matrix * vertex_normal);
    vec3 tangent = normalize(u_normal_matrix * vertex_tangent.xyz);
    vec3 bi_tangent = normalize(cross(normal, tangent));

    mat3 to_object_local = transpose(mat3(tangent, bi_tangent, normal));

    vec3 position_in_view = (u_view_model_matrix * vec4(vertex_position, 1.0)).xyz;
    light_direction = normalize(to_object_local * (u_light.position_in_view.xyz - position_in_view));
    view_direction = to_object_local * normalize(-position_in_view);

    gl_Position = u_mvp_matrix * vec4(vertex_position, 1.0);
}