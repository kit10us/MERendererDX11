// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <medx11/RendererFactory.h>
#include <medx11/Renderer.h>

using namespace medx11;

RendererFactory::RendererFactory( mewos::IWindowsOS * os )
	: m_os{ os }
{
}

RendererFactory::~RendererFactory()
{
}

me::render::IRenderer * RendererFactory::Produce( me::render::Display display, size_t index )
{
	return new medx11::Renderer( m_os, display, index );
}
