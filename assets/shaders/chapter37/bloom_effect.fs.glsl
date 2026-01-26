#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;
layout (location = 2) in vec2 uv_in_view;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform sampler2D u_hdr_texture;
layout (binding = 1) uniform sampler2D u_blur_texture1;
layout (binding = 2) uniform sampler2D u_blur_texture2;

uniform int u_pass;
uniform float u_lum_thresh;
uniform float u_pixel_offsets[10] = float[](0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
uniform float u_weights[10];
uniform float u_ave_lum;

uniform struct LightInfo
{
  vec4 position_in_view;
  vec3 L;
  vec3 La;
} u_lights[3];

uniform struct MaterialInfo
{
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float shininess;
} u_material;

uniform mat3 u_rgb2xyz = mat3
(
    0.4124564, 0.2126729, 0.0193339,
    0.3575761, 0.7151522, 0.1191920,
    0.1804375, 0.0721750, 0.9503041
);

uniform mat3 u_xyz2rgb = mat3
(
     3.2404542, -0.9692660,  0.0556434,
    -1.5371385,  1.8760108, -0.2040259,
    -0.4985314,  0.0415560,  1.0572252
);

uniform float u_exposure = 0.35;
uniform float u_white = 0.928;

float CalculateLuminance(vec3 color)
{
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

vec3 CalculateBlinnPhong(vec3 position, vec3 normal, int light_index)
{
    vec3 ambient_color = u_lights[light_index].La * u_material.Ka;

    vec3 s = normalize(u_lights[light_index].position_in_view.xyz - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if (s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        specular_color = u_material.Ks * pow(max(dot(h, normal), 0.0), u_material.shininess);
    }

    return ambient_color + u_lights[light_index].L * (diffuse_color + specular_color);
}

vec4 Pass1()
{
    vec3 n = normalize(normal_in_view);
    vec3 color = vec3(0.0);
    for (int i = 0; i < 3; ++i)
    {
        color += CalculateBlinnPhong(position_in_view, n, i); 
    }

    return vec4(color,1);
}

vec4 Pass2()
{
    vec4 val = texture(u_hdr_texture, uv_in_view);
    if (CalculateLuminance(val.rgb) > u_lum_thresh)
    {
        return val;
    }
    else
    {
        return vec4(0.0);
    }
}

vec4 Pass3()
{
    float dy = 1.0 / (textureSize(u_blur_texture1, 0)).y;

    vec4 sum = texture(u_blur_texture1, uv_in_view) * u_weights[0];
    for (int i = 1; i < 10; ++i)
    {
         sum += texture(u_blur_texture1, uv_in_view + vec2(0.0, u_pixel_offsets[i]) * dy) * u_weights[i];
         sum += texture(u_blur_texture1, uv_in_view - vec2(0.0, u_pixel_offsets[i]) * dy) * u_weights[i];
    }
    return sum;
}

vec4 Pass4()
{
    float dx = 1.0 / (textureSize(u_blur_texture2, 0)).x;

    vec4 sum = texture(u_blur_texture2, uv_in_view) * u_weights[0];
    for (int i = 1; i < 10; ++i)
    {
       sum += texture(u_blur_texture2, uv_in_view + vec2(u_pixel_offsets[i], 0.0) * dx) * u_weights[i];
       sum += texture(u_blur_texture2, uv_in_view - vec2(u_pixel_offsets[i], 0.0) * dx) * u_weights[i];
    }
    return sum;
}

vec4 Pass5()
{
    /////////////// Tone mapping ///////////////
    vec4 color = texture(u_hdr_texture, uv_in_view);
    vec3 xyz_color = u_rgb2xyz * vec3(color);
    float xyz_sum = xyz_color.x + xyz_color.y + xyz_color.z;
    vec3 xy_Y_color = vec3(xyz_color.x / xyz_sum, xyz_color.y / xyz_sum, xyz_color.y);

    float L = (u_exposure * xy_Y_color.z) / u_ave_lum;
    L = (L * ( 1 + L / (u_white * u_white))) / (1.0 + L);

    xyz_color.x = (L * xy_Y_color.x) / (xy_Y_color.y);
    xyz_color.y = L;
    xyz_color.z = (L * (1 - xy_Y_color.x - xy_Y_color.y)) / xy_Y_color.y;

    vec4 tone_map_color = vec4(u_xyz2rgb * xyz_color, 1.0);

    ///////////// Combine with blurred texture /////////////
    vec4 blur_color = texture(u_blur_texture1, uv_in_view);

     return tone_map_color + blur_color;
}

void main()
{
    if (u_pass == 1)
    {
        fragment_color = Pass1();
    }
    else if (u_pass == 2)
    {
        fragment_color = Pass2();
    }
    else if (u_pass == 3)
    {
        fragment_color = Pass3();
    }
    else if (u_pass == 4)
    {
        fragment_color = Pass4();
    }
    else if (u_pass == 5)
    {
        fragment_color = Pass5();
    }
}