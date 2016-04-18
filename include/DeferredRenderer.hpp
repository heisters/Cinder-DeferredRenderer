#pragma once

#include "cinder/gl/gl.h"

#include "Light.hpp"
#include "Material.hpp"
#include "Model.hpp"

class DeferredRenderer;

typedef int32_t scene_object_id;
template< class object_t >
using scene_object_container = std::vector< object_t >;

// A SceneObject is like a fancy pointer that encapsulates the raw buffer data
// (object_t) and renderer metadata about how to handle the object.
template< class object_t >
class SceneObject {
public:
    SceneObject() {}
    SceneObject( scene_object_container< object_t >* container, scene_object_id id ) : mContainerPtr( container ), mId( id ), mMetadata( new metadata ) {}


    object_t&   operator ()() const { return mContainerPtr->at( mId ); }
    object_t*   operator ->() const { return &(*this)(); }
	object_t&	operator *() const { return ( *this )( ); }
                operator bool() const { return mContainerPtr != nullptr && mId != -1; }
                operator scene_object_id() const { return mId; }

	bool				isValid() { return mContainerPtr != nullptr;  }
    scene_object_id     getId() const { return mId; }
    bool                isVisible() const { return mMetadata->visible; }
    bool&               visible() { return mMetadata->visible; }

private:
    // Pointers and iterators to vector elements are not stable when the vector
    // is resized, but we need vector's element contiguity for creating UBOs.
    // So we hold a pointer to the container itself, and the index of the
    // element, both of which are stable across reallocations.
    scene_object_container< object_t >* mContainerPtr = nullptr;
    scene_object_id                     mId = -1;

    struct metadata {
        bool                            visible = true;
    };

    std::shared_ptr< metadata >         mMetadata = nullptr;

};


class ScopedInstancedModelMap {
public:
    ScopedInstancedModelMap( SceneObject< InstancedModel > &model, GLenum access = GL_READ_WRITE ) :
    mModel( model ),
    mVboPtr( (Model*)mModel->getVbo()->map( access ) ),
	mVboEndPtr( mVboPtr + size() )
    { }

    ScopedInstancedModelMap( const ScopedInstancedModelMap& ) = delete;
    ScopedInstancedModelMap& operator=( const ScopedInstancedModelMap& ) = delete;

    ~ScopedInstancedModelMap()
    {
        mModel->getVbo()->unmap();
    }

	Model*	operator*() const { return mVboPtr; }
    Model*	operator->() const { return mVboPtr; }
    Model*	operator++() { return ++mVboPtr; }
    Model*	operator++(int) { return mVboPtr++; }

	size_t	size() const { return mModel->size(); }
	bool	isValid() const { return mVboPtr < mVboEndPtr; }

private:
	SceneObject< InstancedModel >&  mModel;
	Model*                          mVboPtr;
	const Model*					mVboEndPtr;
};

template< class ubo_t >
class ScopedUboMap {
public:
	ScopedUboMap( ci::gl::UboRef ubo, GLenum access = GL_READ_WRITE ) :
		mUbo( ubo ),
		mUboPtr( (ubo_t*)ubo->map( access ) )
	{}

	ScopedUboMap( const ScopedUboMap& ) = delete;
	ScopedUboMap& operator=( const ScopedUboMap& ) = delete;

	~ScopedUboMap()
	{
		mUbo->unmap();
	}

	ubo_t* get() const { return mUboPtr; }
	ubo_t* operator->() const { return mUboPtr; }
	ubo_t* operator++() { return ++mUboPtr; }
	ubo_t* operator++( int ) { return mUboPtr++; }
	ubo_t* operator+=( int rhs ) { return mUboPtr += rhs; }
	ubo_t* operator-=( int rhs ) { return mUboPtr -= rhs; }

private:
	ci::gl::UboRef					mUbo;
	ubo_t*							mUboPtr;
};

template< class ubo_t >
ubo_t* operator+( const ScopedUboMap< ubo_t >& lhs, int rhs ) { return lhs.get() + rhs; }
template< class ubo_t >
ubo_t* operator-( const ScopedUboMap< ubo_t >& lhs, int rhs ) { return lhs.get() - rhs; }


class Scene
{
    friend class DeferredRenderer;

public:
	Scene() {};

	Scene( Scene const& ) = delete;
	Scene& operator=( Scene const& ) = delete;


	SceneObject< Light >            add( const Light &light );
	std::vector< SceneObject< Light > > add( const Light &light, bool castsRays );
    SceneObject< Material >         add( const Material &material );
    SceneObject< InstancedModel >   add( const InstancedModel &models );


    ci::CameraPersp                 getCamera() const { return mCamera; };
    ci::CameraPersp&                getCamera() { return mCamera; };
    void                            setCamera( const ci::CameraPersp &cam ) { mCamera = cam; }

	ci::gl::UboRef					getUboLight() { return mUboLight; }
	ci::gl::UboRef					getUboRayLight() { return mUboRayLight; }
	ci::gl::UboRef					getUboMaterial() { return mUboMaterial; }

private:
	scene_object_container< Light >             mLightData;
	scene_object_container< Light >             mRayLightData;
    scene_object_container< Material >          mMaterialData;
    scene_object_container< InstancedModel >    mInstancedModelData;

    std::vector< SceneObject< InstancedModel > > mInstancedModels;

    ci::CameraPersp                             mCamera;

	ci::gl::UboRef								mUboLight;
	ci::gl::UboRef								mUboRayLight;
	ci::gl::UboRef								mUboMaterial;
};


class DeferredRenderer
{
public:
    DeferredRenderer();
	DeferredRenderer( DeferredRenderer const& ) = delete;
	DeferredRenderer& operator=( DeferredRenderer const& ) = delete;

	void						draw( const ci::Rectf &rect );
    void						draw();
    void						resize( const ci::ivec2& windowSize );
    void						update();

    void						createBatches( const ci::ivec2& windowSize );

    Scene&                      scene() { return mScene; };
    const Scene&                scene() const { return mScene; };


    ci::CameraPersp&            shadowCamera() { return mShadowCamera; }
    const ci::CameraPersp&      shadowCamera() const { return mShadowCamera; }

	enum : int32_t
	{
		Ao_None,
		Ao_Hbao,
		Ao_Sao
	} typedef Ao;
private:
    Scene                       mScene;

    ci::CameraPersp				mShadowCamera;


    ci::gl::FboRef				mFboAo;
    ci::gl::FboRef				mFboAccum;
    ci::gl::FboRef				mFboCsz;
    ci::gl::FboRef				mFboGBuffer;
    ci::gl::FboRef				mFboPingPong;
    ci::gl::FboRef				mFboRayColor;
    ci::gl::FboRef				mFboRayDepth;
    ci::gl::FboRef				mFboShadowMap;

    ci::gl::Texture2dRef		mTextureFboAo[ 2 ];
    ci::gl::Texture2dRef		mTextureFboAccum[ 3 ];
    ci::gl::Texture2dRef		mTextureFboGBuffer[ 3 ];
    ci::gl::Texture2dRef		mTextureFboPingPong[ 2 ];
    ci::gl::Texture2dRef		mTextureFboRayColor[ 2 ];


    ci::gl::BatchRef			mBatchDebugRect;
    ci::gl::BatchRef			mBatchEmissiveRect;
    ci::gl::BatchRef			mBatchGBufferLightSourceSphere;
    ci::gl::BatchRef			mBatchLBufferLightCube;
    ci::gl::BatchRef			mBatchLBufferShadowRect;
    struct InstancedModelBatch {
        SceneObject< InstancedModel > obj;
        ci::gl::BatchRef              batch;
    };
    std::vector< InstancedModelBatch > mBatchGBuffers;

public:

	std::vector< InstancedModelBatch >& gBufferBatches() { return mBatchGBuffers; }
	ci::gl::FboRef				getFboShadowMap() { return mFboShadowMap; }

private:

    std::vector< InstancedModelBatch > mBatchShadowMaps;

    ci::gl::BatchRef			mBatchAoCompositeRect;
    ci::gl::BatchRef			mBatchHbaoAoRect;
    ci::gl::BatchRef			mBatchHbaoBlurRect;
    ci::gl::BatchRef			mBatchSaoAoRect;
    ci::gl::BatchRef			mBatchSaoBlurRect;
    ci::gl::BatchRef			mBatchSaoCszRect;

    ci::gl::BatchRef			mBatchBloomBlurRect;
    ci::gl::BatchRef			mBatchBloomCompositeRect;
    ci::gl::BatchRef			mBatchBloomHighpassRect;

    ci::gl::BatchRef			mBatchColorRect;
    ci::gl::BatchRef			mBatchDofRect;
    ci::gl::BatchRef			mBatchFogRect;
    ci::gl::BatchRef			mBatchFxaaRect;

    ci::gl::BatchRef			mBatchRayCompositeRect;
    ci::gl::BatchRef			mBatchRayOccludeRect;
	ci::gl::BatchRef			mBatchRayLightSphere;
	ci::gl::BatchRef			mBatchRayScatterRect;

    ci::gl::BatchRef			mBatchStockColorRect;
    ci::gl::BatchRef			mBatchStockTextureRect;
	ci::gl::BatchRef			mBatchStockColorSphere;


    void						setUniforms( const ci::ivec2 &windowSize );

    bool						mEnabledAoBlur = true;
    bool						mEnabledColor = true;
    bool						mEnabledBloom = true;
    bool						mEnabledDoF = true;
    bool						mEnabledFog = true;
    bool						mEnabledFxaa = true;
    bool						mEnabledRay = true;
    bool						mEnabledRayPrev = true;
    bool						mEnabledShadow = true;

    bool						mDrawAo = false;
    bool						mDrawDebug = false;
    bool						mDrawLightVolume = false;
    
    bool						mHighQuality = false;
    bool						mHighQualityPrev = false;
    
    Ao                          mAo = Ao_Sao;
    Ao                          mAoPrev = Ao_Sao;
    int32_t						mMipmapLevels = 5;
	ci::vec2					mOffset = ci::vec2( 0.f );

    int32_t                     mLightMaterialId;

	ci::ivec2                   mWindowSize;

	float						mLightAccumulation = 1.f;// 0.43f;
	float						mBloomAttenuation = 1.f;// 1.7f;
	float						mBloomScale = 1.f;// 0.012f;
	float						mFocalDepth = 1.f; // 1.f == use length of camera eye point vector

public:

    bool&                       enabledAoBlur()     { return mEnabledAoBlur; }
    bool&                       enabledColor()      { return mEnabledColor; }
    bool&                       enabledBloom()      { return mEnabledBloom; }
    bool&                       enabledDoF()        { return mEnabledDoF; }
    bool&                       enabledFog()        { return mEnabledFog; }
    bool&                       enabledFxaa()       { return mEnabledFxaa; }
    bool&                       enabledRay()        { return mEnabledRay; }
    bool&                       enabledRayPrev()    { return mEnabledRayPrev; }
    bool&                       enabledShadow()     { return mEnabledShadow; }

    bool&                       drawAo()            { return mDrawAo; }
    bool&                       drawDebug()         { return mDrawDebug; }
    bool&                       drawLightVolume()   { return mDrawLightVolume; }

    bool&                       highQuality()       { return mHighQuality; }

    Ao&                         ao()                { return mAo; }
    Ao&                         aoPrev()            { return mAoPrev; }

	float&						lightAccumulation() { return mLightAccumulation; }
	float&						bloomAttenuation() { return mBloomAttenuation; }
	float&						bloomScale() { return mBloomScale; }
	float&						focalDepth() { return mFocalDepth; }
};