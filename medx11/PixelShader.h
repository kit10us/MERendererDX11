// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <medx11/ConstantBuffer.h>
#include <atlbase.h>

namespace medx11
{
	class PixelShader : public me::render::IPixelShader
	{
	public:
		PixelShader( me::render::IRenderer * renderer, me::render::PixelShaderParameters parameters );

		~PixelShader();

		void Destroy();

		void Create( me::render::PixelShaderParameters parameters );

	public: // me::render::IPixelShader
		me::render::BlendDesc GetBlendDesc() const override;

	public: // me::render::IShader
		me::render::IConstantBuffer::ptr CreateConstantBuffer( me::render::BufferUsage::TYPE usage ) const override;
		const void * GetBytecode() const override;
		size_t GetBytecodeLength() const override;
		void Use() override;
		bool IsTrans() const override;

	public: // rm::IResource
		bool Reload() override;
		std::string GetSource() const override;

	protected:
		Renderer * m_renderer;
		me::render::PixelShaderParameters m_parameters;
		CComPtr< ID3D11PixelShader > m_pixelShader;
		CComPtr< ID3D10Blob > m_pixelShaderBuffer;
		CComPtr< ID3D11BlendState > m_blendState;
		D3D11_BLEND_DESC m_blendDesc;
	};
}