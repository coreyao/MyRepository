#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec3 normal;
in vec3 fragPos;
in vec3 tangent;

uniform sampler2D u_colorTexture;
uniform vec4 u_color;
uniform int u_enableLight;
uniform vec3 u_eyePos;

// - Material
struct Material
{
	sampler2D baseColorTex;
	float shininess;

	bool bHasNormalMap;
	sampler2D normalMapTex;
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

struct SpotLight
{
	vec3 position;
	vec3 direction;

	float innerCutoff;
	float outerCutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
const int MAX_SPOT_LIGHT_COUNT = 5;
uniform SpotLight u_AllSpotLight[MAX_SPOT_LIGHT_COUNT];

vec3 CalcDirLightContribution(vec3 n)
{
	vec3 outColor = vec3(0.0, 0.0, 0.0);
	vec3 baseColor = (texture(u_Material.baseColorTex, colorCoord) * u_color ).xyz;
	for ( int i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i )
	{
		vec3 lightDir = -normalize(u_AllDirLight[i].direction);

		outColor += baseColor * u_AllDirLight[i].ambient;
		outColor += baseColor * u_AllDirLight[i].diffuse * max(dot(lightDir, normalize(normal)), 0.0);

		vec3 halfDir = normalize(lightDir + normalize(u_eyePos - fragPos));
		float spec = max(dot(halfDir, normalize(n)), 0.0);
		outColor += baseColor * u_AllDirLight[i].specular * pow(spec, u_Material.shininess);
	}

	return outColor;
}

vec3 CalcPointLightContribution(vec3 n)
{
	vec3 outColor = vec3(0.0, 0.0, 0.0);
	vec3 baseColor = (texture(u_Material.baseColorTex, colorCoord) * u_color ).xyz;
	
	for ( int i = 0; i < MAX_POINT_LIGHT_COUNT; ++i )
	{
		float distance = length(u_AllPointLight[i].position - fragPos);
		float attenuation = 1.0f / (u_AllPointLight[i].constant + u_AllPointLight[i].linear * distance + u_AllPointLight[i].quadratic * (distance * distance));    

		vec3 lightDir = normalize( u_AllPointLight[i].position - fragPos);

		outColor += baseColor * u_AllPointLight[i].ambient * attenuation;
		outColor += baseColor * u_AllPointLight[i].diffuse * max(dot(lightDir, normalize(n)), 0.0) * attenuation;

		vec3 halfDir = normalize(lightDir + normalize(u_eyePos - fragPos));
		float spec = max(dot(halfDir, normalize(n)), 0.0);
		outColor += baseColor * u_AllPointLight[i].specular * pow(spec, u_Material.shininess) * attenuation;
	}

	return outColor;
}

vec3 CalcSpotLightContribution(vec3 n)
{
	vec3 outColor = vec3(0.0, 0.0, 0.0);
	vec3 baseColor = (texture(u_Material.baseColorTex, colorCoord) * u_color ).xyz;
	
	for ( int i = 0; i < MAX_SPOT_LIGHT_COUNT; ++i )
	{
		float distance = length(u_AllSpotLight[i].position - fragPos);
		float attenuation = 1.0f / (u_AllSpotLight[i].constant + u_AllSpotLight[i].linear * distance + u_AllSpotLight[i].quadratic * (distance * distance));    

		vec3 ambient = baseColor * u_AllSpotLight[i].ambient * attenuation;

		vec3 lightDir = normalize( u_AllSpotLight[i].position - fragPos);
		vec3 diffuse = baseColor * u_AllSpotLight[i].diffuse * max(dot(lightDir, normalize(n)), 0.0) * attenuation;

		vec3 halfDir = normalize(lightDir + normalize(u_eyePos - fragPos));
		float spec = max(dot(halfDir, normalize(n)), 0.0);
		vec3 specular = baseColor * u_AllSpotLight[i].specular * pow(spec, u_Material.shininess) * attenuation;

		float fTheta = acos( dot(lightDir, normalize(-u_AllSpotLight[i].direction)) );
		float epsilon = u_AllSpotLight[i].innerCutoff - u_AllSpotLight[i].outerCutoff;
		float intensity = clamp( (fTheta - u_AllSpotLight[i].outerCutoff) / epsilon, 0.0, 1.0 );
		diffuse *= intensity;
		specular *= intensity;

		outColor += ambient;
		outColor += diffuse;
		outColor += specular;
	}

	return outColor;
}

void main()
{
	vec4 baseColor = texture(u_Material.baseColorTex, colorCoord) * u_color;
	vec3 finalColor = vec3(0.0, 0.0, 0.0);
	if ( u_enableLight > 0 )
	{
		vec3 n = normal;
		if (u_Material.bHasNormalMap)
		{
			vec3 T = normalize(tangent);
			vec3 N = normalize(normal);
			vec3 B = normalize(cross(N, T));
			mat3 TBN = mat3(T, B, N);

			n = texture(u_Material.normalMapTex, colorCoord).xyz;
			n = n * 2.0 - 1.0;
			n = normalize(TBN * n);
		}

		finalColor += CalcDirLightContribution(n);
		finalColor += CalcPointLightContribution(n);
		finalColor += CalcSpotLightContribution(n);
	}
	else
	{
		finalColor = baseColor.xyz;
	}

	outputColor = vec4(finalColor, baseColor.w);

	// - gamma correction
	float gamma = 2.2;
    outputColor.rgb = pow(outputColor.rgb, vec3(1.0/gamma));
}