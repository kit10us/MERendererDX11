// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <medx11/DirectX.h>
#include <mewos/IWindowsOS.h>
#include <me/render/IRenderer.h>
#include <me/render/Display.h>
#include <me/render/IRendererFactory.h>
#include <atlbase.h>
#include <memory>

namespace medx11
{
	class RendererFactory : public me::render::IRendererFactory
	{
	public:
		RendererFactory( mewos::IWindowsOS * os );
		virtual ~RendererFactory();

	public: // me::render::IRenderFactory
		me::render::IRenderer * Produce( me::render::Display display, size_t index ) override;

	private:
		mewos::IWindowsOS * m_os;
	};
}