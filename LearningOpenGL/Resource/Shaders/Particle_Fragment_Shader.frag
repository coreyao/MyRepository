#version 330
out vec4 outputColor;
in vec2 colorCoord;
in vec4 vertexColor;
uniform sampler2D u_colorTexture;
uniform vec4 u_color;
uniform vec4 u_UVAnim;

void main()
{
	vec2 finalCoord = colorCoord;
	if ( u_UVAnim.z > 0 && u_UVAnim.w > 0 )
	{
		finalCoord.x = u_UVAnim.y / u_UVAnim.w + finalCoord.x / u_UVAnim.w;
		finalCoord.y = u_UVAnim.x / u_UVAnim.z + finalCoord.y / u_UVAnim.z;
	}
	outputColor = texture(u_colorTexture, finalCoord) * vertexColor * u_color * 2.0;
}