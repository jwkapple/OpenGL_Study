#version 330 core
out vec4 LightColor;

in vec3 Normal;
in vec2 TexCoords;
in vec3 Pos;


struct Material
{
 sampler2D texture_diffuse1;
};

uniform Material material;
uniform vec3 cameraPos;

void main()
{	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize ( cameraPos - Pos);

	float diff = max(dot(viewDir,-norm),0.0);

	vec3 diffuse =  diff * vec3(texture(material.texture_diffuse1,TexCoords));


	LightColor = vec4(diffuse,1.0);
}