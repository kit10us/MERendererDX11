// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <medx11/DirectX.h>
#include <mewos/IWindowsOS.h>
#include <me/render/IRenderer.h>
#include <me/render/Display.h>
#include <atlbase.h>
#include <memory>

namespace medx11
{
	class Renderer : public me::render::IRenderer
	{
	public:
		Renderer( mewos::IWindowsOS * os, me::render::Display display, size_t index );
		virtual ~Renderer();				

		ID3D11Device * GetDxDevice() const;
		ID3D11DeviceContext * GetDxContext() const;

	public: // me::render::IRenderer...
		//me::game::IGame* GetGame() override;

		const me::render::Display & GetDisplay() const;

		me::render::Viewport GetViewport() const override;

		void BeforeRender() override;
		void BeforeRenderSolids() override;
		void BeforeRenderTrans() override;
		void AfterRender() override;

		bool IsFullscreen() const override;

		size_t GetIndex() const override;

		void Render( const me::render::RenderInfo & renderInfo, const me::render::RenderMethod & method, me::render::Effect::ptr effect, me::render::IConstantBuffer * vertexCB, me::render::IConstantBuffer * pixelCB, me::render::MatrixFeed & matrixFeed ) override;
		void Render( const me::render::RenderInfo & renderInfo, const me::render::RenderMethod & method, me::render::BufferSet * bufferSet, me::render::MatrixFeed & matrixFeed ) override;

		me::render::IVertexBuffer::ptr ProduceVB( me::render::VertexBufferParameters parameters ) override;
		me::render::IIndexBuffer::ptr ProduceIB( me::render::IndexBufferParameters parameters ) override;
		me::render::IVertexShader::ptr ProduceVS( me::render::VertexShaderParameters parameters ) override;
		me::render::IPixelShader::ptr ProducePS( me::render::PixelShaderParameters parameters ) override;
		me::render::ITexture::ptr ProduceT( me::render::TextureParameters parameters ) override;
 		me::render::IVertexConstruct::ptr ProduceVC( const me::render::VertexDeclaration & vd, const me::render::IVertexShader & vs ) override;

		void UseTextures( std::vector< me::render::ITexture::ptr > textures ) override;

	private:
		me::render::Display m_display;
		size_t m_index;

		CComPtr< ID3D11Device > m_dxDevice;
		CComPtr< ID3D11DeviceContext > m_dxContext;
		DXGI_SWAP_CHAIN_DESC m_swapChainDesc;
		CComPtr< IDXGISwapChain > m_swapChain;
		CComPtr< ID3D11RenderTargetView > m_renderTargetView;
		CComPtr< ID3D11Texture2D > m_depthStencilBuffer;
		CComPtr< ID3D11DepthStencilView > m_depthStencilView;
		CComPtr< ID3D11RasterizerState > m_rasterizerState;	

		CComPtr< ID3D11DepthStencilState> m_depthStencilState_Solids;
		CComPtr< ID3D11DepthStencilState> m_depthStencilState_Trans;

		size_t m_totalInstances;
		CComPtr< ID3D11Buffer > m_instanceBufferM[ 2 ];
	};
}