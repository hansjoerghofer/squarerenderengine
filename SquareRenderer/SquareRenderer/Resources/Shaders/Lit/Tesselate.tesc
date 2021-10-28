#version 410 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

// define the number of CPs in the output patch
layout (vertices = 3) out;

in ControlPointData
{
    vec2 uv;
    vec3 normalTS;
    vec3 tangentTS;
    vec3 fragmentPosWS;
} IN[];

out ControlShaderData
{
    vec2 uv;
    vec3 normalTS;
    vec3 tangentTS;
    vec3 fragmentPosWS;
} OUT[];

uniform float tesselationLevel = 100.0; // [1, 100]

/*float GetTessLevel(float Distance0, float Distance1)
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
}*/

void main()
{
    // Set the control points of the output patch
    OUT[gl_InvocationID].uv             = IN[gl_InvocationID].uv;
    OUT[gl_InvocationID].normalTS       = IN[gl_InvocationID].normalTS;
    OUT[gl_InvocationID].tangentTS      = IN[gl_InvocationID].tangentTS;
    OUT[gl_InvocationID].fragmentPosWS  = IN[gl_InvocationID].fragmentPosWS;

    // Calculate the distance from the camera to the three control points
    /*float EyeToVertexDistance0 = distance(_cameraPosition(), IN[0].fragmentPosWS);
    float EyeToVertexDistance1 = distance(_cameraPosition(), IN[1].fragmentPosWS);
    float EyeToVertexDistance2 = distance(_cameraPosition(), IN[2].fragmentPosWS);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
                           
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];*/

    gl_TessLevelOuter[0] = tesselationLevel;
    gl_TessLevelOuter[1] = tesselationLevel;
    gl_TessLevelOuter[2] = tesselationLevel;

    gl_TessLevelInner[0] = tesselationLevel;
}