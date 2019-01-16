// Copyright (c) 2003 - 2013, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <me/render/ResourceType.h>
#include <me/render/BufferUsage.h>

namespace medx11
{
	class ConstantBuffer : public me::render::IConstantBuffer
	{
	public:
		ConstantBuffer( const me::render::IRenderer * renderer );
		ConstantBuffer( const me::render::IRenderer * renderer, me::render::ConstantBufferParameters parameters );
		~ConstantBuffer();


	public: // me::render::IConstantBuffer
		const me::render::ConstantTable * GetTable() const override;

		void Create( me::render::ConstantBufferParameters parameters ) override;

		void Destroy() override;

		size_t GetBufferCount() const override;

		void Use( size_t startSlot, size_t startBuffer ) override;

		void LockConstants( size_t bufferIndex, unify::DataLock & lock ) override;
		void UnlockConstants( size_t buffer, unify::DataLock & lock ) override;

		me::render::ResourceType::TYPE GetType() const override;

		me::render::BufferUsage::TYPE GetUsage() const override;

	protected:
		const Renderer * m_renderer;
		me::render::ConstantBufferParameters m_parameters;

		me::render::ConstantTable::ptr m_table;

		std::vector< ID3D11Buffer * > m_buffers;
		
		me::render::ResourceType::TYPE m_type;
		me::render::BufferUsage::TYPE m_usage;
		
		size_t m_locked;
		size_t m_bufferAccessed;
	};
}