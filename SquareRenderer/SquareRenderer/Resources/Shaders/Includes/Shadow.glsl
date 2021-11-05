//? #version 450 core
//? #include "Lights.glsl"
//? #include "Sampling.glsl"

uniform vec4 _shadowMapDim; // resolution (w,h,1/w,1/h)
uniform sampler2D[_MAX_SIZE_LIGHT] _shadowMaps;

// PCF
uniform float pcfRadius = 4; // [1,100]

// PCSS
uniform int pcssBlockerSamples = 16; // [1,64]
uniform int pcssShadowSamples = 32; // [2,128]
uniform float lightSize = 10; // [1,100]
uniform float pcssPenumbraFactor = 1; // [1,100]

//-----------------------------------------------
//            Shadow Mapping
//-----------------------------------------------
float _hardShadow(in vec4 position, int index)
{
    vec3 projCoords = position.xyz / position.w;

    // Not needed because of bias matrix
    // transform to [0,1] range
    // projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    if (currentDepth > 1.0) return 1;

    float closestDepth = texture(_shadowMaps[index], projCoords.xy).r;
    return currentDepth > closestDepth ? 0.0 : 1.0;
    //return texture(_shadowMap[index], projCoords);
}

float _shadowPCFHammersley(in vec4 position, int index)
{
    vec3 projCoords = position.xyz / position.w;
    float currentDepth = projCoords.z;

    if (currentDepth > 1.0) return 1;

    float visibility = 0;
    vec2 sampleOffset;
    vec3 lookup;

    const uint SAMPLE_COUNT = 32u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        lookup = projCoords;
        sampleOffset = _hammersley(i, SAMPLE_COUNT) * 2 - 1;
        sampleOffset *= pcfRadius;

        lookup.xy += _shadowMapDim.zw * sampleOffset;

        float closestDepth = texture(_shadowMaps[index], lookup.xy).r;
        visibility += currentDepth > closestDepth ? 0.0 : 1.0;
        //visibility += texture(_shadowMap[index], lookup);
    }

    visibility /= SAMPLE_COUNT;

    return visibility;
}

float _shadowPCSSHammersley(in vec4 position, int index)
{
    vec3 projCoords = position.xyz / position.w;
    float receiverDepth = projCoords.z;

    if (receiverDepth > 1.0) return 1;

    // blocker search
    float lightSize2 = lightSize * 2;
    float blockerDepth = 0;
    uint blockedSampleCount = 0;
    const uint BLOCKER_SAMPLE_COUNT = uint(pcssBlockerSamples);
    const float INV_COUNT = 1.0 / float(BLOCKER_SAMPLE_COUNT);
    for (uint i = 0u; i < BLOCKER_SAMPLE_COUNT; ++i)
    {
        vec2 sampleOffset = _hammersleyOpt(i, INV_COUNT) * 2 - 1;
        sampleOffset *= lightSize2;

        vec2 lookup = _shadowMapDim.zw * sampleOffset + projCoords.xy;
        float closestDepth = texture(_shadowMaps[index], lookup).r;

        if (receiverDepth > closestDepth)
        {
            blockerDepth += closestDepth;
            blockedSampleCount++;
        }
    }

    // fragment is either in full light or complete shadow
    if (blockedSampleCount == 0 || blockedSampleCount == BLOCKER_SAMPLE_COUNT)
    {
        // either 1 for unblocked (blockedSampleCount == 0)
        // or 0 for completely blocked (blockedSampleCount == BLOCKER_SAMPLE_COUNT)
        return 1 - clamp(blockedSampleCount, 0u, 1u);
    }

    blockerDepth /= blockedSampleCount;

    // penumbra estimation
    float penumbraRadius = lightSize2 * (receiverDepth - blockerDepth) / blockerDepth;

    // filtering
    float visibility = 0;
    uint SAMPLE_COUNT = uint(pcssShadowSamples);//penumbraRadius > lightSize2 * 0.1f ? 64u : 32u;
    float INV_SAMPLE_COUNT = 1.0 / float(SAMPLE_COUNT);
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 sampleOffset = _hammersleyOpt(i, INV_SAMPLE_COUNT) * 2 - 1;
        sampleOffset *= penumbraRadius * pcssPenumbraFactor;

        vec2 lookup = _shadowMapDim.zw * sampleOffset + projCoords.xy;
        float closestDepth = texture(_shadowMaps[index], lookup.xy).r;

        visibility += float(receiverDepth <= closestDepth);
    }
    visibility *= INV_SAMPLE_COUNT;

    return visibility;
}

float _shadow(in vec4 position, int index)
{

#if SHADOW_HARD || SHADOW_PCF || SHADOW_PCSS

    if (_shadowMapIndex[index].x >= 0)
    {
    #if SHADOW_PCSS

        return _shadowPCSSHammersley(position, int(_shadowMapIndex[index].x));

    #elif SHADOW_PCF

        return _shadowPCFHammersley(position, int(_shadowMapIndex[index].x));

    #else

        return _hardShadow(position, int(_shadowMapIndex[index].x));

    #endif
    }
    else
#endif
    {
        return 1.0;
    }
}