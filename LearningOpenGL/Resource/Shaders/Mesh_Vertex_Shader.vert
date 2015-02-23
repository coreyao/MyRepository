#version 330
layout(location = 0) in vec4 position;
//layout(location = 1) in vec2 texCoord;
out vec2 colorCoord;

uniform mat4 perspectiveMatrix;

void main()
{
	gl_Position = vec4(position.xyz, 1.0);
	//colorCoord = texCoord;
}