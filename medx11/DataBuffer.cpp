// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx11/DataBuffer.h>
#include <me/exception/FailedToCreate.h>
#include <me/exception/FailedToLock.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/Exception.h>

using namespace medx11;
using namespace me;
using namespace render;

DataBuffer::DataBuffer( IRenderer * renderer )
	: m_renderer( dynamic_cast< Renderer * >(renderer) )
	, m_usage( BufferUsage::Default )
	// TODO: , m_shaderResourceView{}
{
}

DataBuffer::DataBuffer( IRenderer * renderer, DataBufferParameters parameters )
	: DataBuffer( renderer )
{
	Create( parameters );
}


DataBuffer::~DataBuffer()
{
	Destroy();
}

void DataBuffer::Create( DataBufferParameters parameters )
{
	Destroy();

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
		usage = BufferUsage::Dynamic;

		m_usage.push_back( usage );
		m_strides.push_back( parameters.stride );
		m_lengths.push_back( count );

		// Ensure we have some sort of idea what we need to be...
		if ( GetSizeInBytes( slot ) == 0 )
		{
			throw exception::FailedToCreate( "Not a valid data buffer size!" );
		}

		auto dxDevice = m_renderer->GetDxDevice();

		// Ensure that if we are BufferUsage::Immutable, then source is not null.
		if ( usage == BufferUsage::Immutable && source == nullptr )
		{
			throw exception::FailedToCreate( "Data buffer is immutable, yet source is null!" );
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

		D3D11_BUFFER_DESC DataBufferDesc{};
		DataBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		DataBufferDesc.ByteWidth = parameters.stride;
		DataBufferDesc.Usage = usageDX;
		DataBufferDesc.CPUAccessFlags = CPUAccessFlags;

		HRESULT result;
		ID3D11Buffer * buffer;
		if ( source != nullptr )
		{
			D3D11_SUBRESOURCE_DATA initialData{};
			initialData.pSysMem = source;
			result = dxDevice->CreateBuffer( &DataBufferDesc, &initialData, &buffer );
		}
		else
		{
			result = dxDevice->CreateBuffer( &DataBufferDesc, nullptr, &buffer );
		}
		OnFailedThrow( result, "Failed to create vertex buffer!" );
		m_buffers.push_back( buffer );
	}
}

void DataBuffer::Destroy()
{
	m_buffers.Clear();
	m_lengths.clear();
	m_strides.clear();
}

void DataBuffer::Use( ConstantBufferUse bufferUse ) const
{
	auto dxContext = m_renderer->GetDxContext();

	switch( bufferUse )
	{
		case ConstantBufferUse::PixelShader:
		dxContext->PSSetConstantBuffers( 0, m_buffers.Size(), m_buffers.GetBuffers() );
		break;

		case ConstantBufferUse::VertexShader:
		dxContext->VSSetConstantBuffers( 0, m_buffers.Size(), m_buffers.GetBuffers() );
		break;

		case ConstantBufferUse::ComputeShader:
		dxContext->CSSetConstantBuffers( 0, m_buffers.Size(), m_buffers.GetBuffers() );
		break;
		
		case ConstantBufferUse::DomainShader:
		dxContext->DSSetConstantBuffers( 0, m_buffers.Size(), m_buffers.GetBuffers() );
		break;

		case ConstantBufferUse::GeometryShader:
		dxContext->GSSetConstantBuffers( 0, m_buffers.Size(), m_buffers.GetBuffers() );
		break;

		case ConstantBufferUse::HullShader:
		dxContext->HSSetConstantBuffers( 0, m_buffers.Size(), m_buffers.GetBuffers() );
		break;
	}
}

void DataBuffer::Lock( size_t bufferIndex, unify::DataLock & lock )
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

	lock.SetLock( subresource.pData, GetSizeInBytes(bufferIndex), unify::DataLock::ReadWrite, 0 );
	m_locked[bufferIndex] = true;
}

void DataBuffer::LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
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

	lock.SetLock( subresource.pData, GetSizeInBytes(bufferIndex), unify::DataLock::Readonly, 0 );
	m_locked[bufferIndex] = true;
}

void DataBuffer::Unlock( size_t bufferIndex, unify::DataLock & lock )
{
	if ( bufferIndex >= m_buffers.size() ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer index out of range)!" );
	if ( ! m_locked[ bufferIndex ] ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffers[ bufferIndex ], bufferIndex );

	m_locked[bufferIndex] = false;
}

void DataBuffer::UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const
{
	if ( bufferIndex >= m_buffers.size() ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer index out of range)!" );
	if ( ! m_locked[ bufferIndex ] ) throw exception::FailedToLock( "Failed to unlock vertex  buffer (buffer not locked)!" );

	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	dxContext->Unmap( m_buffers[ bufferIndex ], bufferIndex );
	
	m_locked[bufferIndex] = false;
}

bool DataBuffer::Valid() const
{
	return m_buffers.size() == m_strides.size();
}

bool DataBuffer::Locked( size_t bufferIndex ) const
{
	return m_locked[ bufferIndex ];
}

BufferUsage::TYPE DataBuffer::GetUsage( size_t bufferIndex ) const
{
	return m_usage[ bufferIndex ];
}

size_t DataBuffer::GetStride( size_t bufferIndex ) const
{
	return m_strides[ bufferIndex ];
}

size_t DataBuffer::GetLength( size_t bufferIndex ) const
{
	return m_lengths[ bufferIndex ];
}

size_t DataBuffer::GetSizeInBytes( size_t bufferIndex ) const
{
	return m_strides[ bufferIndex ] * m_lengths[ bufferIndex ];
}
