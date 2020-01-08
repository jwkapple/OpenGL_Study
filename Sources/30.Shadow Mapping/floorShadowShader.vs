#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Pos;
out vec2 TexCoord;
out vec4 ShadowPos;
out vec3 Normal;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
 gl_Position = projection * view * model * vec4(aPos,1.0);
 Pos = vec3(model * vec4(aPos,1.0));
 TexCoord = aTexCoord;
 Normal = transpose(inverse(mat3(model))) * aNormal;

 ShadowPos = lightSpaceMatrix * vec4(Pos,1.0);
}