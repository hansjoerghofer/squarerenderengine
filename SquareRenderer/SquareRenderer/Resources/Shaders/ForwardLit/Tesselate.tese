#version 410 core

#pragma include ../Includes/Common.glsl //! #include "../Includes/Common.glsl"
#pragma include ../Includes/Lights.glsl //! #include "../Includes/Lights.glsl"
#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

layout(triangles, equal_spacing, ccw) in;

uniform mat4 modelToWorld = mat4(1);
uniform mat4 normalToWorld = mat4(1);

uniform sampler2D pbrAttributesTexture; // [white]
uniform float displacementFactor = 1;

in ControlShaderData
{
    vec2 uv;
    vec3 normalTS;
    vec3 tangentTS;
    vec3 fragmentPosWS;
} IN[];

out VertexShaderData
{
    vec2 uv;
    vec3 normalWS;
    vec3 fragmentPosWS;

    mat3 TBN;

    vec4[_MAX_SIZE_LIGHT] pFragmentLS;
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

    vec3 normalTS = interpolate3D(IN[0].normalTS, IN[1].normalTS, IN[2].normalTS);
    normalTS = normalize(normalTS);

    vec3 tangentTS = interpolate3D(IN[0].tangentTS, IN[1].tangentTS, IN[2].tangentTS);
    tangentTS = normalize(tangentTS);

    OUT.TBN = _constructTBN(modelToWorld, normalTS, tangentTS);
    OUT.normalWS = normalize(normalToWorld * vec4(normalTS, 0)).xyz;

    OUT.fragmentPosWS = interpolate3D(IN[0].fragmentPosWS, IN[1].fragmentPosWS, IN[2].fragmentPosWS);

    for(int i = 0; i < _numLights; ++i)
    {
        OUT.pFragmentLS[i] = _lightsMatrix[i] * vec4(OUT.fragmentPosWS, 1);
    }

    // Displace the vertex along the normal
    float displacement = texture(pbrAttributesTexture, OUT.uv).w;
    OUT.fragmentPosWS += OUT.normalWS * displacement * displacementFactor;

    gl_Position = _VP * vec4(OUT.fragmentPosWS, 1);
}