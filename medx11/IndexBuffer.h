// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <me/render/IIndexBuffer.h>
#include <me/render/BufferUsage.h>
#include <me/render/IndexLock.h>
#include <me/render/BufferUsage.h>
#include <unify/unify.h>
#include <unify/Flags.h>
#include <atlbase.h>
#include <memory>

namespace medx11
{
	class IndexBuffer : public me::render::IIndexBuffer
	{
	public:
		IndexBuffer( me::render::IRenderer * renderer );
		IndexBuffer( me::render::IRenderer * renderer, me::render::IndexBufferParameters parameters );
		~IndexBuffer();

		void Create( me::render::IndexBufferParameters parameters );
		void Resize( size_t bufferIndex, unsigned int numIndices );

		/// <summary>
		/// Append to our existing indices, the indices from ib, adding a vertex offset to each new index.
		/// Returns the index offset for the first index from 'from'.
		/// </summary>
		size_t Append( size_t bufferIndex, const IndexBuffer & from, size_t vertexOffset = 0 );
		void Destroy();

		size_t GetBufferCount() const override;

		void Lock( size_t bufferIndex, unify::DataLock & lock ) override;
		void LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;
		void Unlock( size_t bufferIndex, unify::DataLock & lock ) override;
		void UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;

		bool Valid() const;
		
		void Use( size_t startBuffer, size_t startSlot ) const override;

		bool Locked( size_t bufferIndex ) const override;
		me::render::BufferUsage::TYPE GetUsage( size_t bufferIndex ) const override;

	protected:
		const Renderer * m_renderer;
		unsigned int m_createFlags;
		CComPtr< ID3D11Buffer > m_buffer;
		mutable bool m_locked;
		me::render::BufferUsage::TYPE m_usage;
		unsigned int m_stride; // Size of each item in the buffer.
		unsigned int m_length; // Number of items we can store in the buffer.
	};
}