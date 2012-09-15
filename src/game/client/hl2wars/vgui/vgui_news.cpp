//====== Copyright 20xx, Sander Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "vgui_news.h"
#include "wars_plat_misc.h"

#include "vgui/ISurface.h"
#include <vgui/IVGui.h>

#include <Awesomium/WebCore.h>
#include "src_python.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

WebNews::WebNews( vgui::Panel *pParent ) : WebView( false, pParent )
{
	GetWebView()->set_load_listener( this );
	GetWebView()->set_view_listener( this );
	//GetWebView()->set_process_listener( this );

	LoadFile( "ui/mainmenu.html" );

	SetZPos( -1 );

	SetVisible( false );
	SetMouseInputEnabled( true );
	//SetUseMouseCapture( false );

	m_CurVersion = CREATE_WEBSTRING( SrcPySystem()->RunT< const char * >( SrcPySystem()->Get("_GetVersion", "srcmgr"), "" ) );

	Awesomium::WebString interfacename = CREATE_WEBSTRING( "interface" );
	m_JSInterface = GetWebView()->CreateGlobalJavascriptObject( interfacename ).ToObject();

	m_LaunchUpdaterMethodName = CREATE_WEBSTRING("launchupdater");
	m_HasDesuraMethodName = CREATE_WEBSTRING("hasdesura");
	m_GetVersionMethodName = CREATE_WEBSTRING("getversion");
	m_JSInterface.SetCustomMethod( m_LaunchUpdaterMethodName, false );
	m_JSInterface.SetCustomMethod( m_HasDesuraMethodName, true );
	m_JSInterface.SetCustomMethod( m_GetVersionMethodName, true );
}

void WebNews::OnMethodCall(Awesomium::WebView* caller,
						unsigned int remote_object_id, 
						const Awesomium::WebString& method_name,
						const Awesomium::JSArray& args)
{
	if( method_name.Compare( m_LaunchUpdaterMethodName ) == 0 )
	{
		Msg("Launching updater\n");


		if( SrcHasProtocolHandler( "desura" ) )
		{
			SrcShellExecute( "desura://launch/mods/half-life-2-wars" );
			//SrcShellExecute( "desura://install/mods/half-life-2-wars" );
			//SrcShellExecute( "desura://refresh" );
			//SrcShellExecute( "desura://update//mods/half-life-2-wars" );
		}
		else
		{
			SrcShellExecute( "http://www.desura.com/" );
		}

		engine->ClientCmd( "exit" );
	}
}

Awesomium::JSValue WebNews::OnMethodCallWithReturnValue(Awesomium::WebView* caller,
											unsigned int remote_object_id,
											const Awesomium::WebString& method_name,
											const Awesomium::JSArray& args)
{
	if( method_name.Compare( m_HasDesuraMethodName ) == 0 )
	{
		return Awesomium::JSValue( SrcHasProtocolHandler( "desura" ) );
	}
	else if( method_name.Compare( m_GetVersionMethodName ) == 0 )
	{
		return Awesomium::JSValue( m_CurVersion );
	}

	return Awesomium::JSValue();
}

void WebNews::PerformLayout()
{
	//vgui::IScheme *pScheme = vgui::scheme()->GetIScheme( GetPanel()->GetScheme() );

	int screenWide, screenTall;
	vgui::surface()->GetScreenSize( screenWide, screenTall );

	int insetx = vgui::scheme()->GetProportionalScaledValue( 200 );
	int insety = vgui::scheme()->GetProportionalScaledValue( 20 );
	SetPos( insetx, insety );
	SetSize( screenWide - insetx - insety, screenTall - insety * 2 );
}

void WebNews::OnDocumentReady(Awesomium::WebView* caller,
							const Awesomium::WebURL& url)
{
	SetVisible( true );
	SetTransparent( true );
}