#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
out vec2 colorCoord;
out vec4 vertexColor;

uniform mat4 perspectiveMatrix;
uniform mat4 modelViewMatrix;

void main()
{
	vec4 pos = perspectiveMatrix * modelViewMatrix * position;
	gl_Position = pos;
	colorCoord = texCoord;
	vertexColor = color;
}