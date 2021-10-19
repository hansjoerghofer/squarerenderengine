#version 410 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

// define the number of CPs in the output patch
layout (vertices = 3) out;

in VertexShaderData
{
    vec2 uv;
    vec3 normal;
    vec3 pFragmentWS;
    
    //mat3 TBN;
} IN[];

out ControlShaderData
{
    vec2 uv;
    vec3 normal;
    vec3 pFragmentWS;

    //mat3 TBN;
} OUT[];

uniform float level = 1.0; // [1, 10]

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;

    if (AvgDistance <= 2.0) {
        return round(10.0 * level);
    }
    else if (AvgDistance <= 5.0) {
        return round(7.0 * level);
    }
    else {
        return round(3 * level);
    }
}

void main()
{
    // Set the control points of the output patch
    OUT[gl_InvocationID].uv = IN[gl_InvocationID].uv;
    OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
    OUT[gl_InvocationID].pFragmentWS = IN[gl_InvocationID].pFragmentWS;

    // Calculate the distance from the camera to the three control points
    float EyeToVertexDistance0 = distance(_cameraPosition(), IN[0].pFragmentWS);
    float EyeToVertexDistance1 = distance(_cameraPosition(), IN[1].pFragmentWS);
    float EyeToVertexDistance2 = distance(_cameraPosition(), IN[2].pFragmentWS);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}