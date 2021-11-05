//? #version 450 core
//? #include "Globals.glsl"
//? #include "Sampling.glsl"

//-----------------------------------------------
//            PBR FUNCTIONS
//-----------------------------------------------

vec3 _fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float _distributionGGX(in vec3 N, in vec3 H, in float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float _geometrySchlickGGX_mixed(
    in float NdotV,
    in float roughness,
    in bool ibl)
{
    float r = roughness + (1.0 - float(ibl));
    float k = (r * r) / (8.0 * 1 - (0.75 * float(ibl)));

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float _geometrySchlickGGX(
    in float NdotV,
    in float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float _geometrySchlickGGX_IBL(float NdotV, float roughness)
{
    // note that we use a different k for IBL
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float _geometrySmith_mixed(
    in vec3 N,
    in vec3 V,
    in vec3 L,
    in float roughness,
    in bool ibl)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = _geometrySchlickGGX_mixed(NdotV, roughness, ibl);
    float ggx1 = _geometrySchlickGGX_mixed(NdotL, roughness, ibl);

    return ggx1 * ggx2;
}

float _geometrySmith(
    in vec3 N,
    in vec3 V,
    in vec3 L,
    in float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = _geometrySchlickGGX(NdotV, roughness);
    float ggx1 = _geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float _geometrySmith_IBL(
    in vec3 N,
    in vec3 V,
    in vec3 L,
    in float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = _geometrySchlickGGX_IBL(NdotV, roughness);
    float ggx1 = _geometrySchlickGGX_IBL(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 _fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 _importanceSampleGGX(vec2 Xi, vec3 N, float r)
{
    float a = r * r;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}