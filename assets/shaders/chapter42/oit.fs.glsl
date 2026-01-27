#version 460

layout (early_fragment_tests) in;

#define MAX_FRAGMENTS 75

layout (location = 0) in vec3 position_in_view;
layout (location = 1) in vec3 normal_in_view;

layout (location = 0) out vec4 fragment_color;

struct NodeType
{
    vec4 color;
    float depth;
    uint next;
};

uniform vec4 u_light_position;
uniform vec3 u_light_intensity;
uniform vec4 u_Kd;
uniform vec4 u_Ka;
uniform uint u_max_nodes;

layout (binding = 0, r32ui) uniform uimage2D u_head_pointers;
layout (binding = 0, offset = 0) uniform atomic_uint u_next_node_index;
layout (binding = 0, std430) buffer LinkedLists
{
    NodeType b_nodes[];
};

subroutine void RenderPassType();
subroutine uniform RenderPassType u_render_pass;

vec3 CalculateDiffuse()
{
    vec3 s = normalize(u_light_position.xyz - position_in_view);
    vec3 n = normalize(normal_in_view);
    return u_light_intensity * (u_Ka.rgb + u_Kd.rgb * max(dot(s, n), 0.0));
}

subroutine(RenderPassType)
void Pass1()
{
    // Get the index of the next empty slot in the buffer
    uint node_index = atomicCounterIncrement(u_next_node_index);

    // Is our buffer full?  If so, we don't add the fragment
    // to the list.
    if (node_index < u_max_nodes )
    {
        // Our fragment will be the new head of the linked list, so
        // replace the value at gl_FragCoord.xy with our new node's
        // index.  We use imageAtomicExchange to make sure that this
        // is an atomic operation.  The return value is the old head
        // of the list (the previous value), which will become the
        // next element in the list once our node is inserted.
        uint prev_head = imageAtomicExchange(u_head_pointers, ivec2(gl_FragCoord.xy), node_index);

        // Here we set the color and depth of this new node to the color
        // and depth of the fragment.  The next pointer, points to the
        // previous head of the list.
        b_nodes[node_index].color = vec4(CalculateDiffuse(), u_Kd.a);
        b_nodes[node_index].depth = gl_FragCoord.z;
        b_nodes[node_index].next = prev_head;
    }
}

subroutine(RenderPassType)
void Pass2()
{
    NodeType frags[MAX_FRAGMENTS];
    int count = 0;

    // Get the index of the head of the list
    uint n = imageLoad(u_head_pointers, ivec2(gl_FragCoord.xy)).r;

    // Copy the linked list for this fragment into an array
    while (n != 0xffffffff && count < MAX_FRAGMENTS)
    {
        frags[count] = b_nodes[n];
        n = frags[count].next;
        ++count;
    }

    // Sort the array by depth using insertion sort (largest
    // to smallest).
    for (uint i = 1; i < count; ++i)
    {
        NodeType to_insert = frags[i];
        uint j = i;
        while (j > 0 && to_insert.depth > frags[j - 1].depth)
        {
            frags[j] = frags[j - 1];
            --j;
        }
        frags[j] = to_insert;
    }

    // Traverse the array, and combine the colors using the alpha
    // channel.
    vec4 color = vec4(0.5, 0.5, 0.5, 1.0);
    for (int i = 0; i < count; ++i)
    {
        color = mix(color, frags[i].color, frags[i].color.a);
    }

    // Output the final color
    fragment_color = color;
}

void main()
{
    u_render_pass();
}