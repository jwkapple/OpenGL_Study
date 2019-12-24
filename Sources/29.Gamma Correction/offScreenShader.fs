#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;



void main()
{
    vec3 col= texture(texture1,TexCoord).rgb;
    float  gamma = 2.2;
    FragColor.rgb = pow(col,vec3(1.0/gamma));
}