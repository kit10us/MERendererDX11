// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <medx11/Renderer.h>
#include <me/render/VertexDeclaration.h>
#include <me/render/IVertexShader.h>
#include <atlbase.h>

namespace medx11
{

	/// <summary> 
	/// Defines the structure of the data in a vertex. 
	/// </summary>
	class VertexConstruct : public me::render::IVertexConstruct
	{
	public:
		VertexConstruct( me::render::IRenderer * renderer, const me::render::VertexDeclaration & vd, const me::render::IVertexShader & vs );
		~VertexConstruct();
		
		void Use() const override;

	private:
		const Renderer * m_renderer;
		CComPtr< ID3D11InputLayout > m_layout;
	};
}