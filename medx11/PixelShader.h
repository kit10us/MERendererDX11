// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <unify/Path.h>
#include <memory>
#include <atlbase.h>

namespace medx11
{
	class PixelShader : public me::render::IPixelShader
	{
	public:
		PixelShader( me::render::IRenderer * renderer );
		PixelShader( me::render::IRenderer * renderer, me::render::PixelShaderParameters parameters );

		~PixelShader();

		void Destroy();

		void Create( me::render::PixelShaderParameters parameters );

		const me::shader::ConstantBuffer * GetConstants() const override;

		void LockConstants( size_t buffer, unify::DataLock & lock ) override;	   

		void UnlockConstants( size_t buffer, unify::DataLock & lock ) override;

		void Use() override;

		std::string GetSource() const override;

		bool Reload() override;

		bool IsTrans() const;

		std::string GetError();

	protected:
		me::render::PixelShaderParameters m_parameters;
		std::string m_errorMessage;
		bool m_created;

		bool m_isTrans;	// Does this pixel shader turn the render into transparent (in part or entire)

		Renderer * m_renderer;
		CComPtr< ID3D11PixelShader > m_pixelShader;
		CComPtr< ID3D10Blob > m_pixelShaderBuffer;

		me::shader::ConstantBuffer::ptr m_constants;
		std::vector< ID3D11Buffer * > m_constantBuffers;
		size_t m_bufferAccessed;
		size_t m_locked;

		CComPtr< ID3D11BlendState > m_blendState;
		D3D11_BLEND_DESC m_blendDesc;
	};
}