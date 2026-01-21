#version 460

layout (location = 0) in vec3 position;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform samplerCube sky_box_texture;

void main()
{
    vec3 color = texture(sky_box_texture, normalize(position)).rgb;
    color = pow(color, vec3(1.0 / 2.2));
    fragment_color = vec4(color, 1.0);
}