#version 330
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 4) in vec3 a_normal;
layout(location = 5) in vec3 a_tangent;

out vec2 colorCoord;
out vec3 normal;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec3 tangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 lightSpaceMatrix;

void main()
{
	fragPos = (modelMatrix * vec4(a_position, 1.0)).xyz;
	fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
	normal = normalize((modelMatrix * vec4(a_normal, 0.0))).xyz;
	tangent = normalize((modelMatrix * vec4(a_tangent, 0.0))).xyz;
	colorCoord = a_texCoord;

	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(a_position, 1.0);
}