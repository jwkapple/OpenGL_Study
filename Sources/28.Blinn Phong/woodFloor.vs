#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;


uniform mat4 view;
uniform mat4 projection;



void main()
{
gl_Position = projection * view * vec4(aPos,1.0);

TexCoords = aTexCoord;
Normal = aNormal;
Pos = aPos;

}