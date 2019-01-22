// Copyright (c) 2003 - 2013, Evil Quail LLC
// All Rights Reserved

#include <medx11/ConstantBuffer.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/Exception.h>

using namespace medx11;
using namespace me;
using namespace render;

ConstantBuffer::ConstantBuffer( const me::render::IRenderer * renderer, me::render::ConstantBufferParameters parameters )
	: m_renderer{ dynamic_cast< const Renderer * >(renderer ) }
	, m_parameters{ parameters }
{
	Create( parameters );
}

ConstantBuffer::~ConstantBuffer()
{
	Destroy();
}

const me::render::ConstantTable * ConstantBuffer::GetTable() const
{
	return &m_table;
}

me::render::ConstantBufferParameters ConstantBuffer::GetParameters() const
{
	return m_parameters;
}

void ConstantBuffer::Create( ConstantBufferParameters parameters )
{
	Destroy();

	m_parameters.usage = me::render::BufferUsage::Dynamic;
	m_parameters = parameters;
	m_table = parameters.constantTable;

	auto dxDevice = m_renderer->GetDxDevice();

	for( size_t bufferIndex = 0, buffer_count = m_table.BufferCount(); bufferIndex < buffer_count; bufferIndex++ )
	{
		D3D11_BUFFER_DESC constantBufferDesc{};
		constantBufferDesc.ByteWidth = m_table.GetSizeInBytes( bufferIndex );
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ID3D11Buffer * createdBuffer;
		HRESULT result = dxDevice->CreateBuffer( &constantBufferDesc, nullptr, &createdBuffer );
		m_buffers.push_back( createdBuffer );
		assert( !FAILED( result ) );
	}
}

void ConstantBuffer::Destroy()
{
	for( auto && buffer : m_buffers )
	{
		buffer->Release();
	}
	m_buffers.clear();
	m_locked = {};
	m_bufferAccessed = {};
}

size_t ConstantBuffer::GetBufferCount() const
{
	return m_buffers.size();
}

void ConstantBuffer::Update( const RenderInfo & renderInfo, const unify::Matrix * world, size_t world_size )
{
	unify::DataLock lock;

	auto worldRef = m_table.GetWorld();
	auto viewRef = m_table.GetView();
	auto projRef = m_table.GetProjection();

	size_t bufferIndex = 0;
	for( size_t bufferIndex = 0, buffer_count = m_table.BufferCount(); bufferIndex < buffer_count; bufferIndex++ )
	{
		unify::DataLock lock;
		LockConstants( bufferIndex, lock );

		// Set automatic variables...

		if( bufferIndex == viewRef.buffer )
		{
			unsigned char * data = (lock.GetData<unsigned char>()) + viewRef.offsetInBytes;
			unify::Matrix* matrix = (unify::Matrix*)data;
			*matrix = renderInfo.GetViewMatrix();
		}

		if( bufferIndex == projRef.buffer )
		{
			unsigned char * data = (lock.GetData<unsigned char>()) + projRef.offsetInBytes;
			unify::Matrix* matrix = (unify::Matrix*)data;
			*matrix = renderInfo.GetProjectionMatrix();
		}

		if( world_size != 0 && bufferIndex == worldRef.buffer )
		{
			unsigned char * data = (lock.GetData<unsigned char>()) + worldRef.offsetInBytes;
			unify::Matrix* matrix = (unify::Matrix*)data;
			*matrix = world[0];
		}

		UnlockConstants( bufferIndex, lock );
		bufferIndex++;
	}
}

void ConstantBuffer::Use( size_t startSlot, size_t startBuffer )
{	
	if( m_locked != 0 )
	{
		throw unify::Exception( "Vertex shader is still locked, while attempting to use it!" );
	}

	// Ensure all buffers have been accessed (defaults)
	for( size_t buffer = 0, size = m_buffers.size(); buffer < size; ++buffer )
	{
		if( !m_table.HasDefaults( buffer ) ) continue;

		// Access test...
		if( ( m_bufferAccessed & (1 << buffer) ) != (1 << buffer) )
		{
			unify::DataLock lock;
			LockConstants( buffer, lock );
			UnlockConstants( buffer, lock );
		}
	}

	if( m_buffers.size() > 0 )
	{

		using me::render::ResourceType;
		using namespace render;

		auto dxContext = m_renderer->GetDxContext();

		switch( m_parameters.type )
		{
		case ResourceType::PixelShader:
			dxContext->PSSetConstantBuffers( startSlot, m_buffers.size(), &m_buffers[ startBuffer ] );
			break;

		case ResourceType::VertexShader:
			dxContext->VSSetConstantBuffers( startSlot, m_buffers.size(), &m_buffers[startBuffer] );
			break;

		case ResourceType::ComputeShader:
			dxContext->CSSetConstantBuffers( startSlot, m_buffers.size(), &m_buffers[startBuffer] );
			break;

		case ResourceType::DomainShader:
			dxContext->DSSetConstantBuffers( startSlot, m_buffers.size(), &m_buffers[startBuffer] );
			break;

		case ResourceType::GeometryShader:
			dxContext->GSSetConstantBuffers( startSlot, m_buffers.size(), &m_buffers[startBuffer] );
			break;

		default:
			throw unify::Exception( "ResourceType::ToString: Not a valid usage type!" );
		}
	}

	m_bufferAccessed = 0;
}

void ConstantBuffer::LockConstants( size_t bufferIndex, unify::DataLock & lock )
{
	if( (m_locked & (1 << bufferIndex)) == (1 << bufferIndex) ) throw exception::FailedToLock( "Failed to lock vertex shader constant buffer!" );

	m_bufferAccessed = m_bufferAccessed | (1 << bufferIndex);
	m_locked = m_locked | (1 << bufferIndex);

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	D3D11_MAPPED_SUBRESOURCE subresource{};
	HRESULT result = dxContext->Map( m_buffers[bufferIndex], bufferIndex, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subresource );
	if( FAILED( result ) )
	{
		throw unify::Exception( "Failed to lock " + me::render::ResourceType::ToString( m_parameters.type ) + " constant buffer!" );
	}

	lock.SetLock( subresource.pData, m_table.GetSizeInBytes( bufferIndex ), unify::DataLockAccess::ReadWrite, 0 );

	// Roughly handle defaults...
	for( auto variable : m_table.GetVariables( bufferIndex ) )
	{
		if( variable.hasDefault )
		{
			lock.CopyBytesFrom( &variable.default[0], variable.offsetInBytes, variable.default.size() * sizeof( float ) );
		}
	}
}

void ConstantBuffer::UnlockConstants( size_t buffer, unify::DataLock & lock )
{
	if( (m_locked & (1 << buffer)) != (1 << buffer) ) throw exception::FailedToLock( "Failed to unlock vertex shader constant buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffers[buffer], buffer );

	m_locked = m_locked & ~(1 << buffer);
}

ResourceType::TYPE ConstantBuffer::GetType() const
{
	return m_parameters.type;
}

BufferUsage::TYPE ConstantBuffer::GetUsage() const
{
	return m_parameters.usage;
}
