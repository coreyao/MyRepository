#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec4 vertexColor;
uniform sampler2D u_colorTexture;

void main()
{
	vec4 colord = texture(u_colorTexture, colorCoord) * vertexColor;
	outputColor = colord;
}