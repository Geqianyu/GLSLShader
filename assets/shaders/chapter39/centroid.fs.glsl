#version 460

// layout (location = 0) centroid in vec2 uv;
layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 fragment_color;

void main()
{
    vec3 yellow = vec3(1.0, 1.0, 0.0);
    vec3 color = vec3(0.0);
    if (uv.s > 1.0)
    {
        color = yellow;
    }
    fragment_color = vec4(color, 1.0);
}