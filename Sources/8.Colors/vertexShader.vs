#version 330 core

layout (location = 0) in vec3 objectPos;

out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 translate;
uniform mat4 rotation;

uniform vec3 objectColor;

void main()
{
gl_Position = projection * view * model * translate * rotation * vec4(objectPos,1.0f);

FragColor = vec4(objectColor,1.0f);

}