#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform sampler2D u_render_texture;

uniform int u_pass;
uniform float u_weights[5];

uniform struct LightInfo
{
  vec4 position_in_view;
  vec3 L;
  vec3 La;
} u_light;

uniform struct MaterialInfo
{
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float shininess;
} u_material;

vec3 CalculateBlinnPhong(vec3 position, vec3 normal)
{
    vec3 ambient_color = u_light.La * u_material.Ka;

    vec3 s = normalize(u_light.position_in_view.xyz - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if(s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        specular_color = u_material.Ks * pow(max(dot(h, normal), 0.0), u_material.shininess);
    }

    return ambient_color + u_light.L * (diffuse_color + specular_color);
}

vec4 Pass1()
{
    return vec4(CalculateBlinnPhong(position_in_view, normalize(normal_in_view)), 1.0);
}

vec4 Pass2()
{
    ivec2 pixel_uv = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(u_render_texture, pixel_uv, 0) * u_weights[0];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, 1)) * u_weights[1];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, -1)) * u_weights[1];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, 2)) * u_weights[2];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, -2)) * u_weights[2];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, 3)) * u_weights[3];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, -3)) * u_weights[3];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, 4)) * u_weights[4];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, -4)) * u_weights[4];
    return sum;
}

vec4 Pass3()
{
    ivec2 pixel_uv = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(u_render_texture, pixel_uv, 0) * u_weights[0];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(1, 0)) * u_weights[1];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(-1, 0)) * u_weights[1];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(2, 0)) * u_weights[2];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(-2, 0)) * u_weights[2];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(3, 0)) * u_weights[3];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(-3, 0)) * u_weights[3];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(4, 0)) * u_weights[4];
    sum += texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(-4, 0)) * u_weights[4];
    return sum;
}

void main()
{
    if(u_pass == 1)
    {
        fragment_color = Pass1();
    }

    if(u_pass == 2)
    {
        fragment_color = Pass2();
    }

    if(u_pass == 3)
    {
        fragment_color = Pass3();
    }
}