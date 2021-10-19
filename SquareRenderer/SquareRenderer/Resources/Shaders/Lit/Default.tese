#version 410 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

layout(triangles, equal_spacing, ccw) in;

//uniform sampler2D gDisplacementMap;
//uniform float gDispFactor;

in ControlShaderData
{
    vec2 uv;
    vec3 normal;
    vec3 pFragmentWS;

    //mat3 TBN;
} IN[];

out EvaluationShaderData
{
    vec2 uv;
    vec3 normal;
    vec3 pFragmentWS;
    
    //mat3 TBN;
} OUT;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    OUT.uv = interpolate2D(IN[0].uv, IN[1].uv, IN[2].uv);
    OUT.normal = interpolate3D(IN[0].normal, IN[1].normal, IN[2].normal);
    OUT.normal = normalize(OUT.normal);
    OUT.pFragmentWS = interpolate3D(IN[0].pFragmentWS, IN[1].pFragmentWS, IN[2].pFragmentWS);

    // Displace the vertex along the normal
    //float Displacement = texture(gDisplacementMap, TexCoord_FS_in.xy).x;
    //OUT.pFragmentWS += OUT.normal * Displacement * gDispFactor;
    gl_Position = _VP * vec4(OUT.pFragmentWS, 1.0);
}