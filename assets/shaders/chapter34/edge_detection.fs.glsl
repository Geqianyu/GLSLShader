#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform sampler2D u_render_texture;

uniform float u_edge_threshold;
uniform int u_pass;

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

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);

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

float CalculateLuminance(vec3 color)
{
    return dot(lum, color);
}

vec4 Pass2()
{
    ivec2 pixel_uv = ivec2(gl_FragCoord.xy);

    float s00 = CalculateLuminance(texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(-1, 1)).rgb);
    float s10 = CalculateLuminance(texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(-1, 0)).rgb);
    float s20 = CalculateLuminance(texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(-1, -1)).rgb);
    float s01 = CalculateLuminance(texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, 1)).rgb);
    float s21 = CalculateLuminance(texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(0, -1)).rgb);
    float s02 = CalculateLuminance(texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(1, 1)).rgb);
    float s12 = CalculateLuminance(texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(1, 0)).rgb);
    float s22 = CalculateLuminance(texelFetchOffset(u_render_texture, pixel_uv, 0, ivec2(1, -1)).rgb);

    float sx = s00 + 2.0 * s10 + s20 - (s02 + 2.0 * s12 + s22);
    float sy = s00 + 2.0 * s01 + s02 - (s20 + 2.0 * s21 + s22);

    float g = sx * sx + sy * sy;

    if (g > u_edge_threshold)
    {
        return vec4(1.0);
    }
    else
    {
        return vec4(0.0,0.0,0.0,1.0);
    }
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
}