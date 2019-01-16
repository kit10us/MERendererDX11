// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved
//

#include <medx11/ShaderResource.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToLock.h>
#include <qxml/Document.h>

using namespace medx11;
using namespace me;

ShaderResource::ShaderResource( me::render::IRenderer * renderer, me::render::ShaderResourceParameters parameters )
	: m_renderer{ dynamic_cast< const Renderer *>(renderer) }
	, m_parameters( parameters )
{
}

ShaderResource::~ShaderResource()
{
	Destroy();
}

void ShaderResource::Create()
{
	auto dxDevice = m_renderer->GetDxDevice();
	auto dxContext = m_renderer->GetDxContext();

	// Release any previous texture
	Destroy();

	HRESULT result = S_OK;
	
	D3D11_BUFFER_DESC desc {};
	desc.Usage = unify::Cast< D3D11_USAGE >( m_parameters.usage );
	
	D3D11_SUBRESOURCE_DATA initialData{};
	HRESULT result = dxDevice->CreateBuffer( &desc, nullptr, &m_buffer );
	if ( FAILED( result ) )
	{
		throw std::exception( "Failed to create shader resource buffer!" );
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc{};
	resourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;

	result = dxDevice->CreateShaderResourceView( m_resource, &resourceViewDesc, &m_resourceView );
	assert( !FAILED( result ) );
}

// Destroyes data, keeps header intact - thus no graphical footprint, yet we can still use it's statistics/dimensions in calculations.
void ShaderResource::Destroy()
{
	m_resource.Release();
}

void ShaderResource::Use() const
{
	auto dxContext = m_renderer->GetDxContext();

	ID3D11ShaderResourceView** views = new ID3D11ShaderResourceView*[1];

	dxContext->VSSetShaderResources( 0, 1, views );
}

void ShaderResource::Lock( unsigned int level, unify::DataLock & lock, unify::DataLockAccess::TYPE type )
{
	auto dxContext = m_renderer->GetDxContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	auto result = dxContext->Map( m_resource, 0, unify::DataLockAccess::WriteAccess( type ) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_READ, 0, &mappedResource );
	if( FAILED( result ) )
	{
		throw unify::Exception( "Failed to lock texture!" );
	}

	lock.SetLock( mappedResource.pData, mappedResource.
	lock.uWidth = m_imageSize.width;
	lock.uHeight = m_imageSize.height;
	lock.uStride = mappedResource.RowPitch;
}

void ShaderResource::Unlock( unsigned int level )
{
	auto dxContext = m_renderer->GetDxContext();
	dxContext->Unmap( m_resource, 0 );
}

bool ShaderResource::Reload()
{
	Destroy();
	Create();
	return true;
}

std::string ShaderResource::GetSource() const
{
	return {};
}