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
uniform vec3 lightPos;

void main()
{	
	vec3 color = vec3(texture(material.texture_diffuse1,TexCoords));
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize (cameraPos - Pos);
	vec3 lightDir = normalize (lightPos - Pos);
	
	vec3 ambient = 0.3 * color;

	float diff = max(dot(lightDir,norm),0.0);
	vec3 diffuse =  diff * color;

	vec3 reflec = reflect(-lightDir,norm);
	float spec = pow(max(dot(reflec,viewDir),0.0),8.0);
	vec3 specular = spec * color;

	LightColor = vec4((ambient + diffuse + specular),1.0);
}