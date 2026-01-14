#version 460

layout (location = 0) in vec3 color;

layout (location = 0) out vec4 fragment_color;

const float PI = 3.141592653589793;

void main()
{
    float fac = 1.0 + 0.5 * cos(gl_FragCoord.x * 100.0 / PI);
    fragment_color = vec4(fac * color, 1.0);
}