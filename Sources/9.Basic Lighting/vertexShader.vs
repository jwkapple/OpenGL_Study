#version 330 core

layout (location = 0) in vec3 objectPos;

out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 translate;
uniform mat4 rotation;

uniform vec3 objectColor;
uniform vec3 lightColor;
void main()
{
	float ambientStrength = 0.2f;
	vec3 ambient = ambientStrength * lightColor;
	
	gl_Position = projection * view * model * translate * rotation * vec4(objectPos,1.0f);
	
	vec3 result = ambient * objectColor;
	FragColor = vec4(result,1.0f);

}