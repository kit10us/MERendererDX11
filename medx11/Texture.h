// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#pragma warning( push )
#pragma warning( disable: 4005 ) // warning C4005: 'MAKEFOURCC': macro redefinition
#include <medx11/Renderer.h>
#pragma warning( pop )

#include <me/render/TextureLock.h>
#include <unify/Rect.h>
#include <unify/Color.h>
#include <unify/Path.h>

#include <string>
#include <memory>

#include <atlbase.h>
#include <DirectXTex.h>


// MS agressive macros.
#ifdef LoadImage
#undef LoadImage
#endif

#ifdef min
#undef min
#endif

namespace medx11
{									  
	//////////////////////////////////
	// Font definitions...
	//////////////////////////////////

#define FONT_BOLD        0x0001
#define FONT_ITALIC      0x0002
#define FONT_ZENABLE     0x0004

	// Font rendering flags
#define FONT_CENTERED_X  0x0001
#define FONT_CENTERED_Y  0x0002
#define FONT_TWOSIDED    0x0004
#define FONT_FILTERED    0x0008

#define SPRITEANIMLOOP_NONE			0
#define SPRITEANIMLOOP_REPEAT		1	// 1 loop period
#define SPRITEANIMLOOP_FORWARDBACK	2	// 2 loop periods

	class Texture : public me::render::ITexture
	{
		friend class Renderer;

	public:
		static bool s_allowTextureUses;

		Texture( me::render::IRenderer * renderer, me::render::TextureParameters parameters = me::render::TextureParameters() );
		virtual ~Texture();

		// ::Resource...
		void Preload();
		void Create();
		void Destroy();

		const unsigned int FileWidth() const;
	
		const unsigned int FileHeight() const;

	public: // me::render::ITexture
		
		const unify::Size< unsigned int > & ImageSize() const override;

		me::render::TextureLockAccess GetLockAccess() const override;

		void LockRect( unsigned int level, me::render::TextureLock & lock, const unify::Rect< long > * rect, unify::DataLockAccess::TYPE access );
		
		void UnlockRect( unsigned int level );

		me::render::SpriteDictionary & GetSpriteDictionary() override;
		
		const me::render::SpriteDictionary & GetSpriteDictionary() const override;

		const me::render::TextureParameters * GetParameters() const override;

	public: // rm::IResource...

		bool Reload() override;

		std::string GetSource() const override;

	protected:
		/// <summary>
		/// Create an image from a specific size.
		/// </summary>
		void CreateFromSize();

		/// <summary>
		/// Load an image from a path.
		/// </summary>
		void LoadImage( unify::Path filePath );

		/// <summary>
		/// Load a header for the image.
		/// </summar>
		void LoadHeader();

		const Renderer * m_renderer;						   
		CComPtr< ID3D11Texture2D > m_texture;
		CComPtr< ID3D11SamplerState > m_colorMapSampler;
		CComPtr< ID3D11ShaderResourceView > m_colorMap;
		bool m_created;
		unify::Size< unsigned int > m_fileSize;
		unify::Size< unsigned int > m_imageSize;
		bool m_useColorKey;
		unify::Color m_colorKey;
		me::render::TextureParameters m_parameters;
		DirectX::ScratchImage m_scratch;	
		me::render::SpriteDictionary m_spriteDictionary;
	};
}