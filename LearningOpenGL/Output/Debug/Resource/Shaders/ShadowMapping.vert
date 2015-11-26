#version 330
layout(location = 0) in vec3 a_position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(a_position, 1.0);
}