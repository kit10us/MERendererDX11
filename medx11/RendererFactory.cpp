// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <medx11/RendererFactory.h>
#include <medx11/Renderer.h>

using namespace medx11;

me::render::IRenderer * RendererFactory::Produce( me::os::IOS * os, me::render::Display display, size_t index )
{
	return new medx11::Renderer( os, display, index );
}
