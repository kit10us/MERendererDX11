// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <medx11/IndexBuffer.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/exception/OutOfBounds.h>
#include <me/exception/NotImplemented.h>

using namespace medx11;
using namespace me;
using namespace render;

IndexBuffer::IndexBuffer( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >(renderer) )
	, m_locked( false )
	, m_usage( BufferUsage::Default )
	, m_length( 0 )
	, m_stride( 0 )
{
}

IndexBuffer::IndexBuffer( IRenderer * renderer, IndexBufferParameters parameters )
	: IndexBuffer( renderer )
{
	Create( parameters );
}						 

IndexBuffer::~IndexBuffer()
{
	Destroy();
}

void IndexBuffer::Create( IndexBufferParameters parameters )
{
	Destroy();

	auto dxDevice = m_renderer->GetDxDevice();

	HRESULT hr = S_OK;

	m_stride = sizeof( unsigned int );
	m_length = (unsigned int)parameters.countAndSource[0].count;

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = m_stride * m_length;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initialData = D3D11_SUBRESOURCE_DATA();
	initialData.pSysMem = parameters.countAndSource[0].source;

	// Create the buffer with the device.
	hr = dxDevice->CreateBuffer( &bufferDesc, &initialData, &m_buffer );
	if ( FAILED( hr ) )
	{
		throw exception::FailedToCreate( "Failed to create index buffer!" );
	}
}

void IndexBuffer::Resize( size_t bufferIndex, unsigned int numIndices )
{
	if ( GetUsage(bufferIndex) != BufferUsage::Staging )
	{
		throw unify::Exception( "IndexBuffer's usage is not Staging! Cannot resize without CPU read and write access." );
	}

	/*
	TODO:

	unsigned int oldNumberOfIndices = GetLength();
	size_t oldSize = GetSize();
	ID3D11Buffer * oldIndexBuffer = m_buffer;
	m_buffer = 0;
	m_length = 0;

	Create( numIndices, this->GetFormat(), GetUsage(), m_createFlags );

	HRESULT hr;
	unsigned char * oldData;
	hr = oldIndexBuffer->Lock( 0, 0, (void**)&oldData, D3DLOCK_READONLY );
	if( FAILED(hr) )
	{
	throw exception::FailedToLock( "Failed to lock index buffer!" );
	}

	unsigned char * newData;
	hr = m_buffer->Lock( 0, 0, (void**)&newData, D3DLOCK_DISCARD );
	if( FAILED(hr) )
	{
	oldIndexBuffer->Release();
	oldIndexBuffer = 0;
	throw exception::FailedToLock( "Failed to lock index buffer!" );
	}

	memcpy( newData, oldData, oldSize );
	m_buffer->Unlock();
	oldIndexBuffer->Unlock();
	oldIndexBuffer->Release();
	oldIndexBuffer = 0;
	*/
}

size_t IndexBuffer::Append( size_t bufferIndex, const IndexBuffer & from, size_t vertexOffset  )
{
	size_t offset = m_length;

	if ( from.m_length == 0 )
	{
		return offset;
	}

	if ( from.GetUsage(bufferIndex) != BufferUsage::Staging )
	{
		throw unify::Exception( "From IndexBuffer's usage is not Staging!" );
	}

	if ( m_length == 0 )
	{
		assert( 0 ); // TODO:
		//Create( from.GetLength(), nullptr, from.GetUsage(), from.m_pimpl->m_createFlags );
	}
	else
	{
		if ( GetUsage(bufferIndex) != BufferUsage::Staging )
		{
			throw unify::Exception( "IndexBuffer's usage is not Staging!" );
		}
		Resize( bufferIndex, m_length + from.m_length );
	}

	// Copy vertices...
	IndexLock locksrc, lockdest;
	Lock( bufferIndex, lockdest );

	from.LockReadOnly( bufferIndex, locksrc );

	for ( size_t i = 0; i < locksrc.Count(); ++i )
	{
		size_t to = offset + i;
		const void * rawData = locksrc.GetItemReadonly< unsigned char >( i );
		if ( locksrc.Stride() == 2 )
		{
			unsigned short fromIndex = *static_cast< const short int * >(rawData);
			lockdest.SetIndex( to, fromIndex + vertexOffset );
		}
		else
		{
			unsigned int fromIndex = *static_cast< const unsigned int * >(rawData);
			lockdest.SetIndex( to, fromIndex + vertexOffset );
		}
	}

	from.UnlockReadOnly( bufferIndex, locksrc );
	Unlock( bufferIndex, lockdest );

	return offset;
}

void IndexBuffer::Destroy()
{
	m_buffer = nullptr;
	m_length = 0;
}

size_t IndexBuffer::GetBufferCount() const
{
	return m_buffer ? 1 : 0;
}

void IndexBuffer::Lock( size_t bufferIndex, unify::DataLock & lock )
{
	if ( ! m_buffer ) throw exception::FailedToLock( "Failed to lock index buffer buffer (buffer not created)!" );
	if ( m_locked ) throw exception::FailedToLock( "Failed to lock index buffer buffer (buffer already locked)!" );

	auto dxContext = m_renderer->GetDxContext();
	D3D11_MAPPED_SUBRESOURCE subresource{};
	HRESULT result = dxContext->Map( m_buffer, (UINT)bufferIndex, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subresource );
	if ( FAILED( result ) )
	{
		throw unify::Exception( "Failed to set vertex shader!" );
	}		

	lock.SetLock( subresource.pData, m_stride * m_length, unify::DataLockAccess::ReadWrite, 0 );
	m_locked = true;
}

void IndexBuffer::LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
{
	if ( ! m_buffer ) throw exception::FailedToLock( "Failed to lock index buffer buffer (buffer not created)!" );
	if ( m_locked ) throw exception::FailedToLock( "Failed to lock index buffer buffer (buffer already locked)!" );

	auto dxContext = m_renderer->GetDxContext();
	D3D11_MAPPED_SUBRESOURCE subresource{};
	HRESULT result = dxContext->Map( m_buffer, (UINT)bufferIndex, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &subresource );
	if ( FAILED( result ) )
	{
		throw unify::Exception( "Failed to set vertex shader!" );
	}		

	lock.SetLock( subresource.pData, m_stride * m_length, unify::DataLockAccess::Readonly, 0 );
	m_locked = true;
}

void IndexBuffer::Unlock( size_t bufferIndex, unify::DataLock & lock )
{
	if ( ! m_buffer ) throw exception::FailedToLock( "Failed to unlock index buffer buffer (buffer not created)!" );
	if ( ! m_locked ) throw exception::FailedToLock( "Failed to unlock index buffer buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffer, (UINT)bufferIndex );

	m_locked = false;
}

void IndexBuffer::UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
{
	if ( ! m_buffer ) throw exception::FailedToLock( "Failed to unlock index buffer buffer (buffer not created)!" );
	if ( m_locked ) throw exception::FailedToLock( "Failed to unlock index buffer buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffer, (UINT)bufferIndex );

	m_locked = false;
}

bool IndexBuffer::Valid() const
{
	return m_buffer != 0;
}

void IndexBuffer::Use( size_t startBuffer, size_t startSlot ) const
{
	assert( ! startBuffer && ! startSlot );

	auto dxContext = m_renderer->GetDxContext();
	if ( !m_buffer )
	{
		return;
	}

	// Set the buffer.
	dxContext->IASetIndexBuffer( m_buffer, DXGI_FORMAT_R32_UINT, 0 );
}

bool IndexBuffer::Locked( size_t bufferIndex ) const
{
	return m_locked;
}

BufferUsage::TYPE IndexBuffer::GetUsage( size_t bufferIndex ) const
{
	return m_usage;
}
