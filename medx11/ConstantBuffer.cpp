// Copyright (c) 2003 - 2013, Evil Quail LLC
// All Rights Reserved

#include <medx11/ConstantBuffer.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/Exception.h>

using namespace medx11;
using namespace me;

ConstantBuffer::ConstantBuffer( const me::IRenderer * renderer )
	: m_renderer( dynamic_cast< const Renderer * >(renderer) )
	, m_usage( BufferUsage::Default )
{
}

ConstantBuffer::ConstantBuffer( const me::IRenderer * renderer, ConstantBufferParameters parameters )
	: ConstantBuffer( renderer )
{
	Create( parameters );
}


ConstantBuffer::~ConstantBuffer()
{
	Destroy();
}

void ConstantBuffer::Create( ConstantBufferParameters parameters )
{
	Destroy();

	bool gaps = false; // We do not support gaps in buffers.
	for( size_t slot = 0; slot < parameters.countAndSourcesize(); slot++ )
	{
		size_t count = 0;
		const void * source = 0;
		count = parameters.countAndSource[slot].count;
		source = parameters.countAndSource[slot].source;

		BufferUsage::TYPE usage = parameters.usage;
		if ( vd->GetInstancing( slot ) != Instancing::None )
		{
			usage = BufferUsage::Dynamic;
		}

		m_usage.push_back( usage );
		m_strides.push_back( m_vertexDeclaration->GetSizeInBytes( slot ) );
		m_lengths.push_back( count );

		// Ensure we have some sort of idea what we need to be...
		if ( GetSizeInBytes( slot ) == 0 )
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
		OnFailedThrow( result, "Failed to create vertex buffer!" );
		m_buffers.push_back( buffer );
	}
}

void ConstantBuffer::Destroy()
{
	for ( auto && buffer : m_buffers )
	{
		buffer->Release();
	}
	m_buffers.clear();
	m_lengths.clear();
	m_strides.clear();
}

void ConstantBuffer::Lock( size_t bufferIndex, unify::DataLock & lock )
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

	lock.SetLock( subresource.pData, GetSizeInBytes(bufferIndex), false, 0 );
	m_locked[bufferIndex] = true;
}

void ConstantBuffer::LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
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

	lock.SetLock( subresource.pData, GetSizeInBytes(bufferIndex), true, 0 );
	m_locked[bufferIndex] = true;
}

void ConstantBuffer::Unlock( size_t bufferIndex, unify::DataLock & lock )
{
	if ( bufferIndex >= m_buffers.size() ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer index out of range)!" );
	if ( ! m_locked[ bufferIndex ] ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffers[ bufferIndex ], bufferIndex );

	m_locked[bufferIndex] = false;
}

void ConstantBuffer::UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
{
	if ( bufferIndex >= m_buffers.size() ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer index out of range)!" );
	if ( ! m_locked[ bufferIndex ] ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffers[ bufferIndex ], bufferIndex );
	
	m_locked[bufferIndex] = false;
}

VertexDeclaration::ptr ConstantBuffer::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

bool ConstantBuffer::Valid() const
{
	return m_buffers.size() == m_strides.size();
}

void ConstantBuffer::Use() const
{	
	auto dxContext = m_renderer->GetDxContext();
	std::vector< unsigned int > offsetInBytes( m_strides.size(), 0 );
	dxContext->IASetConstantBuffers( 0, m_buffers.size(), &m_buffers[0], &m_strides[0], &offsetInBytes[0] );
}

unify::BBox< float > & ConstantBuffer::GetBBox()
{
	return m_bbox;
}

const unify::BBox< float > & ConstantBuffer::GetBBox() const
{
	return m_bbox;
}

bool ConstantBuffer::Locked( size_t bufferIndex ) const
{
	return m_locked[ bufferIndex ];
}

BufferUsage::TYPE ConstantBuffer::GetUsage( size_t bufferIndex ) const
{
	return m_usage[ bufferIndex ];
}

size_t ConstantBuffer::GetStride( size_t bufferIndex ) const
{
	return m_strides[ bufferIndex ];
}

size_t ConstantBuffer::GetLength( size_t bufferIndex ) const
{
	return m_lengths[ bufferIndex ];
}

size_t ConstantBuffer::GetSizeInBytes( size_t bufferIndex ) const
{
	return m_strides[ bufferIndex ] * m_lengths[ bufferIndex ];
}
