#version 460

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out vec3 position_data;
layout (location = 2) out vec3 normal_data;
layout (location = 3) out vec3 color_data;
layout (location = 4) out float ao_data;

const int c_kernel_size = 64;
const vec2 c_rand_scale = vec2(800.0 / 4.0, 600.0 / 4.0);

uniform int u_pass;
uniform mat4 u_projection_matrix;
uniform vec3 u_sampler_kernel[c_kernel_size];
uniform float u_radius = 0.55;

uniform struct LightInfo
{
    vec4 position_in_view;
    vec3 L;
    vec3 La;
} u_light;

uniform struct MaterialInfo
{
    vec3 Kd;
    bool is_use_texture;
} u_material;

layout (binding = 0) uniform sampler2D u_position_texture;
layout (binding = 1) uniform sampler2D u_normal_texture;
layout (binding = 2) uniform sampler2D u_color_texture;
layout (binding = 3) uniform sampler2D u_ao_texture;
layout (binding = 4) uniform sampler2D u_random_texture;
layout (binding = 5) uniform sampler2D u_diffuse_texture;

vec3 CalculateAmbientAndDiffuse(vec3 position, vec3 normal, vec3 diffuse, float ao)
{
    ao = pow(ao, 4.0);
    vec3 ambient_color = u_light.La * diffuse * ao;
    vec3 s = normalize(vec3(u_light.position_in_view) - position);
    float s_dot_n = max(dot(s, normal), 0.0);
    return ambient_color + u_light.L * diffuse * s_dot_n;
}

void Pass1()
{
    position_data = position_in_view;
    normal_data = normalize(normal_in_view);
    if (u_material.is_use_texture)
    {
        color_data = pow(texture(u_diffuse_texture, uv).rgb, vec3(2.2));
    }
    else
    {
        color_data = u_material.Kd;
    }
}

void Pass2()
{
    vec3 rand_direction = normalize(texture(u_random_texture, uv.xy * c_rand_scale).xyz);
    vec3 normal = normalize(texture(u_normal_texture, uv).xyz);
    vec3 bitangent = cross(normal, rand_direction);
    if (length(bitangent) < 0.0001)
    {
        bitangent = cross(normal, vec3(0,0,1));
    }
    bitangent = normalize(bitangent);
    vec3 tangent = cross(bitangent, normal);
    mat3 to_camera_space = mat3(tangent, bitangent, normal);

    float occlusion_sum = 0.0;
    vec3 point_position = texture(u_position_texture, uv).xyz;
    for (int i = 0; i < c_kernel_size; ++i)
    {
        vec3 sample_position = point_position + u_radius * (to_camera_space * u_sampler_kernel[i]);

        vec4 p = u_projection_matrix * vec4(sample_position, 1.0);
        p *= 1.0 / p.w;
        p.xyz = p.xyz * 0.5 + 0.5;

        float surface_z = texture(u_position_texture, p.xy).z;
        float z_dist = surface_z - point_position.z;

        if (z_dist >= 0.0 && z_dist <= u_radius && surface_z > sample_position.z )
        {
            occlusion_sum += 1.0;
        }
    }

    float occ = occlusion_sum / c_kernel_size;
    ao_data = 1.0 - occ;
}

void Pass3()
{
    ivec2 pixel = ivec2(gl_FragCoord.xy);
    float sum = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            sum += texelFetchOffset(u_ao_texture, pixel, 0, ivec2(x, y)).r;
        }
    }

    float ao = sum * (1.0 / 9.0);
    ao_data = ao;
}

void Pass4()
{
    vec3 position = texture(u_position_texture, uv).xyz;
    vec3 normal = texture(u_normal_texture, uv).xyz;
    vec3 diffuse_color = texture(u_color_texture, uv).rgb;
    float ao_val = texture(u_ao_texture, uv).r;

    vec3 col = CalculateAmbientAndDiffuse(position, normal, diffuse_color, ao_val);
    col = pow(col, vec3(1.0 / 2.2));

    fragment_color = vec4(col, 1.0);
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
    else if (u_pass == 3)
    {
        Pass3();
    }
    else if (u_pass == 4)
    {
        Pass4();
    }
}