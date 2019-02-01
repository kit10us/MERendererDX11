// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <medx11/PixelShader.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/game/IGame.h>

using namespace medx11;
using namespace me;
using namespace render;

PixelShader::PixelShader( IRenderer * renderer, PixelShaderParameters parameters )
	: m_renderer{ dynamic_cast< Renderer *  >(renderer) }
	, m_parameters{ parameters }
	, m_blendDesc{}
{
	Create( parameters );
}

PixelShader::~PixelShader()
{
	Destroy();
}

void PixelShader::Destroy()
{
	m_pixelShader = nullptr;
	m_pixelShaderBuffer = nullptr;
}

void PixelShader::Create( PixelShaderParameters parameters )
{
	Destroy();

	m_parameters = parameters;

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
			  
	// Create blend state...
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
			throw exception::FailedToCreate( "Failed to create pixel shader \"" + m_parameters.path.ToString() + "\" blending state!" );
		}
	}

	//m_constantBuffer = { CreateConstantBuffer( me::render::BufferUsage::Dynamic ) };
}

me::render::BlendDesc PixelShader::GetBlendDesc() const
{
	return m_parameters.blendDesc;
}

me::render::IConstantBuffer::ptr PixelShader::CreateConstantBuffer( BufferUsage::TYPE usage ) const
{
	ConstantBuffer::ptr constantBuffer{ new ConstantBuffer( m_renderer, ConstantBufferParameters{ me::render::ResourceType::PixelShader, usage, m_parameters.constantTable } ) };
	return constantBuffer;
}

/*
IConstantBuffer * PixelShader::GetConstantBuffer()
{
	return m_constantBuffer.get();
}

const IConstantBuffer * PixelShader::GetConstantBuffer() const
{
	return GetConstantBuffer();
}
*/

const void * PixelShader::GetBytecode() const
{
	return m_pixelShaderBuffer->GetBufferPointer();
}

size_t PixelShader::GetBytecodeLength() const
{
	return m_pixelShaderBuffer->GetBufferSize();
}

void PixelShader::Use()
{
	auto dxContext = m_renderer->GetDxContext();
	dxContext->PSSetShader( m_pixelShader, nullptr, 0 );

	//m_constantBuffer->Use( 0, 0 );

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

bool PixelShader::IsTrans() const
{
	return m_parameters.trans;
}

bool PixelShader::Reload()
{
	Destroy();
	Create( m_parameters );
	return true;
}

std::string PixelShader::GetSource() const
{
	return m_parameters.path.ToXPath();
}
