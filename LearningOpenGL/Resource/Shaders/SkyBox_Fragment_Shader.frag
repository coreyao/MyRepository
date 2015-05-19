#version 330
out vec4 outputColor;

in vec3 colorCoord;

uniform samplerCube u_colorTexture;

void main()
{
	vec4 color = texture(u_colorTexture, normalize(colorCoord));
	//outputColor = vec4(color.rbg, 1.0);
	outputColor = vec4(normalize(colorCoord), 1.0);
}