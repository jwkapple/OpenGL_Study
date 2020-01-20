#version 330 core
out vec4 LightColor;

in vec2 TexCoord;
in vec3 Pos;
in vec3 Normal;

struct Light {
   vec3 Position;
   vec3 Color;
};

uniform Light lights[16];
uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

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


void main()
{	
	vec3 color = texture(diffuseTexture,TexCoord).rgb;
	vec3 norm = normalize(Normal);
	
	vec3 lighting = vec3(0.0);

	vec3 ambient = 0.0 * color;

	for(int i = 0 ; i < 4; i++)
	{
		vec3 lightDir = normalize(lights[i].Position - Pos);
		float diff = max(dot(lightDir,norm), 0.0);
		vec3 diffuse = lights[i].Color * diff * color;
		vec3 result = diffuse;

		float distance = length(Pos - lights[i].Position);
		result *= 1.0 / (distance * distance);
		lighting += result;
	}
	
	vec3 light = (ambient + lighting);
	LightColor = vec4(light, 1.0);

}