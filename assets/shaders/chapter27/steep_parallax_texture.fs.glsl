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

const float bump_scale = 0.03;

vec2 FindOffset(vec3 v, out float height)
{
    const int n_steps = int(mix(60.0, 10.0, abs(v.z)));
    float height_step = 1.0 / n_steps;
    vec2 delta_t = (v.xy * bump_scale) / (n_steps * v.z);
    float h = 1.0;
    vec2 tc = uv_in_view.xy;
    height = texture(u_height_texture, tc).r;
    while(height < h)
    {
        h -= height_step;
        tc -= delta_t;
        height = texture(u_height_texture, tc).r;
    }
    return tc;
}

bool IsOccluded(float height, vec2 tc, vec3 s)
{
    const int n_shadow_steps = int(mix(60.0, 10.0, abs(s.z)));
    float height_step = 1.0 / n_shadow_steps;
    vec2 delta_t = (s.xy * bump_scale) / (n_shadow_steps * s.z);
    float h = height + height_step * 0.1;
    while(height < h && h < 1.0)
    {
        h += height_step;
        tc += delta_t;
        height = texture(u_height_texture, tc).r;
    }

    return h < 1.0;
}

vec3 CalculateBlinnPhong()
{
    vec3 v = normalize(view_direction);
    vec3 s = normalize(light_direction);

    float height = 1.0;
    vec2 tc = FindOffset(v, height);

    vec3 color = texture(u_color_texture, tc).rgb;
    vec3 n = texture(u_normal_texture, tc).xyz;
    n.xy = 2.0 * n.xy - 1.0;
    n  = normalize(n);

    vec3 ambient_color = u_light.La * color;

    float s_dot_n = max(dot(s, n), 0.0);
    vec3 diffuse_color = vec3(0.0);
    vec3 specular_color = vec3(0.0);

    if(s_dot_n > 0.0 && ! IsOccluded(height, tc, s))
    {
        diffuse_color = color * s_dot_n;
        vec3 h = normalize(v + s);
        specular_color = u_material.Ks * pow(max(dot(h, n), 0.0), u_material.shininess);
    }

    return ambient_color + u_light.L * (diffuse_color + specular_color);
}

void main()
{
    vec3 color = CalculateBlinnPhong();
    color = pow(color, vec3(1.0 / 2.2));
    fragment_color = vec4(color, 1.0);
}