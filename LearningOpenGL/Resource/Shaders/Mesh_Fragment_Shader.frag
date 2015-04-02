#version 330
out vec4 outputColor;
in vec2 colorCoord;
uniform sampler2D colorTexture;
uniform vec4 u_color;
void main()
{
	outputColor = texture(colorTexture, colorCoord) * u_color;
}