#version 330
layout(location = 0) out vec4 outputColor;
layout(location = 1) out vec4 outputBrightColor;

in vec2 colorCoord;
in vec3 normal;
in vec3 fragPos;
in vec3 tangent;
in vec4 fragPosLightSpace;
in vec4 colorVertex;

uniform vec4 u_color;

void main()
{
	outputColor = u_color;
	float brightness = dot(u_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        outputBrightColor = u_color;
}