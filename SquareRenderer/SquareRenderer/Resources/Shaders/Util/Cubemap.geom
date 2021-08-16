#version 450 core

//-----------------------------------------------
// CUBEMAP GEOMETRY SHADER: POSITION PASS THROUGH
//-----------------------------------------------

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 VP[6];

// FragPos from GS (output per emitvertex)
out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        // built-in variable that specifies to which face we render.
        gl_Layer = face;

        // for each triangle vertex
        for(int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = VP[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 