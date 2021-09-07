#version 330 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D redChannel;
uniform sampler2D greenChannel;
uniform sampler2D blueChannel;
uniform sampler2D alphaChannel;

void main()
{ 
    float red   = texture(redChannel, TexCoords).r;
    float green = texture(greenChannel, TexCoords).r;
    float blue  = texture(blueChannel, TexCoords).r;
    float alpha = texture(alphaChannel, TexCoords).r;

    FragColor = vec4(red, green, blue, alpha);
}