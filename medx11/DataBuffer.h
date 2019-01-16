// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <me/render/IDataBuffer.h>

namespace medx11
{
	class DataBuffer
	{
	public:
		DataBuffer( me::render::IRenderer * renderer );
		DataBuffer( me::render::IRenderer * renderer, me::render::DataBufferParameters parameters );
		~DataBuffer();

	public: // me::renderer::IDataBuffer
		void Create( me::render::DataBufferParameters parameters ) override;

	public: // me::IBuffer
		void Destroy();
		void Use( ConstantBufferUse bufferUse ) const override;

		void Lock( size_t bufferIndex, unify::DataLock & lock ) override;
		void LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;
		void Unlock( size_t bufferIndex, unify::DataLock & lock ) override;
		void UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;
		
		bool Valid() const;

		bool Locked( size_t bufferIndex ) const override;
		me::render::BufferUsage::TYPE GetUsage( size_t bufferIndex ) const override;
		size_t GetStride( size_t bufferIndex ) const override;
		size_t GetLength( size_t bufferIndex ) const override;
		size_t GetSizeInBytes( size_t bufferIndex ) const override;

	protected:
		const Renderer * m_renderer;

		BufferArray m_buffers;
	};
}