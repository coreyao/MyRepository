#version 330
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 4) in vec3 a_normal;

out vec2 colorCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
	fragPos = (modelMatrix * a_position).xyz;
	normal = normalize((modelMatrix * vec4(a_normal, 0.0))).xyz;
	colorCoord = a_texCoord;

	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * a_position;
}