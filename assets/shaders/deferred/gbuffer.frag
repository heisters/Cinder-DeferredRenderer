uniform int uMaterialId;
uniform sampler2D uTexture;
uniform samplerCube uCubeMap;

in Vertex
{
	vec3 color;
	vec3 normal;
    vec2 uv;
    vec3 NormalWorldSpace;
    vec3 EyeDirWorldSpace;
} vertex;

layout (location = 0) out vec4	oAlbedo;
layout (location = 1) out ivec4	oMaterial;
layout (location = 2) out vec4	oNormal;

// http://aras-p.info/texts/CompactNormalStorage.html#method04spheremap
vec2 pack( vec3 v )
{
	float f = sqrt( 8.0 * v.z + 8.0 );
	return v.xy / f + 0.5;
}

void main( void )
{
    vec3 reflectedEyeWorldSpace = reflect( vertex.EyeDirWorldSpace, normalize( vertex.NormalWorldSpace ) );
    vec4 diffuseColor   = vec4( vertex.color, 1.0 );
    vec4 cubeMapColor   = texture( uCubeMap, reflectedEyeWorldSpace );
    vec4 texColor       = texture( uTexture, vertex.uv );


    oAlbedo     = mix( mix( diffuseColor, cubeMapColor, cubeMapColor.a ), texColor, texColor.a );
	oMaterial	= ivec4( uMaterialId, 0, 0, 255 );
	oNormal		= vec4( pack( normalize( vertex.normal ) ), 0.0, 1.0 );
}
