#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec3 normal;

uniform sampler2D u_colorTexture;
uniform vec4 u_color;
uniform int u_enableLight;

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
const int MAX_DIRECTIONAL_LIGHT_COUNT = 1;
uniform DirLight u_AllDirLight[MAX_DIRECTIONAL_LIGHT_COUNT];

vec3 CalcDirLightContribution()
{
	vec3 outColor = vec3(0.0, 0.0, 0.0);
	vec3 baseColor = texture(u_Material.baseColorTex, colorCoord).xyz;
	for ( int i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i )
	{
		outColor += baseColor * u_AllDirLight[i].ambient;
		outColor += baseColor * u_AllDirLight[i].diffuse * max(dot(-u_AllDirLight[i].direction, normal), 0.0);
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
	}
	else
	{
		finalColor = baseColor.xyz;
	}

	outputColor = vec4(finalColor, baseColor.w);
}