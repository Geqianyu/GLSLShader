#version 460

layout (location = 0) in vec3 world_position;
layout (location = 1) in vec3 world_normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform samplerCube u_ibl;
layout (binding = 1) uniform sampler2D u_diffuse_texture;

uniform vec3 u_camera_position;

const float PI = 3.14159265358979323846;

vec3 CalculateSchlickFresnel(float dot_product)
{
    vec3 f0 = vec3(0.04);
    return f0 + (1 - f0) * pow(1.0 - dot_product, 5);
}

void main()
{
    float gamma = 2.2;
    vec3 normal = normalize(world_normal);
    vec3 v = normalize(u_camera_position - world_position);

    vec3 light_color = texture(u_ibl, normal).rgb;
    vec3 color = texture(u_diffuse_texture, uv).rgb;

    color = pow(color, vec3(gamma));

    color *= light_color;

    color = pow(color, vec3(1.0 / gamma));

    fragment_color = vec4(color, 1);
}