#include "../common/vertex_in.glsl"

uniform sampler2D uSampler0;
uniform sampler2D uSampler1;
uniform float     blend;

layout (location = 0) out vec4 oColor;

void main( void )
{
    vec4 color1 = texture( uSampler0, vertex.uv );
    vec4 color2 = texture( uSampler1, vertex.uv );
    oColor		= mix( color1, color2, blend );
}
