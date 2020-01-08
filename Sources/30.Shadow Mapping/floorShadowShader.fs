#version 330 core
out vec4 LightColor;

in vec2 TexCoord;
in vec3 Pos;
in vec4 ShadowPos;
in vec3 Normal;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform bool blinn;


float ShadowCalculation (vec4 shadowPos)
{
  vec3 projCoords = shadowPos.xyz / shadowPos.w;
  float shadow = 0.0;

  if(projCoords.z > 1.0)
  {
    shadow = 0;  
  }
  else
  {
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Pos);
 
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
  
    vec2 texelSize = 1.0 / textureSize(shadowMap,0);

    for( int x = -1; x <= 1; ++x)
   {
     for( int y = -1; y <= 1; ++y)
     {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
     }
   }
   shadow /= 9.0;
 }
  return shadow;
}

void main()
{	
	vec3 color = texture(diffuseTexture,TexCoord).rgb;
	vec3 norm = normalize(Normal);
	vec3 lightColor = vec3(1.0);

	vec3 lightDir = normalize(lightPos - Pos);
	vec3 viewDir = normalize(cameraPos - Pos);

	vec3 ambient = 0.25  * color;
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff  * lightColor;

	float spec = 0.0;
	vec3 reflec = reflect(-lightDir, norm);
	spec = pow(max(dot(viewDir,reflec),0.0), 64.0);	
	vec3 specular = spec * lightColor;

	float shadow = ShadowCalculation(ShadowPos);
	vec3 lighting = (ambient +(1.0 - shadow)*(diffuse + specular)) * color;
	LightColor = vec4(lighting,1.0);

}