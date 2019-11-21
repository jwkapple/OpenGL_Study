#version 330 core
out vec4	FragColor;


in vec3 FragNormal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform vec3 objectColor;
uniform vec3 lightColor;


void main()
{
	vec3 norm = normalize(FragNormal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff * lightColor;

	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;

	float specularStrength = 0.7;
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir,norm);
	
	float spec = pow(max(dot(viewDir,reflectDir),0.0),128);
	vec3 specular = spec * specularStrength * lightColor;

	vec3 result = (diffuse + ambient + specular) * objectColor;
	FragColor = vec4(result,1.0);
}