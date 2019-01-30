// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved
//

#pragma once

#include <me/render/Format.h>
#include <me/render/BufferUsage.h>
#include <me/render/ITexture.h>
#include <unify/Cast.h>
#include <dxgiformat.h>
#include <d3d11.h>

namespace unify
{
	template<> DXGI_FORMAT Cast( me::render::Format::TYPE format );
	template<> me::render::Format::TYPE Cast( DXGI_FORMAT format );

	template<> D3D11_USAGE Cast( me::render::BufferUsage::TYPE usage );
	template<> me::render::BufferUsage::TYPE Cast( D3D11_USAGE usage );
}
