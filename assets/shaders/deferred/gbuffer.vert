#if defined( INSTANCED_LIGHT_SOURCE )
#include "../common/light.glsl"
#endif

uniform mat4	ciModelViewProjection;
#if !defined( INSTANCED_MODEL )
uniform mat3	ciNormalMatrix;
uniform mat4    ciModelViewMatrix;
#endif
uniform mat4    uTextureMatrix;
uniform mat4    ciViewMatrix;
uniform mat4	ciViewMatrixInverse;

in vec4 	ciPosition;
in vec3 	ciNormal;
in vec4 	ciColor;
in vec2     ciTexCoord0;
#if defined( INSTANCED_MODEL )
in mat3		vInstanceNormalMatrix;
in mat4		vInstanceModelMatrix;
in mat4		vInstanceModelViewMatrix;
#endif

out Vertex
{
	vec3 color;
	vec3 normal;
    vec2 uv;
    vec3 NormalWorldSpace;
    vec3 EyeDirWorldSpace;
} vertex;

void main( void )
{
	vertex.color		= ciColor.rgb;
    vertex.uv           = (uTextureMatrix * vec4( ciTexCoord0, 0.0, 0.0 )).st;

#if defined( INSTANCED_MODEL )
	mat3 normalMatrix	= vInstanceNormalMatrix;
    mat4 modelViewMatrix= vInstanceModelViewMatrix;
#else
	mat3 normalMatrix	= ciNormalMatrix;
    mat4 modelViewMatrix= ciModelViewMatrix;
#endif
	vec3 n				= ciNormal;
#if defined( INVERT_NORMAL )
	n					= -n;
#endif
	vertex.normal 		= normalMatrix * n;

	vec4 p				= ciPosition;
#if defined( INSTANCED_LIGHT_SOURCE )
	Light light			= uLights[ gl_InstanceID ];
	p.xyz				*= light.radius;
	p.xyz				+= light.position;
	p.w					= 1.0;
	vertex.color		*= light.diffuse.rgb;
#endif
	
#if defined( INSTANCED_MODEL )
	p					= vInstanceModelMatrix * p;
#endif
    
    vec4 positionViewSpace = modelViewMatrix * p;
    vec4 eyeDirViewSpace = positionViewSpace - vec4( 0, 0, 0, 1 ); // eye is always at 0,0,0 in view space
    vertex.EyeDirWorldSpace = vec3( ciViewMatrixInverse * eyeDirViewSpace );
    vertex.NormalWorldSpace = normalize( vec3( vec4( vertex.normal, 0 ) * ciViewMatrix ) );


	gl_Position			= ciModelViewProjection * p;
}
 