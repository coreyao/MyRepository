#version 330
out vec4 outputColor;
in vec2 colorCoord;

uniform sampler2D u_colorTexture;

void main()
{
	vec3 hdrColor = texture(u_colorTexture, colorCoord).rgb;
	vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
	outputColor = vec4(hdrColor, 1.0); //vec4(texture(u_colorTexture, colorCoord).rgb, 1.0);
}