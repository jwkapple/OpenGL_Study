#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;

uniform float exposure;

void main()
{

 const float gamma = 2.2;
 vec3 col = texture(diffuse, TexCoord).rgb;
 
 vec3 mapped = col / (col + vec3(1.0));
 mapped = pow(mapped, vec3(1.0 / gamma));

 FragColor = vec4(mapped,1.0);

}