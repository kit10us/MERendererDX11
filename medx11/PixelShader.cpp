// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx11/PixelShader.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/game/IGame.h>

using namespace medx11;
using namespace me;
using namespace shader;
using namespace render;

PixelShader::PixelShader( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >(renderer) )
	, m_created( false )
	, m_isTrans( false )
	, m_locked( 0 )
	, m_bufferAccessed( 0 )
	, m_blendDesc{}
{
}

PixelShader::PixelShader( IRenderer * renderer, PixelShaderParameters parameters )
	: PixelShader( renderer )
{
	Create( parameters );
}

PixelShader::~PixelShader()
{
	Destroy();
}

void PixelShader::Destroy()
{
	for( auto && buffer : m_constantBuffers )
	{
		buffer->Release();
	}
	m_constantBuffers.clear();

	m_pixelShader = nullptr;
	m_pixelShaderBuffer = nullptr;
}

void PixelShader::Create( PixelShaderParameters parameters )
{
	Destroy();

	m_parameters = parameters;
	m_constants = parameters.constants;

	bool debug =
#if defined( DEBUG ) || defined( _DEBUG )
		true;
#else
		false;
#endif

	HRESULT result;
	CComPtr< ID3D10Blob > errorBlob; // Generic buffer for error data.
	D3D_SHADER_MACRO * shaderMacros = 0;
	unsigned int flags1 = D3DCOMPILE_ENABLE_STRICTNESS;
	if ( debug )
	{
		flags1 |= D3DCOMPILE_DEBUG;
	}

	unsigned int flags2 = 0; // Only used for effect compilation.

	if ( ! m_parameters.code.empty() )
	{
		result = D3DCompile( m_parameters.code.c_str(), m_parameters.code.length(), nullptr, nullptr, nullptr, m_parameters.entryPointName.c_str(), m_parameters.profile.c_str(), flags1, flags2, &m_pixelShaderBuffer, &errorBlob );
	}
	else if ( !m_parameters.path.Empty() )
	{
		result = D3DCompileFromFile( unify::Cast< std::wstring >( m_parameters.path.ToString() ).c_str(), shaderMacros, D3D_COMPILE_STANDARD_FILE_INCLUDE, m_parameters.entryPointName.c_str(), m_parameters.profile.c_str(), flags1, flags2, &m_pixelShaderBuffer, &errorBlob );
	}
	else
	{
		throw exception::FailedToCreate( "Failed to create shader, neither code nor file path specified!" );
	}

	if ( FAILED( result ) )
	{
		OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
		throw exception::FailedToCreate( std::string( "Failed to create shader \"" ) + m_parameters.path.ToString() + "\": " +  std::string( (char*)errorBlob->GetBufferPointer() ) );
	}

	auto dxDevice = m_renderer->GetDxDevice();

	ID3D11ClassLinkage * classLinkage = nullptr;
	result = dxDevice->CreatePixelShader( m_pixelShaderBuffer->GetBufferPointer(), m_pixelShaderBuffer->GetBufferSize(), classLinkage, &m_pixelShader );
	if ( FAILED( result ) )
	{
		throw exception::FailedToCreate( "Failed to create shader!" );
	}

	using namespace DirectX;
			  
	if ( m_constants )
	{
		for( size_t bufferIndex = 0, buffer_count = m_constants->BufferCount(); bufferIndex < buffer_count; bufferIndex++ )
		{
			D3D11_BUFFER_DESC constantBufferDesc{};
			constantBufferDesc.ByteWidth = m_constants->GetSizeInBytes( bufferIndex );
			constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			ID3D11Buffer * createdBuffer;
			result = dxDevice->CreateBuffer( &constantBufferDesc, nullptr, &createdBuffer );
			m_constantBuffers.push_back( createdBuffer );
			assert( !FAILED( result ) );
		}
	}

	// Create blend state...
	//if( m_parameters.trans )
	{
		m_blendDesc.RenderTarget[0].BlendEnable = parameters.blendDesc.enable ? TRUE : FALSE;
		m_blendDesc.RenderTarget[0].SrcBlend = (D3D11_BLEND)parameters.blendDesc.src;
		m_blendDesc.RenderTarget[0].DestBlend = (D3D11_BLEND)parameters.blendDesc.dest;
		m_blendDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)parameters.blendDesc.op;
		m_blendDesc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND)parameters.blendDesc.srcAlpha;
		m_blendDesc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND)parameters.blendDesc.destAlpha;
		m_blendDesc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP)parameters.blendDesc.opAlpha;
		m_blendDesc.RenderTarget[0].RenderTargetWriteMask = (UINT8)parameters.blendDesc.renderTargetWriteMask;
		HRESULT hr = dxDevice->CreateBlendState( &m_blendDesc, &m_blendState );

		if( FAILED( hr ) )
		{
			throw exception::FailedToCreate( "Pixel Shader failed to create blending state!" );
		}
	}
		
	m_created = true;
}

const ConstantBuffer * PixelShader::GetConstants() const
{
	return m_constants.get();
}

void PixelShader::LockConstants( size_t bufferIndex, unify::DataLock & lock )
{
	if ( (m_locked & (1 << bufferIndex)) == (1 << bufferIndex) ) throw exception::FailedToLock( "Failed to lock vertex shader constant buffer!" );

	m_bufferAccessed = m_bufferAccessed | (1 << bufferIndex);
	m_locked = m_locked | (1 << bufferIndex);

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	D3D11_MAPPED_SUBRESOURCE subresource{};
	HRESULT result = dxContext->Map( m_constantBuffers[ bufferIndex ], bufferIndex, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subresource );
	if ( FAILED( result ) )
	{
		throw unify::Exception( "Failed to set vertex shader!" );
	}		

	lock.SetLock( subresource.pData, m_constants->GetSizeInBytes( bufferIndex ), false, 0 );

	// Roughly handle defaults...
	for ( auto variable : m_constants->GetVariables( bufferIndex ) )
	{
		if ( variable.hasDefault )
		{
			lock.CopyBytesFrom( &variable.default[0], variable.offsetInBytes, variable.default.size() * sizeof( float ) );
		}
	}
}

void PixelShader::UnlockConstants( size_t buffer, unify::DataLock & lock )
{
	if ( (m_locked & (1 << buffer)) != (1 << buffer) ) throw exception::FailedToLock( "Failed to unlock vertex shader constant buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_constantBuffers[ buffer ], buffer );

	m_locked = m_locked & ~(1 << buffer);
}

void PixelShader::Use()
{
	auto dxContext = m_renderer->GetDxContext();
	dxContext->PSSetShader( m_pixelShader, nullptr, 0 );
	
	// Ensure all buffers have been accessed (defaults)
	for ( size_t buffer = 0, size = m_constantBuffers.size(); buffer < size; ++buffer )
	{
		// Access test...
		if ( (m_bufferAccessed & (1 << buffer)) != (1 << buffer) )
		{
			unify::DataLock lock;
			LockConstants( buffer, lock );
			UnlockConstants( buffer, lock );
		}		   		
	}

	if ( m_constantBuffers.size() > 0 )
	{
		dxContext->PSSetConstantBuffers( 0, m_constantBuffers.size(), &m_constantBuffers[0] );
	}

	// Blending...
	if( m_blendState )
	{
		float blendFactor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
		unsigned int sampleMask = 0xffffffff;
		dxContext->OMSetBlendState( m_blendState, blendFactor, sampleMask );
	}
	else
	{
		float blendFactor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
		unsigned int sampleMask = 0xffffffff;
		dxContext->OMSetBlendState( nullptr, blendFactor, sampleMask );
	}

}

std::string PixelShader::GetSource() const
{
	return m_parameters.path.ToXPath();
}

bool PixelShader::Reload()
{
	Destroy();
	Create( m_parameters );
	return true;
}

bool PixelShader::IsTrans() const
{
	return m_parameters.trans;
}
