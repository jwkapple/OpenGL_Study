#version 330 core

layout (location = 0) in vec3 aPos;

out vec4 LightFragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 translate;
uniform mat4 rotation;

uniform vec3 lightColor;

void main()
{
gl_Position = projection * view * model * translate * rotation * vec4(aPos,1.0f);

LightFragColor = vec4(lightColor,1.0f);

}