#version 460

layout (location = 0) in vec3 light_direction;
layout (location = 1) in vec2 uv_in_view;
layout (location = 2) in vec3 view_direction;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform sampler2D u_color_texture;
layout (binding = 1) uniform sampler2D u_normal_texture;
layout (binding = 2) uniform sampler2D u_height_texture;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 L;
    vec3 La;
} u_light;

uniform struct MaterialInfo
{
    vec3 Ks;
    float shininess;
} u_material;

vec3 CalculateBlinnPhong()
{
    vec3 v = normalize(view_direction);
    vec3 s = normalize(light_direction);

    const float bump_factor = 0.009;
    float height = 1.0 - texture(u_height_texture, uv_in_view).r;
    vec2 delta = v.xy * height * bump_factor / v.z;
    vec2 uv = uv_in_view - delta;

    vec3 normal = texture(u_normal_texture, uv).xyz;
    normal.xy = 2.0 * normal.xy - 1.0;
    normal = normalize(normal);

    float s_dot_n = max(dot(s, normal), 0.0);

    vec3 color = texture(u_color_texture, uv).rgb;

    vec3 ambient_color = u_light.La * color;

    vec3 diffuse_color = color * s_dot_n;

    vec3 specular_color = vec3(0.0);
    if(s_dot_n > 0.0)
    {
        vec3 h = normalize(v + s);
        specular_color = u_material.Ks * pow(max(dot(h, normal), 0.0), u_material.shininess);
    }
    return ambient_color + u_light.L * (diffuse_color + specular_color);
}

void main()
{
    vec3 color = CalculateBlinnPhong();
    color = pow(color, vec3(1.0 / 2.2));
    fragment_color = vec4(color, 1.0);
}