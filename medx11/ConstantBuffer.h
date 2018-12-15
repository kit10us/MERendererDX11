// Copyright (c) 2003 - 2013, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <me/IVertexBuffer.h>
#include <unify/BBox.h>
#include <atlbase.h>

namespace medx11
{
	struct ConstantBufferParameters
	{
	};

	class ConstantBuffer : public me::IBuffer
	{
	public:
		ConstantBuffer( const me::IRenderer * renderer );
		ConstantBuffer( const me::IRenderer * renderer, ConstantBufferParameters parameters );
		~ConstantBuffer();

		void Create( ConstantBufferParameters parameters );
		void Destroy();

		void Lock( size_t bufferIndex, unify::DataLock & lock ) override;
		void LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;
		void Unlock( size_t bufferIndex, unify::DataLock & lock ) override;
		void UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;
		
		bool Valid() const;
		void Use() const override;

		bool Locked( size_t bufferIndex ) const override;
		me::BufferUsage::TYPE GetUsage( size_t bufferIndex ) const override;
		size_t GetStride( size_t bufferIndex ) const override;
		size_t GetLength( size_t bufferIndex ) const override;
		size_t GetSizeInBytes( size_t bufferIndex ) const override;

	protected:
		const Renderer * m_renderer;
		std::vector< ID3D11Buffer * > m_buffers;

		mutable std::vector< bool > m_locked;
		std::vector< me::BufferUsage::TYPE > m_usage;
		std::vector< size_t > m_strides; // Size of each item in the buffer.
		std::vector< size_t > m_lengths; // Number of items we can store in the buffer.
	};
}