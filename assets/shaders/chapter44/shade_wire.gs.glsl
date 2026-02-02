#version 460

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

in vec3 view_position[];
in vec3 view_normal[];

out vec3 g_normal;
out vec3 g_position;
noperspective out vec3 edge_distance;

uniform mat4 u_viewport_matrix;

void main()
{
    vec2 p0 = vec2(u_viewport_matrix * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec2 p1 = vec2(u_viewport_matrix * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec2 p2 = vec2(u_viewport_matrix * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));

    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos((b * b + c * c - a * a) / (2.0 * b * c));
    float beta = acos((a * a + c * c - b * b) / (2.0 * a * c));
    float ha = abs(c * sin(beta));
    float hb = abs(c * sin(alpha));
    float hc = abs(b * sin(alpha));

    edge_distance = vec3(ha, 0.0, 0.0);
    g_normal = view_normal[0];
    g_position = view_position[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    edge_distance = vec3(0.0, hb, 0.0);
    g_normal = view_normal[1];
    g_position = view_position[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    edge_distance = vec3(0.0, 0.0, hc);
    g_normal = view_normal[2];
    g_position = view_position[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}