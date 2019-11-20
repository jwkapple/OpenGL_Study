#version 330 core
out vec4 LightColor;

in vec4 LightFragColor;

void main()
{
LightColor = LightFragColor;
}