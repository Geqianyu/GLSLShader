#version 460

layout (location = 0) in vec3 world_position;
layout (location = 1) in vec3 world_normal;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform samplerCube u_sky_box_cube_map;

uniform struct MaterialInfo
{
    float eta;                  // 环境折射率 n1 和 物体折射率 n2 的壁纸 n1 / n2
    float reflecttion_factor;   // 被反射的光的因子
} u_material;

uniform vec3 u_camera_world_position;

void main()
{
    vec3 normal = normalize(world_normal);
    vec3 view_direction = normalize(u_camera_world_position - world_position);
    vec3 reflect_direction = reflect(-view_direction, normal);
    vec3 refract_direction = refract(-view_direction, normal, u_material.eta);

    vec3 reflect_color = texture(u_sky_box_cube_map, reflect_direction).rgb;
    vec3 refract_color = texture(u_sky_box_cube_map, refract_direction).rgb;

    vec3 color = mix(refract_color, reflect_color, u_material.reflecttion_factor);

    color = pow(color, vec3(1.0 / 2.2));
    fragment_color = vec4(color, 1.0);
}