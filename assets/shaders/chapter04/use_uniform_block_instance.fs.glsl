#version 460

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 fragment_color;

uniform BlobSetting
{
    vec4 inner_color;
    vec4 outer_color;
    float inner_radius;
    float outer_radius;
} u_Bolb;

void main()
{
    float dx = uv.x - 0.5;
    float dy = uv.y - 0.5;
    float dist = sqrt(dx * dx + dy * dy);
    fragment_color = mix(u_Bolb.inner_color, u_Bolb.outer_color, smoothstep(u_Bolb.inner_radius, u_Bolb.outer_radius, dist));
}