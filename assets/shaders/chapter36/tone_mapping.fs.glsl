#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;
layout (location = 2) in vec2 uv_in_view;

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out vec3 fragment_hdr_color;

layout (binding = 0) uniform sampler2D u_render_texture;

uniform int u_pass;
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
uniform bool u_do_tone_map = true;

vec3 CalculateBlinnPhong(vec3 position, vec3 normal, int light_index)
{
    vec3 ambient_color = u_lights[light_index].La * u_material.Ka;

    vec3 s = normalize(u_lights[light_index].position_in_view.xyz - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    vec3 diffuse_color = u_material.Kd * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if(s_dot_n > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        specular_color = u_material.Ks * pow(max(dot(h, normal), 0.0), u_material.shininess);
    }

    return ambient_color + u_lights[light_index].L * (diffuse_color + specular_color);
}

void Pass1()
{
    vec3 n = normalize(normal_in_view);
    fragment_hdr_color = vec3(0.0);
    for( int i = 0; i < 3; i++)
    {
        fragment_hdr_color += CalculateBlinnPhong(position_in_view, n, i); 
    }
}

void Pass2()
{
    vec4 color = texture(u_render_texture, uv_in_view);

    // Convert to XYZ
    vec3 xyz_color = u_rgb2xyz * vec3(color);

    // Convert to xyY
    float xyz_sum = xyz_color.x + xyz_color.y + xyz_color.z;
    vec3 xy_Y_color = vec3(xyz_color.x / xyz_sum, xyz_color.y / xyz_sum, xyz_color.y);

    // Apply the tone mapping operation to the luminance (xyYCol.z or xyzCol.y)
    float L = (u_exposure * xy_Y_color.z) / u_ave_lum;
    L = (L * (1.0 + L / (u_white * u_white))) / (1.0 + L);

    // Using the new luminance, convert back to XYZ
    xyz_color.x = (L * xy_Y_color.x) / (xy_Y_color.y);
    xyz_color.y = L;
    xyz_color.z = (L * (1 - xy_Y_color.x - xy_Y_color.y)) / xy_Y_color.y;

    // Convert back to RGB and send to output buffer
    if (u_do_tone_map)
    {
        fragment_color = vec4(u_xyz2rgb * xyz_color, 1.0);
    }
    else
    {
        fragment_color = color;
    }
}

void main()
{
    if (u_pass == 1)
    {
        Pass1();
    }
    else if (u_pass == 2)
    {
        Pass2();
    }
}