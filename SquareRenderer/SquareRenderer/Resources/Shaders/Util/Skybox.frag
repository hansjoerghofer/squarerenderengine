#version 450 core

out vec4 FragColor;

in vec3 uvs;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, uvs);
    //FragColor = mix(vec4(1,0,0,1), texture(skybox, uvs), 0.1);

    // DEBUG MIP levels
    //FragColor = textureLod(skybox, uvs, 0.8);
}