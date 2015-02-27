#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 boneIndex;
layout(location = 3) in vec4 boneWeight;

out vec2 colorCoord;

uniform mat4 perspectiveMatrix;
uniform mat4 modelViewMatrix;

const int SKINNING_JOINT_COUNT = 60;
uniform vec4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];

vec4 getPosition()
{
	int matrixIndex = 0;
    vec4 matrixPalette1;
    vec4 matrixPalette2;
    vec4 matrixPalette3;

    float blendWeight = boneWeight[0];
    if (blendWeight > 0.0)
	{
		matrixIndex = int (boneIndex[0]) * 3;
		matrixPalette1 = u_matrixPalette[matrixIndex] * blendWeight;
		matrixPalette2 = u_matrixPalette[matrixIndex + 1] * blendWeight;
		matrixPalette3 = u_matrixPalette[matrixIndex + 2] * blendWeight;
	}
    
    blendWeight = boneWeight[1];
    if (blendWeight > 0.0)
    {
        matrixIndex = int(boneIndex[1]) * 3;
        matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
        matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
        matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;
    }
    
    
    blendWeight = boneWeight[2];
    if (blendWeight > 0.0)
    {
        matrixIndex = int(boneIndex[2]) * 3;
        matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
        matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
        matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;
    }
    
    blendWeight = boneWeight[3];
    if (blendWeight > 0.0)
    {
        matrixIndex = int(boneIndex[3]) * 3;
        matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
        matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
        matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;
    }

    vec4 _skinnedPosition;
    vec4 pos = vec4(position, 1.0);
    _skinnedPosition.x = dot(pos, matrixPalette1);
    _skinnedPosition.y = dot(pos, matrixPalette2);
    _skinnedPosition.z = dot(pos, matrixPalette3);
    _skinnedPosition.w = pos.w;
    
    return _skinnedPosition;
}

void main()
{
	gl_Position = perspectiveMatrix * modelViewMatrix * getPosition();
	colorCoord = texCoord;
}