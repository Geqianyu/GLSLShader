#version 460

in vec2 uv;

layout (location = 0) out vec4 fragment_color;

uniform sampler2D u_sprite_texture;

void main()
{
    fragment_color = texture(u_sprite_texture, uv);
}