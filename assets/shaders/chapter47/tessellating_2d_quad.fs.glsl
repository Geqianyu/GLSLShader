#version 460

uniform float u_line_width;
uniform vec4 u_line_color;
uniform vec4 u_quad_color;

noperspective in vec3 edge_distance;

layout (location = 0) out vec4 fragment_color;

float EdgeMix()
{
    float d = min(min(edge_distance.x, edge_distance.y), edge_distance.z);

    if (d < u_line_width - 1.0)
    {
        return 1.0;
    }
    else if (d > u_line_width + 1.0)
    {
        return 0.0;
    }
    else
    {
        float x = d - (u_line_width - 1.0);
        return exp2(-2.0 * (x * x));
    }
}

void main()
{
    float mix_val = EdgeMix();

    fragment_color = mix(u_quad_color, u_line_color, mix_val);
}