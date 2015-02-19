#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
out vec2 colorCoord;
void main()
{
	gl_Position = position;
	colorCoord = texCoord;
}