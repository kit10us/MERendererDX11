// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <medx11/VertexBuffer.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/exception/NotImplemented.h>

using namespace medx11;
using namespace me;
using namespace render;

VertexBuffer::VertexBuffer( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >(renderer) )
	, m_usage( BufferUsage::Default )
{
}

VertexBuffer::VertexBuffer( IRenderer * renderer, VertexBufferParameters parameters )
	: VertexBuffer( renderer )
{
	Create( parameters );
}


VertexBuffer::~VertexBuffer()
{
	Destroy();
}

void VertexBuffer::Create( VertexBufferParameters parameters )
{
	Destroy();

	m_bbox = parameters.bbox;

	auto vd = parameters.vertexDeclaration;

	bool gaps = false; // We do not support gaps in buffers.
	for( size_t slot = 0; slot < vd->NumberOfSlots(); slot++ )
	{
		size_t count = 0;
		const void * source = 0;
		if ( slot < parameters.countAndSource.size() )
		{
			count = parameters.countAndSource[slot].count;
			source = parameters.countAndSource[slot].source;
		}

		if ( count == 0 )
		{
			if ( vd->GetInstances( slot ) > 0 )
			{
				count = vd->GetInstances( slot );
			}
			else
			{
				gaps = true;
				continue;
			}
		}
		else if ( gaps == true )
		{
			throw exception::FailedToCreate( "Vertex buffer has empty gaps, likely due to requested create sizes and instancing!" );
		}

		BufferUsage::TYPE usage = parameters.usage;
		if ( vd->GetInstancing( slot ) != Instancing::None )
		{
			usage = BufferUsage::Dynamic;
		}

		m_usage.push_back( usage );
		m_vertexDeclaration = vd;
		m_strides.push_back( m_vertexDeclaration->GetSizeInBytes( slot ) );
		m_lengths.push_back( count );

		// Ensure we have some sort of idea what we need to be...
		if ( m_strides[slot] * m_lengths[slot] == 0 )
		{
			throw exception::FailedToCreate( "Not a valid vertex buffer size!" );
		}

		auto dxDevice = m_renderer->GetDxDevice();

		// Ensure that if we are BufferUsage::Immutable, then source is not null.
		if ( usage == BufferUsage::Immutable && source == nullptr )
		{
			throw exception::FailedToCreate( "Vertex buffer is immutable, yet source is null!" );
		}

		D3D11_USAGE usageDX{};
		unsigned int CPUAccessFlags = 0; // TODO: This needs to be managed better (from parameters or XML?)
		switch ( m_usage[slot] )
		{
		case BufferUsage::Default:
			usageDX = D3D11_USAGE_DEFAULT;
			break;
		case BufferUsage::Immutable:
			usageDX = D3D11_USAGE_IMMUTABLE;
			break;
		case BufferUsage::Dynamic:
			usageDX = D3D11_USAGE_DYNAMIC;
			CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case BufferUsage::Staging:
			usageDX = D3D11_USAGE_STAGING;
			break;
		}

		D3D11_BUFFER_DESC vertexBufferDesc{};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = parameters.vertexDeclaration->GetSizeInBytes( slot ) * count;
		vertexBufferDesc.Usage = usageDX;
		vertexBufferDesc.CPUAccessFlags = CPUAccessFlags;

		{
			HRESULT result;
			ID3D11Buffer * buffer;
			if ( source != nullptr )
			{
				D3D11_SUBRESOURCE_DATA initialData{};
				initialData.pSysMem = source;
				result = dxDevice->CreateBuffer( &vertexBufferDesc, &initialData, &buffer );
			}
			else
			{
				result = dxDevice->CreateBuffer( &vertexBufferDesc, nullptr, &buffer );
			}
			
			if( FAILED( result ) )
			{
				throw unify::Exception( "Failed to create vertex buffer!" );
			}

			m_buffers.push_back( buffer );
		}
	}
}

VertexDeclaration::ptr VertexBuffer::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

unify::BBox< float > & VertexBuffer::GetBBox()
{
	return m_bbox;
}

const unify::BBox< float > & VertexBuffer::GetBBox() const
{
	return m_bbox;
}

bool VertexBuffer::Valid() const
{
	return m_buffers.size() == m_strides.size();
}

void VertexBuffer::Destroy()
{
	for ( auto && buffer : m_buffers )
	{
		buffer->Release();
	}
	m_buffers.clear();
	m_lengths.clear();
	m_strides.clear();
}

size_t VertexBuffer::GetBufferCount() const
{
	return m_buffers.size();
}

void VertexBuffer::Use( size_t startBuffer, size_t startSlot ) const
{
	auto dxContext = m_renderer->GetDxContext();
	std::vector< unsigned int > offsetInBytes( m_strides.size(), 0 );
	dxContext->IASetVertexBuffers( 0, m_buffers.size(), &m_buffers[0], &m_strides[0], &offsetInBytes[0] );
}

void VertexBuffer::Lock( size_t bufferIndex, unify::DataLock & lock )
{
	if ( bufferIndex >= m_buffers.size() ) throw exception::FailedToLock( "Failed to lock vertex  buffer (buffer index out of range)!" );
	if ( m_locked[ bufferIndex ] ) throw exception::FailedToLock( "Failed to lock vertex  buffer (buffer already locked)!" );

	auto dxContext = m_renderer->GetDxContext();
	D3D11_MAPPED_SUBRESOURCE subresource{};
	HRESULT result = dxContext->Map( m_buffers[ bufferIndex ], bufferIndex, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subresource );
	if ( FAILED( result ) )
	{
		throw unify::Exception( "Failed to set vertex shader!" );
	}		

	lock.SetLock( subresource.pData, m_strides[bufferIndex] * m_lengths[bufferIndex], unify::DataLockAccess::ReadWrite, 0 );
	m_locked[bufferIndex] = true;
}

void VertexBuffer::LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
{
	if ( bufferIndex >= m_buffers.size() ) throw exception::FailedToLock( "Failed to lock vertex  buffer (buffer index out of range)!" );
	if ( m_locked[ bufferIndex ] ) throw exception::FailedToLock( "Failed to lock vertex  buffer (buffer already locked)!" );

	auto dxContext = m_renderer->GetDxContext();
	D3D11_MAPPED_SUBRESOURCE subresource{};
	HRESULT result = dxContext->Map( m_buffers[ bufferIndex ], bufferIndex, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subresource );
	if ( FAILED( result ) )
	{
		throw unify::Exception( "Failed to set vertex shader!" );
	}		

	lock.SetLock( subresource.pData, m_strides[bufferIndex] * m_lengths[bufferIndex], unify::DataLockAccess::Readonly, 0 );
	m_locked[bufferIndex] = true;
}

void VertexBuffer::Unlock( size_t bufferIndex, unify::DataLock & lock )
{
	if ( bufferIndex >= m_buffers.size() ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer index out of range)!" );
	if ( ! m_locked[ bufferIndex ] ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffers[ bufferIndex ], bufferIndex );

	m_locked[bufferIndex] = false;
}

void VertexBuffer::UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
{
	if ( bufferIndex >= m_buffers.size() ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer index out of range)!" );
	if ( ! m_locked[ bufferIndex ] ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffers[ bufferIndex ], bufferIndex );
	
	m_locked[bufferIndex] = false;
}

bool VertexBuffer::Locked( size_t bufferIndex ) const
{
	return m_locked[ bufferIndex ];
}

BufferUsage::TYPE VertexBuffer::GetUsage( size_t bufferIndex ) const
{
	return m_usage[ bufferIndex ];
}
