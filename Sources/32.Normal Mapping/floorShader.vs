#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out mat3 TBN;
out vec3 Pos;
out vec2 TexCoords;

layout (std140) uniform Matrices
{
 mat4 projection;
 mat4 view;
};

uniform mat4 model;

void main()
{
gl_Position = projection * view * model * vec4(aPos,1.0);

TexCoords = aTexCoord;
Pos = vec3(model * vec4(aPos, 1.0));

	vec3 T = normalize(vec3(model * vec4(aTangent, 0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0)));

TBN = mat3(T, B, N);

}