#version 460

layout (location = 0) in vec3 world_position;
layout (location = 1) in vec3 world_normal;

layout (location = 0) out vec4 fragment_color;

layout (binding = 0) uniform samplerCube u_sky_box_cube_map;

uniform vec3 u_camera_world_position;
uniform float u_reflect_factor;
uniform vec4 u_material_color;

void main()
{
    vec3 view_direction = normalize(u_camera_world_position - world_position);
    vec3 reflect_direction = reflect(-view_direction, normalize(world_normal));
    vec3 cube_map_color = texture(u_sky_box_cube_map, reflect_direction).rgb;
    cube_map_color = pow(cube_map_color, vec3(1.0 / 2.2));
    fragment_color = vec4(mix(u_material_color.rgb, cube_map_color, u_reflect_factor), 1.0);
}