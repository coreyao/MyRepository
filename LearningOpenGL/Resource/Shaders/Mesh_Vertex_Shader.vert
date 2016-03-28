#version 330
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 4) in vec3 a_normal;
layout(location = 5) in vec3 a_tangent;
layout(location = 6) in vec4 a_color;

out vec2 colorCoord;
out vec4 colorVertex;
out vec3 normal;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec4 fragPosProjectorSpace;
out vec3 tangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 ProjectorSpaceMatrix;
uniform bool u_flipnormal;

void main()
{
	fragPos = (modelMatrix * vec4(a_position, 1.0)).xyz;
	fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
	fragPosProjectorSpace = ProjectorSpaceMatrix * vec4(fragPos, 1.0);

	mat3 NormalMatrix = transpose(inverse(mat3(modelMatrix)));
	normal = normalize(NormalMatrix * a_normal);
	if ( u_flipnormal )
		normal = -normal;
	tangent = normalize((NormalMatrix * a_tangent));
	colorCoord = a_texCoord;
	colorVertex = a_color;

	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(a_position, 1.0);
}