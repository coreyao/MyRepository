#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec4 vertexColor;
uniform sampler2D colorTexture;

void main()
{
	outputColor = texture(colorTexture, colorCoord) * vertexColor;
}