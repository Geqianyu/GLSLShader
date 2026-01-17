#version 460

layout (location = 0) in vec3 front_color;
layout (location = 1) in vec3 back_color;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 fragment_color;

void main()
{
    const float scale = 15.0;

    bvec2 to_tiscard = greaterThan(fract(uv * scale), vec2(0.2, 0.2));

    if(all(to_tiscard))
    {
        discard;
    }
    else
    {
        if(gl_FrontFacing)
        {
            fragment_color = vec4(front_color, 1.0);
        }
        else
        {
            fragment_color = vec4(back_color, 1.0);
        }
    }
}