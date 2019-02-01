// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <me/render/IRenderer.h>
#include <me/render/Display.h>
#include <medx11/DirectX.h>
#include <mewos/WindowsOS.h>
#include <atlbase.h>
#include <memory>

namespace medx11
{
	class RendererFactory : public me::render::IRendererFactory
	{
	public:
		me::render::IRenderer * Produce( me::render::Display display, size_t index ) override;
	};
}