#version 330 core

out vec4	FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirectionalLight
{
	vec3 direction;
};

in vec3 FragNormal;
in vec3 FragPos;
in vec2 FragTexCoord;

uniform Material material;
uniform Light light;
uniform DirectionalLight directLight;


uniform vec3 lightPos;
uniform vec3 cameraPos;


void main()
{

	// Previous Light Setting :: 00 

	vec3 norm = normalize(FragNormal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(lightDir,FragNormal),0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse,FragTexCoord)) * light.diffuse;

	vec3 ambient = light.ambient * vec3(texture(material.diffuse,FragTexCoord));
	
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflec = reflect(norm,-lightDir);
	float result = pow(max(dot(reflec,viewDir),0.0),material.shininess);
	vec3 specular = light.specular * result * vec3(texture(material.specular,FragTexCoord));

	// Directional Light Setting :: 01
	
	vec3 directLightDir = normalize(-directLight.direction);
	float directDiff = max(dot(directLightDir,FragNormal),0.0);
	vec3 directDiffuse = directDiff * vec3(texture(material.diffuse,FragTexCoord)) * light.diffuse;


	vec3 directReflec = reflect(directLightDir,norm);
	float directResult = pow(max(dot(directReflec,viewDir),0.0),material.shininess);
	vec3 directSpecular = light.specular * directResult * vec3(texture(material.specular,FragTexCoord));

	vec4 color = vec4((ambient + directDiffuse + directSpecular),1.0);


	FragColor = color ;
	
}