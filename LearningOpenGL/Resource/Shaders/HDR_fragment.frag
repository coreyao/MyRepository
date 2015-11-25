#version 330
out vec4 outputColor;
in vec2 colorCoord;

uniform sampler2D u_colorTexture;
uniform float u_exposure;

void main()
{
	vec3 hdrColor = texture(u_colorTexture, colorCoord).rgb;
    // Reinhard tone mapping
    //vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
	vec3 mapped = vec3(1.0) - exp(-hdrColor * u_exposure);
	outputColor = vec4(mapped, 1.0);
}