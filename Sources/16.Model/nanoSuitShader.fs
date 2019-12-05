#version 330 core
out vec4 LightColor;

in vec2 TexCoords;

struct Material
{
 sampler2D texture_diffuse1;
};

uniform Material material;
void main()
{	
	vec3 tmp = vec3(texture(material.texture_diffuse1,TexCoords));
	LightColor = vec4((tmp),1.0);
}