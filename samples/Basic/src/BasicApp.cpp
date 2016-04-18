#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"
#include "cinder/Rand.h"

#include "DeferredRenderer.hpp"

#define M_TAU 6.28318530717958647692528676655900576839433879875021

class BasicApp : public ci::app::App {
public:
	BasicApp();

	void setup() override;
	void update() override;
	void draw() override;
	void resize() override;

	void keyDown( ci::app::KeyEvent event ) override;
	void mouseDown ( ci::app::MouseEvent event ) override;

private:
	DeferredRenderer					mRenderer;
	SceneObject< InstancedModel >		mCubes;
	SceneObject< Material >				mCubeMaterial;

	ci::CameraPersp						mCam;
};


using namespace ci;
using namespace ci::app;
using namespace std;

BasicApp::BasicApp()
{
	// Create materials

	mCubeMaterial = mRenderer.scene().add( Material()
										  .colorDiffuse( Colorf( 0.9f, 0.9f, 0.9f ) )
										  .shininess( 0.5f ) );

	// Create 8 cubes

	InstancedModel model( geom::Cube(), 8 );
	model.setMaterialId( mCubeMaterial.getId() );
	mCubes = mRenderer.scene().add( model );


	// Create lighting

	// Center light
	mRenderer.scene().add( Light()
						  .color( Colorf( 1.f, 1.f, 0.9f ) )
						  .intensity( 1.f )
						  .position( vec3() )
						  .volume( 3.f ) );

	// Outer lights
	for ( int i = 0; i < 40; ++i ) {
		float hue = randFloat();
		Colorf diffuse( CM_HSV, hue, 1.f, 0.8f );
		Colorf specular( CM_HSV, hue, 1.f, 1.f );
		vec3 p = randVec3() * 2.5f;
		mRenderer.scene().add( Light()
							  .colorDiffuse( diffuse )
							  .colorSpecular( specular )
							  .position( p )
							  .volume( 1.5f )
							  .intensity( 0.5f ) );
	}


	// Setup camera

	mCam = CameraPersp( getWindowWidth(), getWindowHeight(), 60.f );
	mCam.lookAt( vec3( 0.f, 0.f, 1.f ), vec3( 0.f, 0.f, 0.f ) );
	mRenderer.scene().setCamera( mCam );


	// Setup renderer

	mRenderer.enabledShadow() = false;
	mRenderer.highQuality() = true;

	mRenderer.createBatches( getWindowSize() );
}

void BasicApp::setup()
{
	resize();
}

void BasicApp::resize()
{
	mCam.setPerspective( 60.f, getWindowAspectRatio(), 0.01f, 10.f );
	mCam = mCam.calcFraming( Sphere( vec3(), 3.f ) );
	mCam.setFarClip( length( mCam.getEyePoint() ) + 4.f );

	mRenderer.scene().setCamera( mCam );
	mRenderer.resize( getWindowSize() );
}

void BasicApp::update()
{
	const static vec3 corners[ 8 ]{
		{ -1.f,	 1.f,  1.f }, { 1.f,  1.f,  1.f },
		{ -1.f,	-1.f,  1.f }, { 1.f, -1.f,  1.f },
		{ -1.f,  1.f, -1.f }, { 1.f,  1.f, -1.f },
		{ -1.f, -1.f, -1.f }, { 1.f, -1.f, -1.f }
	};

	double t = getElapsedSeconds();


	{
		ScopedInstancedModelMap vboMap( mCubes );
		int i = 0;
		while ( vboMap.isValid() ) {
			mat4 m = rotate( (float)( M_TAU * t ) * 0.1f, vec3( 1.f, 1.f, 0.f ) ) *
			translate( corners[ i ] );
			vboMap->setModelMatrix( m );

			i++;
			vboMap++;
		}
	}


	mRenderer.update();
}

void BasicApp::draw()
{
	mRenderer.draw();
}

void BasicApp::keyDown( KeyEvent event )
{
	static int debugLevel = 0;
	if ( event.getCode() == KeyEvent::KEY_BACKSLASH ) {
		debugLevel = ( debugLevel + 1 ) % 3;
		mRenderer.drawDebug() = debugLevel == 1;
		mRenderer.drawLightVolume() = debugLevel == 2;
	}
}

void BasicApp::mouseDown( MouseEvent event )
{
	
}

CINDER_APP( BasicApp, RendererGl( RendererGl::Options().version( 3, 3 ) ), []( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setHighDensityDisplayEnabled( false );
	settings->setWindowSize( 1280, 720 );
} );
