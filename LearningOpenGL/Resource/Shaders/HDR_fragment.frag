#version 330
out vec4 outputColor;
in vec2 colorCoord;

uniform sampler2D u_colorTexture;

void main()
{
	outputColor = vec4(1.0, 1.0, 0, 1.0);//texture(u_colorTexture, colorCoord);
}