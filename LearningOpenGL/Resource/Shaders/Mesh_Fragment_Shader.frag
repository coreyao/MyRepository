#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec3 normal;
in vec3 fragPos;
in vec3 tangent;
in vec4 fragPosLightSpace;
in vec4 colorVertex;

uniform sampler2D u_shadowMapTexture;
uniform vec4 u_color;
uniform int u_enableLight;
uniform vec3 u_eyePos;

uniform float u_fGamma;
uniform bool u_enableGammaCorrection; 

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
const int MAX_DIRECTIONAL_LIGHT_COUNT = 3;
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
const int MAX_POINT_LIGHT_COUNT = 3;
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
const int MAX_SPOT_LIGHT_COUNT = 3;
uniform SpotLight u_AllSpotLight[MAX_SPOT_LIGHT_COUNT];

float CalcInShadow(vec3 lightDir)
{
	return 0.0;

	vec3 projCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//if ( projCoord.x < -1 || projCoord.x > 1 || projCoord.y < -1 || projCoord.y > 1 || projCoord.z < -1 || projCoord.z > 1 )
		//return 1.0;

	projCoord = projCoord * 0.5 + 0.5;

	float bias = max( (1.0 - dot( normalize(normal), lightDir )) * 0.05, 0.005 );
	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMapTexture, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMapTexture, projCoord.xy + vec2(x, y) * texelSize).r; 
            shadow += projCoord.z - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

	if(projCoord.z > 1.0)
       shadow = 0.0;

	return shadow;

	/*float clostDepth = texture(u_shadowMapTexture, projCoord.xy).r;
	if ( clostDepth < projCoord.z - bias )
		return 1.0;
	else
		return 0.0;*/
}

vec3 CalcDirLightContribution(vec3 n)
{
	vec3 outColor = vec3(0.0, 0.0, 0.0);
	vec3 baseColor = (texture(u_Material.baseColorTex, colorCoord) * u_color ).xyz;
	for ( int i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i )
	{
		vec3 lightDir = normalize(u_AllDirLight[i].direction);
		float IsInShadow = CalcInShadow(lightDir);

		outColor += baseColor * u_AllDirLight[i].ambient;
		outColor += baseColor * u_AllDirLight[i].diffuse * max(dot(lightDir, n), 0.0) * (1.0 - IsInShadow);

		vec3 halfDir = normalize(lightDir + normalize(u_eyePos - fragPos));
		float spec = max(dot(halfDir, n), 0.0);
		outColor += baseColor * u_AllDirLight[i].specular * pow(spec, u_Material.shininess) * (1.0 - IsInShadow);
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
		outColor += baseColor * u_AllPointLight[i].diffuse * max(dot(lightDir, n), 0.0) * attenuation;

		vec3 halfDir = normalize(lightDir + normalize(u_eyePos - fragPos));
		float spec = max(dot(halfDir, n), 0.0);
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
		vec3 diffuse = baseColor * u_AllSpotLight[i].diffuse * max(dot(lightDir, n), 0.0) * attenuation;

		vec3 halfDir = normalize(lightDir + normalize(u_eyePos - fragPos));
		float spec = max(dot(halfDir, n), 0.0);
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
	vec4 baseColor = texture(u_Material.baseColorTex, colorCoord) * u_color * colorVertex;
	vec3 finalColor = vec3(0.0, 0.0, 0.0);
	if ( u_enableLight > 0 )
	{
		vec3 n = normalize(normal);
		if (u_Material.bHasNormalMap)
		{
			vec3 T = normalize(tangent);
			vec3 N = n;
			vec3 B = normalize(cross(T, N));
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

	if ( u_enableGammaCorrection )
		finalColor.rgb = pow( finalColor.rgb, vec3(1.0 / u_fGamma) );

	outputColor = vec4(finalColor, baseColor.w);
}