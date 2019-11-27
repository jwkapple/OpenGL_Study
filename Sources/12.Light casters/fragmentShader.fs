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

struct LightBox
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	vec3 color;
};

in vec3 FragNormal;
in vec3 FragPos;
in vec2 FragTexCoord;

uniform Material material;
uniform Light light;
uniform LightBox lightBox;
uniform Light sunLight;


void main()
{
	// ===================================
	// Viewer Light
	// ===================================
	
	vec3 norm = normalize(FragNormal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(lightDir,norm),0.0);
	
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

	// ===================================
	// Sun Box Light (Point Light)
	// ===================================

	float distance = length(sunLight.position - FragPos);
	float attenuation = 1.0 / (sunLight.constant + sunLight.linear * distance + sunLight.quadratic * (distance * distance));

	vec3 sunLightDir = normalize(sunLight.position - FragPos);
	float sunDiff = max(dot(sunLightDir,norm),0.0);
	
	vec3 sunDiffuse = sunDiff * vec3(texture(material.diffuse,FragTexCoord)) * attenuation;

	vec3 sunAmbient = vec3(1.0) * attenuation;

	vec3 viewDir = normalize(light.position - FragPos);
	vec3 sunReflec = reflect(-norm,sunLightDir);
	float sunResult = pow(max(dot(sunReflec,viewDir),0.0),material.shininess);

	vec3 sunSpecular = sunResult * vec3(texture(material.specular,FragTexCoord)) * attenuation;
	
	// ===================================
	// Moving Box Light 
	// ===================================

	vec3 boxLightDir = normalize(lightBox.position - FragPos);
	float boxDiff = max(dot(boxLightDir,norm),0.0);
	
	vec3 boxDiffuse = boxDiff * vec3(texture(material.diffuse,FragTexCoord));

	vec3 boxAmbient = vec3(0.0);

	vec3 boxReflec = reflect(-norm,boxLightDir);
	float boxResult = pow(max(dot(boxReflec,viewDir),0.0),material.shininess);
	
	vec3 boxSpecular = boxResult * vec3(texture(material.specular,FragTexCoord));
	
	vec4 enviorment = mix(vec4((sunDiffuse + sunAmbient + sunSpecular)* sunLight.color,1.0),vec4((boxDiffuse + boxAmbient + boxSpecular)* lightBox.color,1.0) , 0.5);
	vec4 viewer = vec4((diffuse + ambient + specular),1.0);

	FragColor =  mix(enviorment,viewer,0.5);
	
}