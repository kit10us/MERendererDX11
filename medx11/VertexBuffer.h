// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <me/render/IVertexBuffer.h>
#include <medx11/ConstantBuffer.h>
#include <unify/BBox.h>
#include <atlbase.h>

namespace medx11
{
	class VertexBuffer : public me::render::IVertexBuffer
	{
	public:
		VertexBuffer( me::render::IRenderer * renderer );
		VertexBuffer( me::render::IRenderer * renderer, me::render::VertexBufferParameters parameters );
		~VertexBuffer();

	public: // me::render::IVertexBuffer
		void Create( me::render::VertexBufferParameters parameters ) override;

		me::render::VertexDeclaration::ptr GetVertexDeclaration() const override;

		unify::BBox< float > & GetBBox() override;
		const unify::BBox< float > & GetBBox() const override;

		bool Valid() const;

	public: // me::render::IBuffer
		void Destroy() override;

		size_t GetBufferCount() const override;

		void Use( size_t startBuffer, size_t startSlot ) const override;

		void Lock( size_t bufferIndex, unify::DataLock & lock ) override;
		void LockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;
		void Unlock( size_t bufferIndex, unify::DataLock & lock ) override;
		void UnlockReadOnly( size_t bufferIndex, unify::DataLock & lock ) const override;

		bool Locked( size_t bufferIndex ) const override;

		me::render::BufferUsage::TYPE GetUsage( size_t bufferIndex ) const override;

		size_t GetStride( size_t bufferIndex ) const override;
		size_t GetLength( size_t bufferIndex ) const override;
		size_t GetSizeInBytes( size_t bufferIndex ) const override;

	protected:
		const Renderer * m_renderer;

		me::render::VertexDeclaration::ptr m_vertexDeclaration;

		unify::BBox< float > m_bbox;

		std::vector< ID3D11Buffer * > m_buffers;
		
		mutable std::vector< bool > m_locked;
		std::vector< me::render::BufferUsage::TYPE > m_usage;
		std::vector< size_t > m_strides; // Size of each item in the buffer.
		std::vector< size_t > m_lengths; // Number of items we can store in the buffer.
	};
}