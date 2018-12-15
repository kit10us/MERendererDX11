// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <me/shader/ConstantBuffer.h>
#include <me/render/VertexDeclaration.h>
#include <unify/Path.h>
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

		const me::shader::ConstantBuffer * GetConstants() const override;

		void LockConstants( size_t buffer, unify::DataLock & lock ) override;	   

		void UnlockConstants( size_t buffer, unify::DataLock & lock ) override;

		void SetVertexDeclaration( me::render::VertexDeclaration::ptr vertexDeclaration );

		me::render::VertexDeclaration::ptr GetVertexDeclaration() const override;

		const void * GetBytecode() const override;

		size_t GetBytecodeLength() const override;

		void Use() override;

		std::string GetSource() const override;

		bool Reload() override;

		bool IsTrans() const override;

		std::string GetError();

	protected:	   
		me::render::VertexShaderParameters m_parameters;
		bool m_assembly;
		std::string m_errorMessage;
		bool m_created;
		
		me::shader::ConstantBuffer::ptr m_constants;

		me::render::VertexDeclaration::ptr m_vertexDeclaration;
		Renderer * m_renderer;

		CComPtr< ID3D11VertexShader > m_vertexShader;
		CComPtr< ID3D10Blob > m_vertexShaderBuffer;
		//CComPtr< ID3D11ShaderReflection > m_vertexShaderReflection;

		std::vector< ID3D11Buffer * > m_constantBuffers;
		size_t m_bufferAccessed;
		size_t m_locked;
	};
}