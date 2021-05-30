#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float gamma = 2.2; // [1, 5]
uniform float exposure = 1.0; // [0, 10]

void main()
{ 
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;

    // reinhard tone mapping
    //vec3 toneMapped = hdrColor / (hdrColor + vec3(1.0));
    // exposure tone mapping
    vec3 toneMapped = vec3(1.0) - exp(-hdrColor * exposure);

    vec3 gammaCorrected = pow(toneMapped, vec3(1.0 / gamma));

    FragColor = vec4(gammaCorrected, 1.f);
}