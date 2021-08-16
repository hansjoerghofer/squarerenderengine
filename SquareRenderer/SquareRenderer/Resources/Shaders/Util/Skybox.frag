#version 450 core

in vec3 uvs;

uniform samplerCube skybox;

void main()
{
    gl_FragColor = texture(skybox, uvs);
    //gl_FragColor = mix(vec4(1,0,0,1), texture(skybox, uvs), 0.1);

    // DEBUG MIP levels
    //gl_FragColor = textureLod(skybox, uvs, 0.8);
}