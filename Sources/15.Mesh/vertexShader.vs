#version 330 core

layout (location = 0) in vec3 objectPos;
layout (location = 1) in vec3 objectNormal;
layout (location = 2) in vec2 texCoords;

out vec3 FragNormal;
out vec3 FragPos;
out vec2 FragTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 translate;
uniform mat4 rotation;

void main()
{

	gl_Position = projection * view * model * translate * rotation * vec4(objectPos,1.0f);
	FragPos = vec3(model* vec4(objectPos,1.0));

	FragNormal = objectNormal;
	FragTexCoord = texCoords;
}