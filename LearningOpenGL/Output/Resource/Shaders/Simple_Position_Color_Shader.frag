#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 fragPosLightSpace;
in vec4 colorVertex;

uniform sampler2D u_lightTexture;

void main()
{
	vec4 baseColor = texture(u_Material.baseColorTex, colorCoord) * colorVertex;
	vec3 finalColor = vec3(0.0, 0.0, 0.0);
	finalColor = baseColor.xyz;
	outputColor = vec4(finalColor.rgb, baseColor.w);
}