#version 330
out vec4 outputColor;
in vec2 colorCoord;

uniform sampler2D u_colorTexture;

void main()
{
	outputColor = texture(u_colorTexture, colorCoord);
}