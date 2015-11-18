#version 330
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;
out vec2 colorCoord;

void main()
{
	gl_Position = vec4(position, 1.0);
	colorCoord = texCoord;
}