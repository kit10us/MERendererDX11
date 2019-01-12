// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <medx11/VertexConstruct.h>
#include <me/exception/FailedToCreate.h>
#include <algorithm>

using namespace medx11;
using namespace me;
using namespace render;

std::vector< D3D11_INPUT_ELEMENT_DESC > ToDX( const VertexElement & element )
{
	D3D11_INPUT_ELEMENT_DESC out{};
	out.InputSlot = element.InputSlot;

	size_t count = 1;

	switch ( element.Format )
	{
	case ElementFormat::Float1: out.Format = DXGI_FORMAT_R32_FLOAT; break;
	case ElementFormat::Float2: out.Format = DXGI_FORMAT_R32G32_FLOAT; break;
	case ElementFormat::Float3: out.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
	case ElementFormat::Float4: out.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;

	case ElementFormat::Int1: out.Format = DXGI_FORMAT_R32_SINT; break;
	case ElementFormat::Int2: out.Format = DXGI_FORMAT_R32G32_SINT; break;
	case ElementFormat::Int3: out.Format = DXGI_FORMAT_R32G32B32_SINT; break;
	case ElementFormat::Int4: out.Format = DXGI_FORMAT_R32G32B32A32_SINT; break;

	case ElementFormat::Matrix4x4: out.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; count = 4; break;
	case ElementFormat::ColorUNorm: out.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
	case ElementFormat::Unknown: out.Format = DXGI_FORMAT_UNKNOWN; break;
	}

	out.SemanticName = element.SemanticName.c_str();
	out.SemanticIndex = element.SemanticIndex;
	out.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; // element.AlignedByteOffset;

	switch( element.SlotClass )
	{
	case SlotClass::Vertex:
		out.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		break;
	case SlotClass::Instance:
		out.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		break;
	default:
		throw unify::Exception( "Invalid slot class!" );
	}

	out.InstanceDataStepRate = element.InstanceDataStepRate;

	std::vector< D3D11_INPUT_ELEMENT_DESC > outVector( count );
	for( size_t i = 0; i < count; ++i )
	{
		outVector[ i ] = out;
		outVector[ i ].SemanticIndex += i;
	}

	return outVector;
}				   

VertexConstruct::VertexConstruct( IRenderer * renderer, const VertexDeclaration & vd, const IVertexShader & vs )
	: m_renderer( dynamic_cast< Renderer * >(renderer) )
{
	if ( vd.GetNumberOfElements() == 0 )
	{
		throw exception::FailedToCreate( "Failed to create vertex declaration, as is empty!" );
	}

	std::vector< D3D11_INPUT_ELEMENT_DESC > elements;
	for ( auto & e : vd.Elements() )
	{
		std::vector< D3D11_INPUT_ELEMENT_DESC > newElements = ToDX( e );

		for( auto && element : newElements )
		{
			elements.push_back( element );
		}
	}

	HRESULT hr = m_renderer->GetDxDevice()->CreateInputLayout( &elements[0], elements.size(), vs.GetBytecode(), vs.GetBytecodeLength(), &m_layout );
	if ( FAILED( hr ) )
	{
		throw unify::Exception( "Failed to create vertex declaration!" );
	}
}

VertexConstruct::~VertexConstruct()
{
	m_layout = nullptr;
}

void VertexConstruct::Use() const
{
	m_renderer->GetDxContext()->IASetInputLayout( m_layout );
}

