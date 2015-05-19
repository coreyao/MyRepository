#version 330
out vec4 outputColor;

in vec3 colorCoord;

uniform samplerCube u_colorTexture;

void main()
{
	vec4 color = texture(u_colorTexture, colorCoord);
	outputColor = vec4(color.rbg, 1.0);
}