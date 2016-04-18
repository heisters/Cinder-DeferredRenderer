#include "Model.hpp"
#include <memory>

using namespace ci;
using namespace std;

Model::Model() :
mModelMatrix( mat4( 1.0f ) ),
mNormalMatrix( mat3( 1.0f ) )
{
}

Model& Model::modelMatrix( const mat4& m )
{
	mModelMatrix = m;
	return *this;
}

Model& Model::normalMatrix( const mat3& m )
{
	mNormalMatrix = m;
	return *this;
}

const mat4& Model::getModelMatrix() const
{
	return mModelMatrix;
}

const mat3& Model::getNormalMatrix() const
{
	return mNormalMatrix;
}

const mat4& Model::getModelViewMatrix() const
{
    return mModelViewMatrix;
}

void Model::setModelMatrix( const mat4& m )
{
	mModelMatrix = m;
}

void Model::setNormalMatrix( const mat3& m )
{
	mNormalMatrix = m;
}

void Model::setModelViewMatrix( const mat4& m )
{
    mModelViewMatrix = m;
}

void Model::setMatrices( const ci::mat4& modelMatrix, const ci::mat4& viewMatrix )
{
    setModelMatrix( modelMatrix );
    setModelViewMatrix( viewMatrix * modelMatrix );
    setNormalMatrix( glm::inverseTranspose( mat3( mModelViewMatrix ) ) );
}

InstancedModel::InstancedModel( const ci::gl::VboMeshRef & mesh, size_t n ) :
	mModels( n ),
	mMaterialId( 0 ),
	mMesh( mesh )
{
	geom::BufferLayout bufferLayout;
	size_t stride = sizeof( Model );
	bufferLayout.append( geom::Attrib::CUSTOM_0, 16, stride, 0, 1 );
	bufferLayout.append( geom::Attrib::CUSTOM_1, 9, stride, sizeof( mat4 ), 1 );
	bufferLayout.append( geom::Attrib::CUSTOM_2, 16, stride, sizeof( mat4 ) + sizeof( mat3 ), 1 );

	mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, size() * stride, data(), GL_DYNAMIC_DRAW );
	mMesh->appendVbo( bufferLayout, mVbo );

	static gl::TextureRef sBlankTex; // a 1x1 0,0,0,0 pixel
	static gl::TextureCubeMapRef sBlankCubeMap;
	static bool sBlankTexesInitialized = false;
	if ( ! sBlankTexesInitialized ) {
		sBlankTexesInitialized = true;

		unsigned char* data = new unsigned char[ 4 ]{};
		Surface8u img( data, 1, 1, 4, SurfaceChannelOrder::RGBA );
		ImageSourceRef cubeMap[ 6 ]{ img, img, img, img, img, img };

		sBlankTex = gl::Texture2d::create( img );
		sBlankCubeMap = gl::TextureCubeMap::create( cubeMap );

		delete[] data;
		data = NULL;
	}

	setTexture( sBlankTex );
	setTextureCubeMap( sBlankCubeMap );
}

InstancedModel::InstancedModel( const ci::geom::Source &geometry, size_t n ) :
	InstancedModel( gl::VboMesh::create( geometry ), n )
{
}
