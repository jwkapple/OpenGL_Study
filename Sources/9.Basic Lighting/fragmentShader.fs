#version 330 core
out vec4 objectColor;

in vec4 FragColor;

void main()
{
objectColor = FragColor;
}