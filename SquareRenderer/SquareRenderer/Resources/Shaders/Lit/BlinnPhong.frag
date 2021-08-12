#version 450 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"
#pragma include ../Includes/Lights.glsl //! #include "../Includes/Lights.glsl"
#pragma include ../Includes/Sampling.glsl //! #include "../Includes/Sampling.glsl"
#pragma include ../Includes/Shadow.glsl //! #include "../Includes/Shadow.glsl"

in VSData
{
    vec2 uv;
    vec3 normalWS;
    vec3 fragPosWS;

    vec4[_MAX_SIZE_LIGHT] fragPosLS;
} vs_in;

uniform sampler2D albedo;
uniform vec4 albedoColor = vec4(1,1,1,1);
uniform float shininessFactor = 0.5;

void main() 
{
    float shininess = (256.0 * shininessFactor) + 1;

    vec3 viewDirWS = normalize(_cameraPosition() - vs_in.fragPosWS);

    vec3 shadedColor = _ambientColor.rgb;
    for(int i = 0; i < _numLights; ++i)
    {
        vec3 lightDirWS;

        // point light
        if(_lightsPosWS[i].w > 0)
        {
            lightDirWS = normalize(_lightsPosWS[i].xyz - vs_in.fragPosWS);
        }
        // directional light
        else
        {
            lightDirWS = -_lightsPosWS[i].xyz;
        }

        vec3 H = normalize(lightDirWS + viewDirWS);
        float diff = max(dot(vs_in.normalWS, lightDirWS), 0);
        float spec = pow(max(dot(vs_in.normalWS, H), 0), shininess);

        float visibility = _shadow(vs_in.fragPosLS[i], i);

        shadedColor += visibility * ((diff * _lightsColor[i].rgb) + (spec * _lightsColor[i].rgb));
    }

    vec3 unlitColor = albedoColor.rgb * texture(albedo, vs_in.uv).rgb;

    gl_FragColor = vec4(shadedColor * unlitColor, 1);
}