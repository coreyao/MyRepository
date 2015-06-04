#version 330
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_boneIndex;
layout(location = 3) in vec4 a_boneWeight;
layout(location = 4) in vec3 a_normal;

out vec2 colorCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

const int SKINNING_JOINT_COUNT = 60;
uniform vec4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];

void getPositionAndNormal(out vec4 pos, out vec4 normal)
{
	int matrixIndex = 0;
    vec4 matrixPalette1 = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 matrixPalette2 = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 matrixPalette3 = vec4(0.0, 0.0, 0.0, 0.0);

	if (a_boneWeight[0] == 0 && a_boneWeight[1] == 0 && a_boneWeight[2] == 0 && a_boneWeight[3] == 0)
	{
		return;
	}

    float blendWeight = a_boneWeight[0];
    if (blendWeight > 0.0)
	{
		matrixIndex = int (a_boneIndex[0]) * 3;
		matrixPalette1 = u_matrixPalette[matrixIndex] * blendWeight;
		matrixPalette2 = u_matrixPalette[matrixIndex + 1] * blendWeight;
		matrixPalette3 = u_matrixPalette[matrixIndex + 2] * blendWeight;
	}
    
    blendWeight = a_boneWeight[1];
    if (blendWeight > 0.0)
    {
        matrixIndex = int(a_boneIndex[1]) * 3;
        matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
        matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
        matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;
    }
    
    blendWeight = a_boneWeight[2];
    if (blendWeight > 0.0)
    {
        matrixIndex = int(a_boneIndex[2]) * 3;
        matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
        matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
        matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;
    }
    
    blendWeight = a_boneWeight[3];
    if (blendWeight > 0.0)
    {
        matrixIndex = int(a_boneIndex[3]) * 3;
        matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
        matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
        matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;
    }

    vec4 _skinnedposition;
    _skinnedposition.x = dot(a_position, matrixPalette1);
    _skinnedposition.y = dot(a_position, matrixPalette2);
    _skinnedposition.z = dot(a_position, matrixPalette3);
    _skinnedposition.w = a_position.w;

	vec4 _skinnednormal;
	normal = vec4(a_normal, 0.0);
    _skinnednormal.x = dot(normal, matrixPalette1);
    _skinnednormal.y = dot(normal, matrixPalette2);
    _skinnednormal.z = dot(normal, matrixPalette3);
    _skinnednormal.w = 0;
    
	pos = _skinnedposition;
	normal = _skinnednormal;
}

void main()
{
	vec4 localPos;
	vec4 localNormal;
	getPositionAndNormal(localPos, localNormal);

	fragPos = (modelMatrix * localPos).xyz;
	normal = normalize((modelMatrix * localNormal)).xyz;
	colorCoord = a_texCoord;

	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * localPos;
}