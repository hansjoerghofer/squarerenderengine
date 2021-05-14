#version 450 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"
#pragma include ../Includes/Lights.glsl //! #include "../Includes/Lights.glsl"

in VSData
{
    vec2 uv;
    vec3 normalWS;
    vec3 fragPosWS;
} vs_in;

uniform vec4 albedoColor = vec4(1,1,1,1);
uniform float shininessFactor = 0.5;

void main() 
{
    float shininess = 128.0 * shininessFactor + 1;

    vec3 viewDirWS = normalize(_invV[3].xyz - vs_in.fragPosWS);

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

        shadedColor += (diff * _lightsColor[i].rgb) + (spec * _lightsColor[i].rgb);
    }

    gl_FragColor = vec4(shadedColor * albedoColor.rgb, 1);
}