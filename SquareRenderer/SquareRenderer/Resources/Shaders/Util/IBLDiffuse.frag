#version 330 core

#pragma include ../Includes/Globals.glsl //! #include "../Includes/Globals.glsl"

in vec4 FragPos;
out vec4 Irradiance;

uniform samplerCube hdri;

vec3 Convolution(vec3 normal)
{
    vec3 irr = vec3(0.0);  

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up         = cross(normal, right);

    float sampleDelta = 0.025;
    float nrSamples = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irr += texture(hdri, sampleVec).rgb * cos(theta) * sin(theta);

            nrSamples++;
        }
    }
    return PI * irr * (1.0 / float(nrSamples));
}

void main()
{		
    // make sure to normalize localPos
    vec3 direction = normalize(FragPos.xyz);
    Irradiance = vec4(Convolution(direction), 1.0);
}