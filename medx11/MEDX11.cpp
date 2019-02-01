// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <mewos/WindowsOS.h>
#include <medx11/RendererFactory.h>
#include <memory.h>
#include <me/game/IGame.h>

void Deleter( mewos::WindowsOS * factory )
{
	delete factory;
}

extern "C" __declspec(dllexport) bool MELoader( me::game::IGame * gameInstance, const qxml::Element * element );

__declspec(dllexport) bool MELoader( me::game::IGame * gameInstance, const qxml::Element * element )
{
	auto os = new mewos::WindowsOS( dynamic_cast< me::os::DefaultOS & >(*gameInstance->GetOS()), element, *gameInstance->GetOS()->GetOSParameters() );

	os->SetRenderFactory( me::render::IRendererFactory::ptr{ new medx11::RendererFactory } );

	gameInstance->SetOS( me::os::IOS::ptr( os ) );


	return true;
}

