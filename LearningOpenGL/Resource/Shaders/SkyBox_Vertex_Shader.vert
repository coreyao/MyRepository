#version 330
layout(location = 0) in vec4 position;

uniform mat4 perspectiveMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

out vec3 colorCoord;

void main()
{
	colorCoord = normalize(normalMatrix * position.xyz);
	gl_Position = perspectiveMatrix * modelViewMatrix * position;
}