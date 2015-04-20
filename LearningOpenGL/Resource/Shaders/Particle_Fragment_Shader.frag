#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec4 vertexColor;
uniform sampler2D u_colorTexture;
uniform vec4 u_color;

void main()
{
	outputColor = texture(u_colorTexture, colorCoord) * vertexColor * u_color * 2.0;
}