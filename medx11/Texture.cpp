// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved
//

#include <medx11/Texture.h>

#include <DDS.h>
#pragma comment( lib, "DirectXTex" )
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
	if ( ! m_parameters.source.Empty() )
	{
		LoadHeader();
		LoadImage( m_parameters.source );
	}
	else
	{
		CreateFromSize();
	}
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

me::render::TextureLockAccess Texture::GetLockAccess() const
{
	return m_parameters.lockAccess;
}

void Texture::LockRect( unsigned int level, TextureLock & lock, const unify::Rect< long > * rect, unify::DataLockAccess::TYPE access )
{
	if ( ! unify::DataLockAccess::Compatible( access, m_parameters.lockAccess.cpu ) )
	{
		throw exception::FailedToLock( "Attempted to lock texture with access " + unify::DataLockAccess::ToString( m_parameters.lockAccess.cpu ) + " for unsupported access " + unify::DataLockAccess::ToString( access ) + "!" );
	}

	if ( ! m_scratch.GetImageCount() )
	{	
		D3D11_MAP mapType;
		{
			using namespace unify;

			switch( access )
			{
			case DataLockAccess::Readonly:
				mapType = D3D11_MAP_READ;
				break;
			case DataLockAccess::Writeonly:
				mapType = D3D11_MAP_WRITE_DISCARD;
				break;
			case DataLockAccess::ReadWrite:
				mapType = D3D11_MAP_READ_WRITE;
				break;

			default:
				throw me::exception::FailedToLock( "Attempted to lock texture with access " + unify::DataLockAccess::ToString( m_parameters.lockAccess.cpu ) + " for unsupported access " + unify::DataLockAccess::ToString( access ) +"!" );
			}
		}
		
		auto dxContext = m_renderer->GetDxContext();
		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		auto result = dxContext->Map( m_texture, 0, mapType, 0, &mappedResource );
		if (WIN_FAILED( result ) )
		{
			throw me::exception::FailedToLock( "Failed to lock texture with access " + unify::DataLockAccess::ToString( m_parameters.lockAccess.cpu ) + " for unsupported access " + unify::DataLockAccess::ToString( access ) + "!" );
		}

		lock.pBits = (unsigned char*)mappedResource.pData;
		lock.uWidth = m_imageSize.width;
		lock.uHeight = m_imageSize.height;
		lock.uStride = mappedResource.RowPitch;
	
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
	else
	{
		lock.pBits = m_scratch.GetImage( level, 0, 0 )->pixels;
		lock.uStride = (UINT)m_scratch.GetImage( level, 0, 0 )->rowPitch;
		lock.bpp = 4;
		if( rect )
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
}

void Texture::UnlockRect( unsigned int level )
{
	auto dxContext = m_renderer->GetDxContext();
	if ( ! m_scratch.GetImageCount() )
	{
		dxContext->Unmap( m_texture, 0 );
	}
	else
	{
		// Nothing to do.
	}
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

void Texture::CreateFromSize()
{
	auto dxDevice = m_renderer->GetDxDevice();

	// Release any previous texture
	Destroy();

	HRESULT result = S_OK;

	DirectX::TexMetadata texMetadata{};

	if (WIN_FAILED( result ))
	{
		throw unify::Exception( "Failed to load image \"" + m_parameters.source.ToString() + "\"!" );
	}

	unsigned int width = m_parameters.size.width;
	unsigned int height = m_parameters.size.height;

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = unify::Cast< DXGI_FORMAT >( m_parameters.format );
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = 0;
	result = dxDevice->CreateTexture2D( &textureDesc, nullptr, &m_texture );
	if (WIN_FAILED( result ))
	{
		Destroy();
		throw unify::Exception( "Failed to create texture of size " + unify::Cast< std::string >( width ) + "x" + unify::Cast< std::string >( height ) + "!" );
	}

	D3D11_SAMPLER_DESC colorMapDesc{};
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	if (m_parameters.min == Filtering::Point && m_parameters.mag == Filtering::Point && m_parameters.mip == Filtering::Point)
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
	else if (m_parameters.min == Filtering::Point && m_parameters.mag == Filtering::Point && m_parameters.mip == Filtering::Linear)
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	}
	else if (m_parameters.min == Filtering::Point && m_parameters.mag == Filtering::Linear && m_parameters.mip == Filtering::Point)
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	}
	else if (m_parameters.min == Filtering::Point && m_parameters.mag == Filtering::Linear && m_parameters.mip == Filtering::Linear)
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	}
	else if (m_parameters.min == Filtering::Linear && m_parameters.mag == Filtering::Point && m_parameters.mip == Filtering::Point)
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	}
	else if (m_parameters.min == Filtering::Linear && m_parameters.mag == Filtering::Point && m_parameters.mip == Filtering::Linear)
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	}
	else if (m_parameters.min == Filtering::Linear && m_parameters.mag == Filtering::Linear && m_parameters.mip == Filtering::Point)
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}
	else if (m_parameters.min == Filtering::Linear && m_parameters.mag == Filtering::Linear && m_parameters.mip == Filtering::Linear)
	{
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}
	else if (m_parameters.min == Filtering::Anisotropic && m_parameters.mag == Filtering::Anisotropic && m_parameters.mip == Filtering::Anisotropic)
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
	assert( !WIN_FAILED( result ) );

	D3D11_SHADER_RESOURCE_VIEW_DESC textureResourceDesc{};
	textureResourceDesc.Format = textureDesc.Format;
	textureResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureResourceDesc.Texture2D.MipLevels = 1;
	textureResourceDesc.Texture2D.MostDetailedMip = 0;

	result = dxDevice->CreateShaderResourceView( m_texture, &textureResourceDesc, &m_colorMap );
	assert( !WIN_FAILED( result ) );

	m_imageSize.width = width;
	m_imageSize.height = height;
}

void Texture::LoadHeader()
{
	unify::Path imageHeaderFilepath{ unify::ChangeExtension( m_parameters.source, ".me_image" ) };

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

	DirectX::TexMetadata texMetadata{};

	if ( m_parameters.source.IsExtension( "DDS" ) )
	{
		result = DirectX::LoadFromDDSFile( unify::Cast< std::wstring >( m_parameters.source.ToString() ).c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &texMetadata, m_scratch );
	}
	else if ( m_parameters.source.IsExtension( "BMP" ) || m_parameters.source.IsExtension( "JPG" ) || m_parameters.source.IsExtension( "JPEG" ) || m_parameters.source.IsExtension( "TIFF" ) || m_parameters.source.IsExtension( "TIF" ) || m_parameters.source.IsExtension( "HDP" ) || m_parameters.source.IsExtension( "PNG" ) )
	{
		result = DirectX::LoadFromWICFile( unify::Cast< std::wstring >( m_parameters.source.ToString() ).c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texMetadata, m_scratch );
	}
	else if ( m_parameters.source.IsExtension( "TGA" ) )
	{
		result = DirectX::LoadFromTGAFile( unify::Cast< std::wstring >( m_parameters.source.ToString() ).c_str(), &texMetadata, m_scratch );
	}
	else
	{
		throw unify::Exception( "File format for \"" + m_parameters.source.ToString() + "\" not supported!" );
	}

	if (WIN_FAILED( result ) )
	{
		throw unify::Exception( "Failed to load image \"" + m_parameters.source.ToString() + "\"!" );
	}

	UINT width = (UINT)m_scratch.GetImage( 0, 0, 0 )->width;
	UINT height = (UINT)m_scratch.GetImage( 0, 0, 0 )->height;

	m_parameters.format = unify::Cast< me::render::Format::TYPE >( m_scratch.GetImage( 0, 0, 0 )->format );

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = m_scratch.GetImage( 0, 0, 0 )->pixels;
	data.SysMemPitch = (UINT)m_scratch.GetImage( 0, 0, 0 )->rowPitch;
	data.SysMemSlicePitch = (UINT)m_scratch.GetImage( 0, 0, 0 )->slicePitch;

	UINT cpuAccess {};

	{
		using namespace unify;

		switch( m_parameters.lockAccess.cpu )
		{
		case DataLockAccess::Readonly:
			cpuAccess = D3D11_CPU_ACCESS_READ;
			break;
		case DataLockAccess::Writeonly:
			cpuAccess = D3D11_CPU_ACCESS_WRITE;
			break;
		case DataLockAccess::ReadWrite:
			cpuAccess = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			break;

		default:
			cpuAccess = 0;
			break;
		}
	}

	UINT bindFlags {};
	bindFlags = D3D11_BIND_SHADER_RESOURCE;

	m_parameters.size = unify::Size< size_t >( width, height );

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_scratch.GetImage( 0, 0, 0 )->format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = unify::Cast< D3D11_USAGE >( m_parameters.usage );
	textureDesc.BindFlags = bindFlags;
	textureDesc.CPUAccessFlags = cpuAccess;
	textureDesc.MiscFlags = 0;
	result = dxDevice->CreateTexture2D( &textureDesc, &data, &m_texture );
	if (WIN_FAILED( result ) )
	{
		Destroy();
		throw unify::Exception( "Failed to create from file image\"" + filePath.ToString() + "\"!" );
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
	assert( !WIN_FAILED( result ) );

	D3D11_SHADER_RESOURCE_VIEW_DESC textureResourceDesc{};
	textureResourceDesc.Format = textureDesc.Format;
	textureResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureResourceDesc.Texture2D.MipLevels = 1;
	textureResourceDesc.Texture2D.MostDetailedMip = 0;

	result = dxDevice->CreateShaderResourceView( m_texture, &textureResourceDesc, &m_colorMap );
	assert( !WIN_FAILED( result ) );

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

