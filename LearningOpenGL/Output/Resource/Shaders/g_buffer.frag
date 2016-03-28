#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 colorCoord;
in vec4 colorVertex;
in vec3 normal;
in vec3 fragPos;
in vec4 fragPosLightSpace;
in vec3 tangent;

// - Material
struct Material
{
	sampler2D baseColorTex;
	float shininess;

	bool bHasNormalMap;
	sampler2D normalMapTex;
};
uniform Material u_Material;

uniform vec4 u_color;

void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(normal);
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = (texture(u_Material.baseColorTex, colorCoord) * u_color).rgb;
    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = 1.0;
}