#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec4 vertexColor;
uniform vec4 u_color;
uniform sampler2D u_colorTexture;

void main()
{
	outputColor = vec4(u_color.rgb, texture(u_colorTexture, colorCoord).r);
}