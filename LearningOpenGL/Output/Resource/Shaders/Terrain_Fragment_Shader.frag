#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec4 vertexColor;

uniform sampler2D u_DetailTex0;
uniform sampler2D u_DetailTex1;
uniform sampler2D u_DetailTex2;
uniform sampler2D u_DetailTex3;
uniform float u_detailSize[4];

uniform sampler2D u_AlphaTex;

void main()
{
	vec4 alphaTexColor = texture(u_AlphaTex, colorCoord);
	vec4 color1 = texture(u_DetailTex0, colorCoord * u_detailSize[0]) * alphaTexColor.x;
	vec4 color2 = texture(u_DetailTex1, colorCoord * u_detailSize[1]) * alphaTexColor.y;
	vec4 color3 = texture(u_DetailTex2, colorCoord * u_detailSize[2]) * alphaTexColor.z;
	vec4 color4 = texture(u_DetailTex3, colorCoord * u_detailSize[3]) * ( 1.0 - alphaTexColor.x - alphaTexColor.y - alphaTexColor.z );

	outputColor = ( color1 + color2 + color3 + color4) * vertexColor;
}