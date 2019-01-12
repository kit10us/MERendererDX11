// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx11/VertexShader.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>

using namespace medx11;
using namespace me;
using namespace shader;
using namespace render;

VertexShader::VertexShader( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >(renderer) )
	, m_assembly( false )
	, m_created( false )
	, m_locked{ 0 }
	, m_bufferAccessed{ 0 }
{
}

VertexShader::VertexShader( IRenderer * renderer, VertexShaderParameters parameters )
	: VertexShader( renderer )
{
	Create( parameters );
}

VertexShader::~VertexShader()
{
	Destroy();
}

void VertexShader::Destroy()
{
	for( auto && buffer : m_constantBuffers )
	{
		buffer->Release();
	}
	m_constantBuffers.clear();

	m_vertexShader = nullptr;
	m_vertexShaderBuffer = nullptr;
}

void VertexShader::Create( VertexShaderParameters parameters )
{
	Destroy();

	m_parameters = parameters;
	m_constants = parameters.constants;
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
	if ( FAILED( result ) )
	{
		OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
		throw exception::FailedToCreate( std::string( "Failed to create shader \"" ) + m_parameters.path.ToString() + "\": " +  std::string( (char*)errorBlob->GetBufferPointer() ) );
	}

	auto dxDevice = m_renderer->GetDxDevice();
	ID3D11ClassLinkage * classLinkage = nullptr;
	result = dxDevice->CreateVertexShader( m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), classLinkage, &m_vertexShader );
	assert( !FAILED( result ) );

	/*
	result = D3DReflect( m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_vertexShaderReflection );
	assert( !FAILED( result ) );
	*/

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
		
	m_vertexDeclaration->Build( m_renderer, *this );
	m_created = true;
}

const ConstantBuffer * VertexShader::GetConstants() const
{
	return m_constants.get();
}

void VertexShader::LockConstants( size_t bufferIndex, unify::DataLock & lock )
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

	lock.SetLock( subresource.pData, m_constants->GetSizeInBytes( bufferIndex ), unify::DataLock::ReadWrite, 0 );

	// Roughly handle defaults...
	for ( auto variable : m_constants->GetVariables( bufferIndex ) )
	{
		if ( variable.hasDefault )
		{
			lock.CopyBytesFrom( &variable.default[0], variable.offsetInBytes, variable.default.size() * sizeof( float ) );
		}
	}
}

void VertexShader::UnlockConstants( size_t buffer, unify::DataLock & lock )
{
	if ( (m_locked & (1 << buffer)) != (1 << buffer) ) throw exception::FailedToLock( "Failed to unlock vertex shader constant buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_constantBuffers[ buffer ], buffer );
	//dxContext->VSSetConstantBuffers( buffer, 1, &m_constantBuffers[ buffer ].p );

	m_locked = m_locked & ~(1 << buffer);
}

void VertexShader::SetVertexDeclaration( VertexDeclaration::ptr vertexDeclaration )
{
	m_vertexDeclaration = vertexDeclaration;
}

VertexDeclaration::ptr VertexShader::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

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
	if ( m_locked != 0 ) throw unify::Exception( "Vertex shader is still locked, while attempting to use it!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	// Ensure all buffers have been accessed (defaults)
	for ( size_t buffer = 0, size = m_constantBuffers.size(); buffer < size; ++buffer )
	{
		if ( !m_constants->HasDefaults( buffer ) ) continue;

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
		dxContext->VSSetConstantBuffers( 0, m_constantBuffers.size(), &m_constantBuffers[0] );
	}
	
	m_vertexDeclaration->Use();

	dxContext->VSSetShader( m_vertexShader, nullptr, 0 );
	m_bufferAccessed = 0;
}

std::string VertexShader::GetSource() const
{
	return m_parameters.path.ToXPath();
}

bool VertexShader::Reload()
{
	Destroy();
	Create( m_parameters );
	return true;
}

bool VertexShader::IsTrans() const
{
	return m_parameters.trans;
}

std::string VertexShader::GetError()
{
	return m_errorMessage;
}