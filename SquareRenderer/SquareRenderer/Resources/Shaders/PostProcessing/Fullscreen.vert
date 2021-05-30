#version 330 core
out vec2 TexCoords;

void main()
{
    // Screen filling triange is generated from Vertex Index.
    //
    // *
    // | *
    // |---*
    // |   | *
    // *-------*
    //
    // i: ( x, y, z, w), (u,v)
    //------------------------
    // 0: (-1,-1, 0, 1), (0,0)
    // 1: ( 3,-1, 0, 1), (2,0)
    // 3: (-1, 3, 0, 1), (0,2)
    //
    TexCoords = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(TexCoords * 2.0f + -1.0f, 0.0f, 1.0f);
} 