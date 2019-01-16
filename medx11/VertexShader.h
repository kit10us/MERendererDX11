// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <medx11/ConstantBuffer.h>
#include <me/render/VertexDeclaration.h>
#include <atlbase.h>

namespace medx11
{
	class VertexShader : public me::render::IVertexShader
	{
	public:
		VertexShader( me::render::IRenderer * renderer );
		VertexShader( me::render::IRenderer * renderer, me::render::VertexShaderParameters parameters );

		~VertexShader();

		void Destroy();

		void Create( me::render::VertexShaderParameters parameters );

	public: // me::render::IVertexShader
		void SetVertexDeclaration( me::render::VertexDeclaration::ptr vertexDeclaration ) override;
		me::render::VertexDeclaration::ptr GetVertexDeclaration() const override;

	public: // me::render::IShader
		me::render::IConstantBuffer * GetConstantBuffer() override;
		const me::render::IConstantBuffer * GetConstantBuffer() const override;
		const void * GetBytecode() const override;
		size_t GetBytecodeLength() const override;
		void Use() override;
		bool IsTrans() const override;

	public: // rm::IResource
		bool Reload() override;
		std::string GetSource() const override;

	protected:	   
		Renderer * m_renderer;
		me::render::VertexShaderParameters m_parameters;
		bool m_assembly;
		me::render::VertexDeclaration::ptr m_vertexDeclaration;
		CComPtr< ID3D11VertexShader > m_vertexShader;
		CComPtr< ID3D10Blob > m_vertexShaderBuffer;
		ConstantBuffer m_constantBuffer;
	};
}