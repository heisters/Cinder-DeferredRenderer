#include "../common/vertex_in.glsl"
#include "../common/offset.glsl"
#include "../common/light.glsl"

// Volumetric light scattering: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch13.html

const float	kDecay		= 1.0;
const float	kDensity	= 1.0;
const float	kExposure	= 0.002;
const int	kNumSamples	= 100;
const float kWeight		= 5.65;
const int	kMaxLights	= 5;

uniform mat4		uLightMatrix;
uniform sampler2D	uSampler;

layout (location = 0) out vec4 oColor;

vec2 lightPosition( Light light )
{
	vec4 p = uLightMatrix * vec4( light.position, 1.0 );
	p = p / p.w * 0.5 + 0.5;
	return p.xy;
}

vec4 sampleLight( vec2 uv, float decay )
{
	return texture( uSampler, uv ) * decay * kWeight;
}

void main( void )
{
	oColor			= vec4( 0.0 );
	vec2 uv			= calcTexCoordFromUv( vertex.uv );

	for ( int li = 0; li < kMaxLights && li < NUM_LIGHTS; ++li ) {
		Light light = uLights[ li ];

		vec2 p		= lightPosition( light );
		vec2 d		= uv - p;

		d			*= 1.0 / float( kNumSamples ) * kDensity;
		float decay	= 1.0;

		vec2 uvd = uv;
		for ( int i = 0; i < kNumSamples; ++i ) {
			uvd		-= d;
			oColor	+= sampleLight( uvd, decay );
			decay	*= kDecay;
		}
	}

	oColor		= oColor * kExposure;
}
 