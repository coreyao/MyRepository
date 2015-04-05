#version 330
out vec4 outputColor;
in vec2 colorCoord;
int vec4 vertexColor;

void main()
{
	outputColor = vertexColor;
}