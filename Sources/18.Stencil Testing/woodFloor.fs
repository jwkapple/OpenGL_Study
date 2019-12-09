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
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform bool blinn;

void main()
{	

	vec3 color =  vec3(texture(material.texture_diffuse1,TexCoords));
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - Pos);
	vec3 viewDir = normalize(cameraPos - Pos);

	vec3 ambient = 0.05  * color;
	
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff  * color;
	float spec = 0.0;
	if(blinn)
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		spec = pow(max(dot(norm,halfwayDir),0.0), 32.0);	
	}
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
	vec3 specular = spec * vec3(0.3);
	LightColor = vec4((ambient + diffuse + specular),1.0);

}