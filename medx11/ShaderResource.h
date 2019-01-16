// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <me/render/IShaderResource.h>
#include <memory>

namespace medx11
{									  
	class ShaderResource : public me::render::IShaderResource
	{
		friend class Renderer;

	public:
		static bool s_allowTextureUses;

		ShaderResource( me::render::IRenderer * renderer, me::render::ShaderResourceParameters parameters );
		virtual ~ShaderResource();

		void Create();
		void Destroy();

	public: // me::render::IShaderResource
		void Use() const override;
		void Lock( unsigned int level, unify::DataLock & lock, unify::DataLockAccess::TYPE type ) override;
		void Unlock( unsigned int level ) override;

	public: // rm::IResource...

		bool Reload() override;
		std::string GetSource() const override;

	protected:
		me::render::ShaderResourceParameters m_parameters;
		const Renderer * m_renderer;
		CComPtr< ID3D11ShaderResourceView > m_resourceView;
		CComPtr< ID3D11Resource > m_resource;
	};
}