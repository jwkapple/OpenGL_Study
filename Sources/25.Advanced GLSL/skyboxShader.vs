#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

void main()
{
gl_Position = projection * view * vec4(20 * aPos,1.0);
gl_Position = gl_Position.xyww;

TexCoord = aPos;

}