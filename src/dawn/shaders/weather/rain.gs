#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;
uniform mat4 projection;
uniform mat4 view;

void build_house(vec4 position)
{    
    fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
    gl_Position = projection*view*(position + vec4(-1, -1, 0.0, 0.0)); // 1:bottom-left   
    EmitVertex();   
    gl_Position =  projection*view*(position + vec4( 1, 1, 0.0, 0.0)); // 2:bottom-right
    EmitVertex();
    EndPrimitive();
}

void main() {    
    build_house(gl_in[0].gl_Position);
}