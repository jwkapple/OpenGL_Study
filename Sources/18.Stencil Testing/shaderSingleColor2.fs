#version 330 core
out vec4 LightColor;

in vec2 TexCoord;

uniform sampler2D diffuseMap;
void main()
{
	LightColor = vec4(vec3(texture(diffuseMap,TexCoord)),1.0);
}