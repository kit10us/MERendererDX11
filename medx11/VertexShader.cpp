// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <medx11/VertexShader.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>

using namespace medx11;
using namespace me;
using namespace render;

VertexShader::VertexShader( IRenderer * renderer, VertexShaderParameters parameters )
	: m_renderer{ dynamic_cast< Renderer *  >(renderer) }
	, m_parameters{ parameters }
{
	Create( parameters );
}

VertexShader::~VertexShader()
{
	Destroy();
}

void VertexShader::Destroy()
{
	//m_constantBuffer.reset();
	m_vertexShader = nullptr;
	m_vertexShaderBuffer = nullptr;
}

void VertexShader::Create( VertexShaderParameters parameters )
{
	Destroy();

	m_parameters = parameters;
	m_vertexDeclaration = parameters.vertexDeclaration;

	bool debug = false;
#if defined( DEBUG ) || defined( _DEBUG )
	debug = true;
#endif

	HRESULT result = S_OK;
	CComPtr< ID3D10Blob > errorBlob; // Generic buffer for error data.
	D3D_SHADER_MACRO * shaderMacros = 0;
	unsigned int flags1 = D3DCOMPILE_ENABLE_STRICTNESS;
	if ( debug )
	{
		flags1 |= D3DCOMPILE_DEBUG;
	}
	unsigned int flags2 = 0; // Only used for effect compilation.
	result = D3DCompileFromFile( unify::Cast< std::wstring >( m_parameters.path.ToString() ).c_str(), shaderMacros, D3D_COMPILE_STANDARD_FILE_INCLUDE, m_parameters.entryPointName.c_str(), m_parameters.profile.c_str(), flags1, flags2, &m_vertexShaderBuffer, &errorBlob );
	if (WIN_FAILED( result ) )
	{
		OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
		throw exception::FailedToCreate( std::string( "Failed to create shader \"" ) + m_parameters.path.ToString() + "\": " +  std::string( (char*)errorBlob->GetBufferPointer() ) );
	}

	auto dxDevice = m_renderer->GetDxDevice();
	ID3D11ClassLinkage * classLinkage = nullptr;
	result = dxDevice->CreateVertexShader( m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), classLinkage, &m_vertexShader );
	assert( !WIN_FAILED( result ) );

	m_vertexDeclaration->Build( m_renderer, *this );
}

void VertexShader::SetVertexDeclaration( VertexDeclaration::ptr vertexDeclaration )
{
	m_vertexDeclaration = vertexDeclaration;
}

VertexDeclaration::ptr VertexShader::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

IConstantBuffer::ptr VertexShader::CreateConstantBuffer( BufferUsage::TYPE usage ) const
{
	ConstantBuffer::ptr constantBuffer{ new ConstantBuffer( m_renderer, ConstantBufferParameters{ me::render::ResourceType::VertexShader, usage, m_parameters.constantTable } ) };
	return constantBuffer;
}

/*
IConstantBuffer * VertexShader::GetConstantBuffer()
{
	return m_constantBuffer.get();
}

const IConstantBuffer * VertexShader::GetConstantBuffer() const
{
	return GetConstantBuffer();
}
*/

const void * VertexShader::GetBytecode() const
{
	return m_vertexShaderBuffer->GetBufferPointer();
}

size_t VertexShader::GetBytecodeLength() const
{
	return m_vertexShaderBuffer->GetBufferSize();
}

void VertexShader::Use()
{
	auto dxContext = m_renderer->GetDxContext();

	m_vertexDeclaration->Use();
	dxContext->VSSetShader( m_vertexShader, nullptr, 0 );
}

bool VertexShader::IsTrans() const
{
	return m_parameters.trans;
}

bool VertexShader::Reload()
{
	Destroy();
	Create( m_parameters );
	return true;
}

std::string VertexShader::GetSource() const
{
	return m_parameters.path.ToXPath();
}

