#version 330
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 4) in vec3 a_normal;
layout(location = 6) in vec4 a_color;

out vec2 colorCoord;
out vec4 colorVertex;
out vec3 normal;
out vec3 fragPos;
out vec4 fragPosLightSpace;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 lightSpaceMatrix;

void main()
{
	fragPos = (modelMatrix * vec4(a_position, 1.0)).xyz;
	fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

	mat3 NormalMatrix = transpose(inverse(mat3(modelMatrix)));
	normal = normalize(NormalMatrix * a_normal);
	colorCoord = a_texCoord;
	colorVertex = a_color;

	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(a_position, 1.0);
}