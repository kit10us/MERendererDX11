// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved
//

#include <medx11/Conversion.h>

template<> 
DXGI_FORMAT unify::Cast( me::render::Format::TYPE format )
{
	using namespace me::render;
	switch( format )
	{
	case Format::Unknown: return DXGI_FORMAT_UNKNOWN;
	case Format::R32G32B32A32_Typeless: return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case Format::R32G32B32A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case Format::R32G32B32A32_UInt: return DXGI_FORMAT_R32G32B32A32_UINT;
	case Format::R32G32B32A32_SInt: return DXGI_FORMAT_R32G32B32A32_SINT;
	case Format::R32G32B32_Typeless: return DXGI_FORMAT_R32G32B32_TYPELESS;
	case Format::R32G32B32_Float: return DXGI_FORMAT_R32G32B32_FLOAT;
	case Format::R32G32B32_UInt: return DXGI_FORMAT_R32G32B32_UINT;
	case Format::R32G32B32_SInt: return DXGI_FORMAT_R32G32B32_SINT;
	case Format::R16G16B16A16_Typeless: return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case Format::R16G16B16A16_Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case Format::R16G16B16A16_UNorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
	case Format::R16G16B16A16_UInt: return DXGI_FORMAT_R16G16B16A16_UINT;
	case Format::R16G16B16A16_SNorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
	case Format::R16G16B16A16_SInt: return DXGI_FORMAT_R16G16B16A16_SINT;
	case Format::R32G32_Typeless: return DXGI_FORMAT_R32G32_TYPELESS;
	case Format::R32G32_Float: return DXGI_FORMAT_R32G32_FLOAT;
	case Format::R32G32_UInt: return DXGI_FORMAT_R32G32_UINT;
	case Format::R32G32_SInt: return DXGI_FORMAT_R32G32_SINT;
	case Format::R32G8X24_Typeless: return DXGI_FORMAT_R32G8X24_TYPELESS;
	case Format::D32_Float_S8X24_UInt: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	case Format::R32_Float_X8X24_Typeless: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case Format::X32_Typeless_G8X24_UInt: return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
	case Format::R10G10B10A2_Typeless: return DXGI_FORMAT_R10G10B10A2_TYPELESS;
	case Format::R10G10B10A2_UNorm: return DXGI_FORMAT_R10G10B10A2_UNORM;
	case Format::R10G10B10A2_UInt: return DXGI_FORMAT_R10G10B10A2_UINT;
	case Format::R11G11B10_Float: return DXGI_FORMAT_R11G11B10_FLOAT;
	case Format::R8G8B8A8_Typeless: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case Format::R8G8B8A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case Format::R8G8B8A8_UNorm_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case Format::R8G8B8A8_UInt: return DXGI_FORMAT_R8G8B8A8_UINT;
	case Format::R8G8B8A8_SNorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
	case Format::R8G8B8A8_SInt: return DXGI_FORMAT_R8G8B8A8_SINT;
	case Format::R16G16_Typeless: return DXGI_FORMAT_R16G16_TYPELESS;
	case Format::R16G16_Float: return DXGI_FORMAT_R16G16_FLOAT;
	case Format::R16G16_UNorm: return DXGI_FORMAT_R16G16_UNORM;
	case Format::R16G16_UInt: return DXGI_FORMAT_R16G16_UINT;
	case Format::R16G16_SNorm: return DXGI_FORMAT_R16G16_SNORM;
	case Format::R16G16_SInt: return DXGI_FORMAT_R16G16_SINT;
	case Format::R32_Typeless: return DXGI_FORMAT_R32_TYPELESS;
	case Format::D32_Float: return DXGI_FORMAT_D32_FLOAT;
	case Format::R32_Float: return DXGI_FORMAT_R32_FLOAT;
	case Format::R32_UInt: return DXGI_FORMAT_R32_UINT;
	case Format::R32_SInt: return DXGI_FORMAT_R32_SINT;
	case Format::R24G8_Typeless: return DXGI_FORMAT_R24G8_TYPELESS;
	case Format::D24_UNorm_S8_UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case Format::R24_UNorm_X8_Typeless: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case Format::X24_Typeless_G8_UInt: return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	case Format::R8G8_Typeless: return DXGI_FORMAT_R8G8_TYPELESS;
	case Format::R8G8_UNorm: return DXGI_FORMAT_R8G8_UNORM;
	case Format::R8G8_UInt: return DXGI_FORMAT_R8G8_UINT;
	case Format::R8G8_SNorm: return DXGI_FORMAT_R8G8_SNORM;
	case Format::R8G8_SInt: return DXGI_FORMAT_R8G8_SINT;
	case Format::R16_Typeless: return DXGI_FORMAT_R16_TYPELESS;
	case Format::R16_Float: return DXGI_FORMAT_R16_FLOAT;
	case Format::D16_UNorm: return DXGI_FORMAT_D16_UNORM;
	case Format::R16_UNorm: return DXGI_FORMAT_R16_UNORM;
	case Format::R16_UInt: return DXGI_FORMAT_R16_UINT;
	case Format::R16_SNorm: return DXGI_FORMAT_R16_SNORM;
	case Format::R16_SInt: return DXGI_FORMAT_R16_SINT;
	case Format::R8_Typeless: return DXGI_FORMAT_R8_TYPELESS;
	case Format::R8_UNorm: return DXGI_FORMAT_R8_UNORM;
	case Format::R8_UInt: return DXGI_FORMAT_R8_UINT;
	case Format::R8_SNorm: return DXGI_FORMAT_R8_SNORM;
	case Format::R8_SInt: return DXGI_FORMAT_R8_SINT;
	case Format::A8_UNorm: return DXGI_FORMAT_A8_UNORM;
	case Format::R1_UNorm: return DXGI_FORMAT_R1_UNORM;
	case Format::R9G9B9E5_SharedExp: return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	case Format::R8G8_B8G8_UNorm: return DXGI_FORMAT_R8G8_B8G8_UNORM;
	case Format::G8R8_G8B8_UNorm: return DXGI_FORMAT_G8R8_G8B8_UNORM;
	case Format::BC1_Typeless: return DXGI_FORMAT_BC1_TYPELESS;
	case Format::BC1_UNorm: return DXGI_FORMAT_BC1_UNORM;
	case Format::BC2_Typeless: return DXGI_FORMAT_BC1_UNORM_SRGB;
	case Format::BC2_UNorm: return DXGI_FORMAT_BC2_TYPELESS;
	case Format::BC2_UNorm_SRGB: return DXGI_FORMAT_BC2_UNORM;
	case Format::BC3_Typeless: return DXGI_FORMAT_BC2_UNORM_SRGB;
	case Format::BC3_UNorm: return DXGI_FORMAT_BC3_TYPELESS;
	case Format::BC3_UNorm_SRGB: return DXGI_FORMAT_BC3_UNORM;
	case Format::BC4_Typeless: return DXGI_FORMAT_BC3_UNORM_SRGB;
	case Format::BC4_UNorm: return DXGI_FORMAT_BC4_TYPELESS;
	case Format::BC4_SNorm: return DXGI_FORMAT_BC4_UNORM;
	case Format::BC5_Typeless: return DXGI_FORMAT_BC4_SNORM;
	case Format::BC5_UNorm: return DXGI_FORMAT_BC5_TYPELESS;
	case Format::BC5_SNorm: return DXGI_FORMAT_BC5_UNORM;
	case Format::B5G6R5_UNorm: return DXGI_FORMAT_BC5_SNORM;
	case Format::B5G5R5A1_UNorm: return DXGI_FORMAT_B5G6R5_UNORM;
	case Format::B8G8R8A8_UNorm: return DXGI_FORMAT_B5G5R5A1_UNORM;
	case Format::B8G8R8X8_UNorm: return DXGI_FORMAT_B8G8R8A8_UNORM;
	case Format::R10G10B10_XR_BIAS_A2_UNorm: return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	case Format::B8G8R8A8Typeless: return DXGI_FORMAT_B8G8R8A8_TYPELESS;
	case Format::B8G8R8A8UNormSRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case Format::B8G8R8X8Typeless: return DXGI_FORMAT_B8G8R8X8_TYPELESS;
	case Format::B8G8R8X8UNormSRGB: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	case Format::BC6H_Typeless: return DXGI_FORMAT_BC6H_TYPELESS;
	case Format::BC6H_UF16: return DXGI_FORMAT_BC6H_UF16;
	case Format::BC6H_SF16: return DXGI_FORMAT_BC6H_SF16;
	case Format::BC7_Typless: return DXGI_FORMAT_BC7_TYPELESS;
	case Format::BC7_UNorm: return DXGI_FORMAT_BC7_UNORM;
	case Format::BC7_UNormSRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;
	case Format::AYUV: return DXGI_FORMAT_AYUV;
	case Format::Y410: return DXGI_FORMAT_Y410;
	case Format::Y416: return DXGI_FORMAT_Y416;
	case Format::NV12: return DXGI_FORMAT_NV12;
	case Format::P010: return DXGI_FORMAT_P010;
	case Format::P016: return DXGI_FORMAT_P016;
	case Format::Opaque420: return DXGI_FORMAT_420_OPAQUE;
	case Format::YUY2: return DXGI_FORMAT_YUY2;
	case Format::Y210: return DXGI_FORMAT_Y210;
	case Format::Y216: return DXGI_FORMAT_Y216;
	case Format::NV11: return DXGI_FORMAT_NV11;
	case Format::AI44: return DXGI_FORMAT_AI44;
	case Format::IA44: return DXGI_FORMAT_IA44;
	case Format::P8: return DXGI_FORMAT_P8;
	case Format::A8P8: return DXGI_FORMAT_A8P8;
	case Format::B4G4R4A4UNorm: return DXGI_FORMAT_B4G4R4A4_UNORM;
	}
	
	throw unify::Exception( "Invalid format!" );
}

template<>
me::render::Format::TYPE unify::Cast( DXGI_FORMAT format )
{
	using namespace me::render;
	switch( format )
	{
	case DXGI_FORMAT_UNKNOWN: return Format::Unknown;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS: return Format::R32G32B32A32_Typeless;
	case DXGI_FORMAT_R32G32B32A32_FLOAT: return Format::R32G32B32A32_Float;
	case DXGI_FORMAT_R32G32B32A32_UINT: return Format::R32G32B32A32_UInt;
	case DXGI_FORMAT_R32G32B32A32_SINT: return Format::R32G32B32A32_SInt;
	case DXGI_FORMAT_R32G32B32_TYPELESS: return Format::R32G32B32_Typeless;
	case DXGI_FORMAT_R32G32B32_FLOAT: return Format::R32G32B32_Float;
	case DXGI_FORMAT_R32G32B32_UINT: return Format::R32G32B32_UInt;
	case DXGI_FORMAT_R32G32B32_SINT: return Format::R32G32B32_SInt;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS: return Format::R16G16B16A16_Typeless;
	case DXGI_FORMAT_R16G16B16A16_FLOAT: return Format::R16G16B16A16_Float;
	case DXGI_FORMAT_R16G16B16A16_UNORM: return Format::R16G16B16A16_UNorm;
	case DXGI_FORMAT_R16G16B16A16_UINT: return Format::R16G16B16A16_UInt;
	case DXGI_FORMAT_R16G16B16A16_SNORM: return Format::R16G16B16A16_SNorm;
	case DXGI_FORMAT_R16G16B16A16_SINT: return Format::R16G16B16A16_SInt;
	case DXGI_FORMAT_R32G32_TYPELESS: return Format::R32G32_Typeless;
	case DXGI_FORMAT_R32G32_FLOAT: return Format::R32G32_Float;
	case DXGI_FORMAT_R32G32_UINT: return Format::R32G32_UInt;
	case DXGI_FORMAT_R32G32_SINT: return Format::R32G32_SInt;
	case DXGI_FORMAT_R32G8X24_TYPELESS: return Format::R32G8X24_Typeless;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return Format::D32_Float_S8X24_UInt;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return Format::R32_Float_X8X24_Typeless;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return Format::X32_Typeless_G8X24_UInt;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS: return Format::R10G10B10A2_Typeless;
	case DXGI_FORMAT_R10G10B10A2_UNORM: return Format::R10G10B10A2_UNorm;
	case DXGI_FORMAT_R10G10B10A2_UINT: return Format::R10G10B10A2_UInt;
	case DXGI_FORMAT_R11G11B10_FLOAT: return Format::R11G11B10_Float;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS: return Format::R8G8B8A8_Typeless;
	case DXGI_FORMAT_R8G8B8A8_UNORM: return Format::R8G8B8A8_UNorm;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return Format::R8G8B8A8_UNorm_SRGB;
	case DXGI_FORMAT_R8G8B8A8_UINT: return Format::R8G8B8A8_UInt;
	case DXGI_FORMAT_R8G8B8A8_SNORM: return Format::R8G8B8A8_SNorm;
	case DXGI_FORMAT_R8G8B8A8_SINT: return Format::R8G8B8A8_SInt;
	case DXGI_FORMAT_R16G16_TYPELESS: return Format::R16G16_Typeless;
	case DXGI_FORMAT_R16G16_FLOAT: return Format::R16G16_Float;
	case DXGI_FORMAT_R16G16_UNORM: return Format::R16G16_UNorm;
	case DXGI_FORMAT_R16G16_UINT: return Format::R16G16_UInt;
	case DXGI_FORMAT_R16G16_SNORM: return Format::R16G16_SNorm;
	case DXGI_FORMAT_R16G16_SINT: return Format::R16G16_SInt;
	case DXGI_FORMAT_R32_TYPELESS: return Format::R32_Typeless;
	case DXGI_FORMAT_D32_FLOAT: return Format::D32_Float;
	case DXGI_FORMAT_R32_FLOAT: return Format::R32_Float;
	case DXGI_FORMAT_R32_UINT: return Format::R32_UInt;
	case DXGI_FORMAT_R32_SINT: return Format::R32_SInt;
	case DXGI_FORMAT_R24G8_TYPELESS: return Format::R24G8_Typeless;
	case DXGI_FORMAT_D24_UNORM_S8_UINT: return Format::D24_UNorm_S8_UInt;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return Format::R24_UNorm_X8_Typeless;
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return Format::X24_Typeless_G8_UInt;
	case DXGI_FORMAT_R8G8_TYPELESS: return Format::R8G8_Typeless;
	case DXGI_FORMAT_R8G8_UNORM: return Format::R8G8_UNorm;
	case DXGI_FORMAT_R8G8_UINT: return Format::R8G8_UInt;
	case DXGI_FORMAT_R8G8_SNORM: return Format::R8G8_SNorm;
	case DXGI_FORMAT_R8G8_SINT: return Format::R8G8_SInt;
	case DXGI_FORMAT_R16_TYPELESS: return Format::R16_Typeless;
	case DXGI_FORMAT_R16_FLOAT: return Format::R16_Float;
	case DXGI_FORMAT_D16_UNORM: return Format::D16_UNorm;
	case DXGI_FORMAT_R16_UNORM: return Format::R16_UNorm;
	case DXGI_FORMAT_R16_UINT: return Format::R16_UInt;
	case DXGI_FORMAT_R16_SNORM: return Format::R16_SNorm;
	case DXGI_FORMAT_R16_SINT: return Format::R16_SInt;
	case DXGI_FORMAT_R8_TYPELESS: return Format::R8_Typeless;
	case DXGI_FORMAT_R8_UNORM: return Format::R8_UNorm;
	case DXGI_FORMAT_R8_UINT: return Format::R8_UInt;
	case DXGI_FORMAT_R8_SNORM: return Format::R8_SNorm;
	case DXGI_FORMAT_R8_SINT: return Format::R8_SInt;
	case DXGI_FORMAT_A8_UNORM: return Format::A8_UNorm;
	case DXGI_FORMAT_R1_UNORM: return Format::R1_UNorm;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return Format::R9G9B9E5_SharedExp;
	case DXGI_FORMAT_R8G8_B8G8_UNORM: return Format::R8G8_B8G8_UNorm;
	case DXGI_FORMAT_G8R8_G8B8_UNORM: return Format::G8R8_G8B8_UNorm;
	case DXGI_FORMAT_BC1_TYPELESS: return Format::BC1_Typeless;
	case DXGI_FORMAT_BC1_UNORM: return Format::BC1_UNorm;
	case DXGI_FORMAT_BC1_UNORM_SRGB: return Format::BC2_Typeless;
	case DXGI_FORMAT_BC2_TYPELESS: return Format::BC2_UNorm;
	case DXGI_FORMAT_BC2_UNORM: return Format::BC2_UNorm_SRGB;
	case DXGI_FORMAT_BC2_UNORM_SRGB: return Format::BC3_Typeless;
	case DXGI_FORMAT_BC3_TYPELESS: return Format::BC3_UNorm;
	case DXGI_FORMAT_BC3_UNORM: return Format::BC3_UNorm_SRGB;
	case DXGI_FORMAT_BC3_UNORM_SRGB: return Format::BC4_Typeless;
	case DXGI_FORMAT_BC4_TYPELESS: return Format::BC4_UNorm;
	case DXGI_FORMAT_BC4_UNORM: return Format::BC4_SNorm;
	case DXGI_FORMAT_BC4_SNORM: return Format::BC5_Typeless;
	case DXGI_FORMAT_BC5_TYPELESS: return Format::BC5_UNorm;
	case DXGI_FORMAT_BC5_UNORM: return Format::BC5_SNorm;
	case DXGI_FORMAT_BC5_SNORM: return Format::B5G6R5_UNorm;
	case DXGI_FORMAT_B5G6R5_UNORM: return Format::B5G5R5A1_UNorm;
	case DXGI_FORMAT_B5G5R5A1_UNORM: return Format::B8G8R8A8_UNorm;
	case DXGI_FORMAT_B8G8R8A8_UNORM: return Format::B8G8R8X8_UNorm;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return Format::R10G10B10_XR_BIAS_A2_UNorm;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS: return Format::B8G8R8A8Typeless;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return Format::B8G8R8A8UNormSRGB;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS: return Format::B8G8R8X8Typeless;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return Format::B8G8R8X8UNormSRGB;
	case DXGI_FORMAT_BC6H_TYPELESS: return Format::BC6H_Typeless;
	case DXGI_FORMAT_BC6H_UF16: return Format::BC6H_UF16;
	case DXGI_FORMAT_BC6H_SF16: return Format::BC6H_SF16;
	case DXGI_FORMAT_BC7_TYPELESS: return Format::BC7_Typless;
	case DXGI_FORMAT_BC7_UNORM: return Format::BC7_UNorm;
	case DXGI_FORMAT_BC7_UNORM_SRGB: return Format::BC7_UNormSRGB;
	case DXGI_FORMAT_AYUV: return Format::AYUV;
	case DXGI_FORMAT_Y410: return Format::Y410;
	case DXGI_FORMAT_Y416: return Format::Y416;
	case DXGI_FORMAT_NV12: return Format::NV12;
	case DXGI_FORMAT_P010: return Format::P010;
	case DXGI_FORMAT_P016: return Format::P016;
	case DXGI_FORMAT_420_OPAQUE: return Format::Opaque420;
	case DXGI_FORMAT_YUY2: return Format::YUY2;
	case DXGI_FORMAT_Y210: return Format::Y210;
	case DXGI_FORMAT_Y216: return Format::Y216;
	case DXGI_FORMAT_NV11: return Format::NV11;
	case DXGI_FORMAT_AI44: return Format::AI44;
	case DXGI_FORMAT_IA44: return Format::IA44;
	case DXGI_FORMAT_P8: return Format::P8;
	case DXGI_FORMAT_A8P8: return Format::A8P8;
	case DXGI_FORMAT_B4G4R4A4_UNORM: return Format::B4G4R4A4UNorm;
	}
	
	throw unify::Exception( "Invalid format!" );
}



template<> 
D3D11_USAGE unify::Cast( me::render::BufferUsage::TYPE usage )
{
	using namespace me::render;

	switch( usage )
	{
	case BufferUsage::Default: return D3D11_USAGE_DEFAULT;
	case BufferUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
	case BufferUsage::Dynamic: return D3D11_USAGE_DYNAMIC;
	case BufferUsage::Staging: return D3D11_USAGE_STAGING;
	}
	
	throw unify::Exception( "Invalid usage!" );
}

template<> 
me::render::BufferUsage::TYPE unify::Cast( D3D11_USAGE usage )
{
	using namespace me::render;

	switch( usage )
	{
	case D3D11_USAGE_DEFAULT: return BufferUsage::Default;
	case D3D11_USAGE_IMMUTABLE: return BufferUsage::Immutable;
	case D3D11_USAGE_DYNAMIC: return BufferUsage::Dynamic;
	case D3D11_USAGE_STAGING: return BufferUsage::Staging;
	}

	throw unify::Exception( "Invalid usage!" );
}
