// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved
//

#include <DDS.h>
#pragma comment( lib, "DirectXTex" )

#include <medx11/Texture.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToLock.h>
#include <qxml/Document.h>

// MS agressive macros.
#ifdef LoadImage
#undef LoadImage
#endif

using namespace medx11;
using namespace me;
using namespace render;

Texture::Texture( IRenderer * renderer, TextureParameters parameters )
	: m_renderer( dynamic_cast< const Renderer *>(renderer) )
	, m_useColorKey( false )
	, m_created( false )
	, m_parameters( parameters )
{
	Create();
}

Texture::~Texture()
{
	Destroy();
}

void Texture::Create()
{
	// Load from a file.
	if ( m_parameters.source.Empty() )
	{
		throw unify::Exception( "Not enough information to load Texture!" );
	}

	LoadHeader();

	LoadImage( m_parameters.source );

	m_created = true;
}

// Destroyes data, keeps header intact - thus no graphical footprint, yet we can still use it's statistics/dimensions in calculations.
void Texture::Destroy()
{
	m_texture = nullptr;
	m_created = false; // TODO: Can solve this from m_texture.
	m_scratch.Release();
}

const unsigned int Texture::FileWidth() const
{
	return m_fileSize.width;
}

const unsigned int Texture::FileHeight() const
{
	return m_fileSize.height;
}

const unify::Size< unsigned int > & Texture::ImageSize() const
{
	return m_imageSize;
}

bool Texture::GetRenderable() const
{
	return m_parameters.renderable;
}

bool Texture::GetLockable() const
{
	return m_parameters.lockable;
}

void Texture::LockRect( unsigned int level, TextureLock & lock, const unify::Rect< long > * rect, bool readonly )
{
	if ( !m_created )
	{
		Create();
	}

	if ( m_parameters.lockable == false )
	{
		throw exception::FailedToLock( "Texture is not lockable!" );
	}

	lock.pBits = m_scratch.GetImage( level, 0, 0 )->pixels;
	lock.uStride = m_scratch.GetImage( level, 0, 0 )->rowPitch;
	lock.bpp = 4;
	if ( rect )
	{
		lock.uWidth = rect->right - rect->left;
		lock.uHeight = rect->bottom - rect->top;
	}
	else
	{
		lock.uWidth = m_imageSize.width;
		lock.uHeight = m_imageSize.height;
	}
	lock.totalBytes = lock.uHeight * lock.uStride;
}

void Texture::UnlockRect( unsigned int level )
{
}

// Load all possible info (short of bits) about the texture
void Texture::Preload()
{
	// Load any new image header file if there
	LoadHeader();

	// Attempt to load the image for information
	Create();

	// Destroy our texture.
	Destroy();
}

void Texture::LoadHeader()
{
	unify::Path imageHeaderFilepath{ ChangeExtension( m_parameters.source, ".xml" ) };

	// Check if the file exists...
	if ( !imageHeaderFilepath.Exists() )
	{
		return;	// It's ok to not have an IHF file.
	}

	m_spriteDictionary.LoadDictionary( imageHeaderFilepath );
}

// Load the actual image file...
void Texture::LoadImage( unify::Path filePath )
{
	auto dxDevice = m_renderer->GetDxDevice();

	// Release any previous texture
	Destroy();

	// Verify file exists
	if ( !filePath.Exists() )
	{
		throw unify::Exception( "Failed to load image, file not found! (" + filePath.ToString() + ")" );
	}

	HRESULT result = S_OK;

	//DirectX::Image sourceImages{};
	DirectX::TexMetadata texMetadata{};

	//DirectX::ScratchImagem_scratch{};

	if ( m_parameters.source.IsExtension( "DDS" ) )
	{
		result = DirectX::LoadFromDDSFile( unify::Cast< std::wstring >( m_parameters.source.ToString() ).c_str(), 0, &texMetadata, m_scratch );
	}
	else if ( m_parameters.source.IsExtension( "BMP" ) || m_parameters.source.IsExtension( "JPG" ) || m_parameters.source.IsExtension( "JPEG" ) || m_parameters.source.IsExtension( "TIFF" ) || m_parameters.source.IsExtension( "TIF" ) || m_parameters.source.IsExtension( "HDP" ) || m_parameters.source.IsExtension( "PNG" ) )
	{
		result = DirectX::LoadFromWICFile( unify::Cast< std::wstring >( m_parameters.source.ToString() ).c_str(), 0, &texMetadata, m_scratch );
	}
	else if ( m_parameters.source.IsExtension( "TGA" ) )
	{
		result = DirectX::LoadFromTGAFile( unify::Cast< std::wstring >( m_parameters.source.ToString() ).c_str(), &texMetadata, m_scratch );
	}
	else
	{
		throw unify::Exception( "File format for \"" + m_parameters.source.ToString() + "\" not supported!" );
	}

	if ( FAILED( result ) )
	{
		throw unify::Exception( "Failed to load image \"" + m_parameters.source.ToString() + "\"!" );
	}

	unsigned int width = m_scratch.GetImage( 0, 0, 0 )->width;
	unsigned int height = m_scratch.GetImage( 0, 0, 0 )->height;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = m_scratch.GetImage( 0, 0, 0 )->pixels;
	data.SysMemPitch = m_scratch.GetImage( 0, 0, 0 )->rowPitch;
	data.SysMemSlicePitch = m_scratch.GetImage( 0, 0, 0 )->slicePitch;

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_scratch.GetImage( 0, 0, 0 )->format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = 0;
	result = dxDevice->CreateTexture2D( &textureDesc, &data, &m_texture );
	if ( FAILED( result ) )
	{
		Destroy();
		throw unify::Exception( "Failed to load image\"" + filePath.ToString() + "\"!" );
	}

	D3D11_SAMPLER_DESC colorMapDesc{};
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	if ( m_parameters.min == Filtering::Point && m_parameters.mag == Filtering::Point && m_parameters.mip == Filtering::Point )
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
	else if ( m_parameters.min == Filtering::Point && m_parameters.mag == Filtering::Point && m_parameters.mip == Filtering::Linear )
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	}
	else if ( m_parameters.min == Filtering::Point && m_parameters.mag == Filtering::Linear && m_parameters.mip == Filtering::Point )
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	}
	else if ( m_parameters.min == Filtering::Point && m_parameters.mag == Filtering::Linear && m_parameters.mip == Filtering::Linear )
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	}
	else if ( m_parameters.min == Filtering::Linear && m_parameters.mag == Filtering::Point && m_parameters.mip == Filtering::Point )
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	}
	else if ( m_parameters.min == Filtering::Linear && m_parameters.mag == Filtering::Point && m_parameters.mip == Filtering::Linear )
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	}
	else if ( m_parameters.min == Filtering::Linear && m_parameters.mag == Filtering::Linear && m_parameters.mip == Filtering::Point )
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}
	else if ( m_parameters.min == Filtering::Linear && m_parameters.mag == Filtering::Linear && m_parameters.mip == Filtering::Linear )
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}
	else if ( m_parameters.min == Filtering::Anisotropic && m_parameters.mag == Filtering::Anisotropic && m_parameters.mip == Filtering::Anisotropic )
	{
		colorMapDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	}									
	else
	{
		throw unify::Exception( "Bad filtering combination!" );
	}

	colorMapDesc.MipLODBias = 0.0f;
	colorMapDesc.BorderColor[0] = 0;
	colorMapDesc.BorderColor[1] = 0;
	colorMapDesc.BorderColor[2] = 0;
	colorMapDesc.BorderColor[3] = 0;
	colorMapDesc.MinLOD = 0;
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = dxDevice->CreateSamplerState( &colorMapDesc, &m_colorMapSampler );
	assert( !FAILED( result ) );

	D3D11_SHADER_RESOURCE_VIEW_DESC textureResourceDesc{};
	textureResourceDesc.Format = textureDesc.Format;
	textureResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureResourceDesc.Texture2D.MipLevels = 1;
	textureResourceDesc.Texture2D.MostDetailedMip = 0;

	result = dxDevice->CreateShaderResourceView( m_texture, &textureResourceDesc, &m_colorMap );
	assert( !FAILED( result ) );

	m_imageSize.width = width;
	m_imageSize.height = height;
}

SpriteDictionary & Texture::GetSpriteDictionary()
{
	return m_spriteDictionary;
}

const SpriteDictionary & Texture::GetSpriteDictionary() const
{
	return m_spriteDictionary;
}

const TextureParameters * Texture::GetParameters() const
{
	return &m_parameters;
}

bool Texture::Reload()
{
	Destroy();
	Create();
	return true;
}

std::string Texture::GetSource() const
{
	return m_parameters.source.ToXPath();
}

