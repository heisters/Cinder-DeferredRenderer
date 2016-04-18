#include "../common/light.glsl"
flat in int vInstanceId;
layout (location = 0) out vec4 oColor;

void main ( void )
{
	Light light			= uLights[ vInstanceId ];

	oColor = light.diffuse;
}