#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D u_colorTexture;
uniform vec4 u_color;
uniform int u_enableLight;
uniform vec3 u_eyePos;

// - Material
struct Material
{
	sampler2D baseColorTex;
	float shininess;
};
uniform Material u_Material;

// - Lighting
struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
const int MAX_DIRECTIONAL_LIGHT_COUNT = 5;
uniform DirLight u_AllDirLight[MAX_DIRECTIONAL_LIGHT_COUNT];

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
const int MAX_POINT_LIGHT_COUNT = 5;
uniform PointLight u_AllPointLight[MAX_POINT_LIGHT_COUNT];

vec3 CalcDirLightContribution()
{
	vec3 outColor = vec3(0.0, 0.0, 0.0);
	vec3 baseColor = texture(u_Material.baseColorTex, colorCoord).xyz;
	for ( int i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i )
	{
		vec3 lightDir = -normalize(u_AllDirLight[i].direction);

		outColor += baseColor * u_AllDirLight[i].ambient;
		outColor += baseColor * u_AllDirLight[i].diffuse * max(dot(lightDir, normalize(normal)), 0.0);

		float spec = max(dot(normalize(reflect(lightDir, normal)), normalize(u_eyePos - fragPos)), 0.0);
		outColor += baseColor * u_AllDirLight[i].specular * pow(spec, u_Material.shininess);
	}

	return outColor;
}

vec3 CalcPointLightContribution()
{
	vec3 outColor = vec3(0.0, 0.0, 0.0);
	vec3 baseColor = texture(u_Material.baseColorTex, colorCoord).xyz;
	
	for ( int i = 0; i < MAX_POINT_LIGHT_COUNT; ++i )
	{
		float distance = length(u_AllPointLight[i].position - fragPos);
		float attenuation = 1.0f / (u_AllPointLight[i].constant + u_AllPointLight[i].linear * distance + u_AllPointLight[i].quadratic * (distance * distance));    

		vec3 lightDir = normalize( u_AllPointLight[i].position - fragPos);

		outColor += baseColor * u_AllPointLight[i].ambient * attenuation;
		outColor += baseColor * u_AllPointLight[i].diffuse * max(dot(lightDir, normalize(normal)), 0.0) * attenuation;

		float spec = max(dot(normalize(reflect(lightDir, normal)), normalize(u_eyePos - fragPos)), 0.0);
		outColor += baseColor * u_AllPointLight[i].specular * pow(spec, u_Material.shininess) * attenuation;
	}

	return outColor;
}

void main()
{
	vec4 baseColor = texture(u_Material.baseColorTex, colorCoord) * u_color;
	vec3 finalColor = vec3(0.0, 0.0, 0.0);
	if ( u_enableLight > 0 )
	{
		finalColor += CalcDirLightContribution();
		finalColor += CalcPointLightContribution();
	}
	else
	{
		finalColor = baseColor.xyz;
	}

	outputColor = vec4(finalColor, baseColor.w);
}