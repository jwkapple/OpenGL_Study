#version 330 core
out vec4 LightColor;

in vec2 TexCoord;
in vec3 Pos;
in vec3 Normal;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform float far_plane;

float closestDepth;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float ShadowCalculation (vec3 fragPos)
{
  vec3 fragToLight = fragPos - lightPos; 

  float shadow = 0.0;
  float bias = 0.06;
  float currentDepth = length(fragToLight);
  float diskRadius = (0.5 + (currentDepth / far_plane)) / 25.0;
  int samples = 20;

  for( int i = 0; i < samples; ++i)
  {
     float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
     closestDepth *= far_plane;
     if(currentDepth - bias > closestDepth)
	shadow += 1.0;
  }

  shadow /= samples;

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

	float shadow = ShadowCalculation(Pos);
	vec3 lighting = (ambient +(1-shadow)*(diffuse + specular)) * color;
	LightColor = vec4(lighting, 1.0);

}