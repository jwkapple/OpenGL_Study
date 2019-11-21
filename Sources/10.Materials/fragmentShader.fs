#version 330 core
out vec4	FragColor;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;	
	float shininess;
};

struct Light
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragNormal;
in vec3 FragPos;

uniform Material material;
uniform Light light;

uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform vec3 objectColor;
uniform vec3 lightColor;


void main()
{
	vec3 norm = normalize(FragNormal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	vec3 ambient = light.ambient * material.ambient;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflec = reflect(norm,-lightDir);
	float result = pow(max(dot(reflec,viewDir),0.0),material.shininess);
	vec3 specular = light.specular * (result * material.specular);

	FragColor = vec4((ambient + diffuse + specular) ,1.0);
	
}