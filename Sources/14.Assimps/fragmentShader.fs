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
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 color;
};

in vec3 FragNormal;
in vec3 FragPos;
in vec2 FragTexCoord;

uniform Material material;
uniform Light uLight;
uniform Light uLightBox;
uniform Light uSunLight;

vec3 CalcDirLight(Light light, Light uLight, vec3 normal,vec3 FragPos);  
vec3 CalcPointLight(Light light, vec3 normal,vec3 FragPos);
vec3 CalcSpotLight(Light light, vec3 normal,vec3 FragPos);

void main()
{

	vec3 norm = normalize(FragNormal);

	// ===================================
	// Viewer Light
	// ===================================
	
	vec3 viewer = CalcSpotLight(uLight, norm, FragPos); 

	// ===================================
	// Sun Box Light (Point Light)
	// ===================================

	vec3 pointLight = CalcPointLight(uSunLight,norm,FragPos);	

	// ===================================
	// Moving Box Light 
	// ===================================

	vec3 directLight = CalcDirLight(uLightBox, uLight, norm, FragPos);




	FragColor = vec4((viewer + pointLight + directLight),1.0);
	
}

vec3 CalcDirLight(Light light,Light uLight, vec3 normal,vec3 FragPos)
{
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 viewDir = normalize(uLight.position - FragPos);

	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse,FragTexCoord));

	vec3 ambient = vec3(0.0);

	vec3 reflec = reflect(-normal,lightDir);

	float result = pow(max(dot(reflec,viewDir),0.0),material.shininess);
	
	vec3 specular = result * vec3(texture(material.specular,FragTexCoord));
	
	vec3 directLight = (diffuse + ambient + specular) * light.color;

	return directLight;
}

vec3 CalcPointLight(Light light, vec3 normal,vec3 FragPos)
{
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 lightDir = normalize(light.position - FragPos);

	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse,FragTexCoord)) * attenuation;

	vec3 ambient = vec3(1.0) * attenuation;

	vec3 viewDir = normalize(uLight.position - FragPos);
	vec3 reflec = reflect(-normal,lightDir);
	float result = pow(max(dot(reflec,viewDir),0.0),material.shininess);

	vec3 specular = result * vec3(texture(material.specular,FragTexCoord)) * attenuation;

	vec3 pointLight = (diffuse + ambient + specular) * light.color;
	
	return pointLight;

}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 FragPos )
{
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(lightDir,normal),0.0);
	
	vec3 diffuse = vec3(0.0);
	vec3 ambient = vec3(0.0);
	vec3 specular = vec3(0.0);

	float theta = dot(lightDir,normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff)/epsilon,0.0,1.0);
	
	diffuse = light.diffuse * diff * vec3(texture(material.diffuse,FragTexCoord));
	specular = light.specular * diff * vec3(texture(material.specular,FragTexCoord));

	diffuse *= intensity;
	specular *= intensity;

	ambient = light.ambient * vec3(texture(material.diffuse,FragTexCoord));

	vec3 spotLight = (diffuse + ambient + specular) * light.color;

	return spotLight;
}