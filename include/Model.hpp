#pragma once

#include "cinder/gl/gl.h"
#include "cinder/Matrix.h"
#include "cinder/GeomIo.h"

class Model
{
public:
    Model();

	Model&			modelMatrix( const ci::mat4& m );
	Model&			normalMatrix( const ci::mat3& m );

	const ci::mat4&	getModelMatrix() const;
	const ci::mat3&	getNormalMatrix() const;
    const ci::mat4&	getModelViewMatrix() const;

	void			setModelMatrix( const ci::mat4& m );
	void			setNormalMatrix( const ci::mat3& m );
    void			setModelViewMatrix( const ci::mat4& m );

    void            setMatrices( const ci::mat4& modelMatrix, const ci::mat4& viewMatrix );
protected:
	ci::mat4		mModelMatrix;
	ci::mat3		mNormalMatrix;
    ci::mat4        mModelViewMatrix;
};

class InstancedModel
{
public:
    typedef std::vector< Model > container_t;

	InstancedModel( const ci::gl::VboMeshRef &mesh, size_t n = 1 );
    InstancedModel( const ci::geom::Source &geometry, size_t n );

    size_t                              size() const { return mModels.size(); };
    Model*                              data() { return mModels.data(); };

    container_t::const_iterator         begin() const { return mModels.begin(); }
    container_t::const_iterator         end() const { return mModels.end(); }
    container_t::iterator               begin() { return mModels.begin(); }
    container_t::iterator               end() { return mModels.end(); }

    int                                 getMaterialId() const { return mMaterialId; };
    void                                setMaterialId( int mid ) { mMaterialId = mid; };

    ci::gl::Texture2dRef                getTexture() { return mTexture; }
    const ci::gl::Texture2dRef&         getTexture() const { return mTexture; }
    void                                setTexture( const ci::gl::Texture2dRef &t ) { mTexture = t; }
    bool                                hasTexture() const { return mTexture != nullptr; }

    ci::gl::TextureCubeMapRef           getTextureCubeMap() { return mTextureCubeMap; }
    const ci::gl::TextureCubeMapRef&    getTextureCubeMap() const { return mTextureCubeMap; }
    void                                setTextureCubeMap( const ci::gl::TextureCubeMapRef &t ) { mTextureCubeMap = t; }
    bool                                hasTextureCubeMap() const { return mTextureCubeMap != nullptr; }

    ci::mat4                            getTextureMatrix() const { return mTextureMtx; }
    void                                setTextureMatrix( const ci::mat4 &m ) { mTextureMtx = m; }

    ci::gl::GlslProgRef                 getShader() const { return mShader; };
    void                                setShader( const ci::gl::GlslProgRef &shader ) { mShader = shader; }
    bool                                hasShader() const { return mShader != nullptr; }
    
    ci::gl::VboMeshRef                  getMesh() const { return mMesh; };
    ci::gl::VboRef                      getVbo() const { return mVbo; };
    
private:
    int                         mMaterialId;
    container_t                 mModels;
    ci::gl::VboMeshRef          mMesh;
    ci::gl::VboRef              mVbo;
    ci::gl::Texture2dRef        mTexture = nullptr;
    ci::gl::TextureCubeMapRef   mTextureCubeMap = nullptr;
    ci::mat4                    mTextureMtx;
    ci::gl::GlslProgRef         mShader = nullptr;
};
 