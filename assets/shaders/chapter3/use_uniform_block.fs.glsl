#version 460

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 fragment_color;

uniform BlobSetting
{
    vec4 u_inner_color;
    vec4 u_outer_color;
    float u_inner_radius;
    float u_outer_radius;
};

void main()
{
    float dx = uv.x - 0.5;
    float dy = uv.y - 0.5;
    float dist = sqrt(dx * dx + dy * dy);
    fragment_color = mix(u_inner_color, u_outer_color, smoothstep(u_inner_radius, u_outer_radius, dist));
    // fragment_color = vec4(uv, 0.0, 1.0);
}