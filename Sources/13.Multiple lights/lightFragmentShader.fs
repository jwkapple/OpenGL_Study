#version 330 core
out vec4 LightColor;

in vec4 FragColor;

void main()
{
	
	LightColor = FragColor;
}