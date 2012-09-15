// This file has been generated by Py++.

#include "cbase.h"
#include "boost/python.hpp"
#include "__convenience.pypp.hpp"
#include "__call_policies.pypp.hpp"
#include "videocfg/videocfg.h"
#include "cbase.h"
#include "vgui_controls/Panel.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/AnalogBar.h"
#include "vgui_controls/Image.h"
#include "vgui_controls/TextImage.h"
#include "vgui_controls/ScrollBar.h"
#include "vgui_controls/ScrollBarSlider.h"
#include "vgui_controls/Menu.h"
#include "vgui_controls/MenuButton.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/RichText.h"
#include "vgui_controls/Tooltip.h"
#include "vgui/IBorder.h"
#include "vgui_bitmapimage.h"
#include "vgui_avatarimage.h"
#include "src_python_vgui.h"
#include "hl2wars/hl2wars_baseminimap.h"
#include "hl2wars/vgui_video_general.h"
#include "src_python.h"
#include "tier0/memdbgon.h"
#include "BitmapImage_pypp.hpp"

namespace bp = boost::python;

struct BitmapImage_wrapper : BitmapImage, bp::wrapper< BitmapImage > {

    BitmapImage_wrapper(BitmapImage const & arg )
    : BitmapImage( arg )
      , bp::wrapper< BitmapImage >(){
        // copy constructor
        
    }

    BitmapImage_wrapper( )
    : BitmapImage( )
      , bp::wrapper< BitmapImage >(){
        // null constructor
    
    }

    BitmapImage_wrapper(::vgui::VPANEL pPanelSize, char const * pFileName )
    : BitmapImage( pPanelSize, pFileName )
      , bp::wrapper< BitmapImage >(){
        // constructor
    
    }

    static boost::python::tuple GetColor_89096456469b8568582542035a4afba7( ::BitmapImage & inst ){
        int r2;
        int g2;
        int b2;
        int a2;
        inst.GetColor(r2, g2, b2, a2);
        return bp::make_tuple( r2, g2, b2, a2 );
    }

    static boost::python::tuple GetSize( ::BitmapImage & inst ){
        int wide2;
        int tall2;
        inst.GetSize(wide2, tall2);
        return bp::make_tuple( wide2, tall2 );
    }

    void DrawFilledRect( int x0, int y0, int x1, int y1 ){
        vgui::Image::DrawFilledRect( x0, y0, x1, y1 );
    }

    void DrawLine( int x0, int y0, int x1, int y1 ){
        vgui::Image::DrawLine( x0, y0, x1, y1 );
    }

    void DrawOutlinedRect( int x0, int y0, int x1, int y1 ){
        vgui::Image::DrawOutlinedRect( x0, y0, x1, y1 );
    }

    void DrawPolyLine( int * px, int * py, int numPoints ){
        vgui::Image::DrawPolyLine( px, py, numPoints );
    }

    void DrawPrintChar( wchar_t ch ){
        vgui::Image::DrawPrintChar( ch );
    }

    void DrawPrintChar( int x, int y, wchar_t ch ){
        vgui::Image::DrawPrintChar( x, y, ch );
    }

    void DrawPrintText( wchar_t const * str, int strlen ){
        vgui::Image::DrawPrintText( str, strlen );
    }

    void DrawPrintText( int x, int y, wchar_t const * str, int strlen ){
        vgui::Image::DrawPrintText( x, y, str, strlen );
    }

    void DrawSetColor( ::Color color ){
        vgui::Image::DrawSetColor( color );
    }

    void DrawSetColor( int r, int g, int b, int a ){
        vgui::Image::DrawSetColor( r, g, b, a );
    }

    void DrawSetTextColor( ::Color color ){
        vgui::Image::DrawSetTextColor( color );
    }

    void DrawSetTextColor( int r, int g, int b, int a ){
        vgui::Image::DrawSetTextColor( r, g, b, a );
    }

    void DrawSetTextFont( ::vgui::HFont font ){
        vgui::Image::DrawSetTextFont( font );
    }

    void DrawSetTextPos( int x, int y ){
        vgui::Image::DrawSetTextPos( x, y );
    }

    void DrawSetTexture( int id ){
        vgui::Image::DrawSetTexture( id );
    }

    void DrawTexturedRect( int x0, int y0, int x1, int y1 ){
        vgui::Image::DrawTexturedRect( x0, y0, x1, y1 );
    }

    virtual bool Evict(  ) {
        #if defined(_WIN32)
        #if defined(_DEBUG)
        Assert( SrcPySystem()->IsPythonRunning() );
        Assert( GetCurrentThreadId() == g_hPythonThreadID );
        #elif defined(PY_CHECKTHREADID)
        if( GetCurrentThreadId() != g_hPythonThreadID )
            Error( "Evict: Client? %d. Thread ID is not the same as in which the python interpreter is initialized! %d != %d. Tell a developer.\n", CBaseEntity::IsClient(), g_hPythonThreadID, GetCurrentThreadId() );
        #endif // _DEBUG/PY_CHECKTHREADID
        #endif // _WIN32
        #if defined(_DEBUG) || defined(PY_CHECK_LOG_OVERRIDES)
        if( py_log_overrides.GetBool() )
            Msg("Calling Evict(  ) of Class: vgui::Image\n");
        #endif // _DEBUG/PY_CHECK_LOG_OVERRIDES
        bp::override func_Evict = this->get_override( "Evict" );
        if( func_Evict.ptr() != Py_None )
            try {
                return func_Evict(  );
            } catch(bp::error_already_set &) {
                PyErr_Print();
                return this->vgui::Image::Evict(  );
            }
        else
            return this->vgui::Image::Evict(  );
    }
    
    bool default_Evict(  ) {
        return vgui::Image::Evict( );
    }

    virtual void GetContentSize( int & wide, int & tall ) {
        namespace bpl = boost::python;
        bpl::override func_GetContentSize = this->get_override( "GetContentSize" );
        if( func_GetContentSize.ptr() != Py_None ) {
            bpl::object py_result = bpl::call<bpl::object>( func_GetContentSize.ptr() );
            wide = boost::python::extract< int >( pyplus_conv::get_out_argument( py_result, "wide" ) );
            tall = boost::python::extract< int >( pyplus_conv::get_out_argument( py_result, "tall" ) );
        }
        else{
            vgui::Image::GetContentSize( wide, tall );
        }
    }
    
    static boost::python::tuple default_GetContentSize( ::vgui::Image & inst ){
        int wide2;
        int tall2;
        if( dynamic_cast< BitmapImage_wrapper * >( boost::addressof( inst ) ) ){
            inst.::vgui::Image::GetContentSize(wide2, tall2);
        }
        else{
            inst.GetContentSize(wide2, tall2);
        }
        return bp::make_tuple( wide2, tall2 );
    }

    virtual ::vgui::HTexture GetID(  ) {
        #if defined(_WIN32)
        #if defined(_DEBUG)
        Assert( SrcPySystem()->IsPythonRunning() );
        Assert( GetCurrentThreadId() == g_hPythonThreadID );
        #elif defined(PY_CHECKTHREADID)
        if( GetCurrentThreadId() != g_hPythonThreadID )
            Error( "GetID: Client? %d. Thread ID is not the same as in which the python interpreter is initialized! %d != %d. Tell a developer.\n", CBaseEntity::IsClient(), g_hPythonThreadID, GetCurrentThreadId() );
        #endif // _DEBUG/PY_CHECKTHREADID
        #endif // _WIN32
        #if defined(_DEBUG) || defined(PY_CHECK_LOG_OVERRIDES)
        if( py_log_overrides.GetBool() )
            Msg("Calling GetID(  ) of Class: vgui::Image\n");
        #endif // _DEBUG/PY_CHECK_LOG_OVERRIDES
        bp::override func_GetID = this->get_override( "GetID" );
        if( func_GetID.ptr() != Py_None )
            try {
                return func_GetID(  );
            } catch(bp::error_already_set &) {
                PyErr_Print();
                return this->vgui::Image::GetID(  );
            }
        else
            return this->vgui::Image::GetID(  );
    }
    
    ::vgui::HTexture default_GetID(  ) {
        return vgui::Image::GetID( );
    }

    virtual int GetNumFrames(  ) {
        #if defined(_WIN32)
        #if defined(_DEBUG)
        Assert( SrcPySystem()->IsPythonRunning() );
        Assert( GetCurrentThreadId() == g_hPythonThreadID );
        #elif defined(PY_CHECKTHREADID)
        if( GetCurrentThreadId() != g_hPythonThreadID )
            Error( "GetNumFrames: Client? %d. Thread ID is not the same as in which the python interpreter is initialized! %d != %d. Tell a developer.\n", CBaseEntity::IsClient(), g_hPythonThreadID, GetCurrentThreadId() );
        #endif // _DEBUG/PY_CHECKTHREADID
        #endif // _WIN32
        #if defined(_DEBUG) || defined(PY_CHECK_LOG_OVERRIDES)
        if( py_log_overrides.GetBool() )
            Msg("Calling GetNumFrames(  ) of Class: vgui::Image\n");
        #endif // _DEBUG/PY_CHECK_LOG_OVERRIDES
        bp::override func_GetNumFrames = this->get_override( "GetNumFrames" );
        if( func_GetNumFrames.ptr() != Py_None )
            try {
                return func_GetNumFrames(  );
            } catch(bp::error_already_set &) {
                PyErr_Print();
                return this->vgui::Image::GetNumFrames(  );
            }
        else
            return this->vgui::Image::GetNumFrames(  );
    }
    
    int default_GetNumFrames(  ) {
        return vgui::Image::GetNumFrames( );
    }

    virtual void GetPos( int & x, int & y ) {
        namespace bpl = boost::python;
        bpl::override func_GetPos = this->get_override( "GetPos" );
        if( func_GetPos.ptr() != Py_None ) {
            bpl::object py_result = bpl::call<bpl::object>( func_GetPos.ptr() );
            x = boost::python::extract< int >( pyplus_conv::get_out_argument( py_result, "x" ) );
            y = boost::python::extract< int >( pyplus_conv::get_out_argument( py_result, "y" ) );
        }
        else{
            vgui::Image::GetPos( x, y );
        }
    }
    
    static boost::python::tuple default_GetPos( ::vgui::Image & inst ){
        int x2;
        int y2;
        if( dynamic_cast< BitmapImage_wrapper * >( boost::addressof( inst ) ) ){
            inst.::vgui::Image::GetPos(x2, y2);
        }
        else{
            inst.GetPos(x2, y2);
        }
        return bp::make_tuple( x2, y2 );
    }

    virtual int GetTall(  ) {
        #if defined(_WIN32)
        #if defined(_DEBUG)
        Assert( SrcPySystem()->IsPythonRunning() );
        Assert( GetCurrentThreadId() == g_hPythonThreadID );
        #elif defined(PY_CHECKTHREADID)
        if( GetCurrentThreadId() != g_hPythonThreadID )
            Error( "GetTall: Client? %d. Thread ID is not the same as in which the python interpreter is initialized! %d != %d. Tell a developer.\n", CBaseEntity::IsClient(), g_hPythonThreadID, GetCurrentThreadId() );
        #endif // _DEBUG/PY_CHECKTHREADID
        #endif // _WIN32
        #if defined(_DEBUG) || defined(PY_CHECK_LOG_OVERRIDES)
        if( py_log_overrides.GetBool() )
            Msg("Calling GetTall(  ) of Class: vgui::Image\n");
        #endif // _DEBUG/PY_CHECK_LOG_OVERRIDES
        bp::override func_GetTall = this->get_override( "GetTall" );
        if( func_GetTall.ptr() != Py_None )
            try {
                return func_GetTall(  );
            } catch(bp::error_already_set &) {
                PyErr_Print();
                return this->vgui::Image::GetTall(  );
            }
        else
            return this->vgui::Image::GetTall(  );
    }
    
    int default_GetTall(  ) {
        return vgui::Image::GetTall( );
    }

    virtual int GetWide(  ) {
        #if defined(_WIN32)
        #if defined(_DEBUG)
        Assert( SrcPySystem()->IsPythonRunning() );
        Assert( GetCurrentThreadId() == g_hPythonThreadID );
        #elif defined(PY_CHECKTHREADID)
        if( GetCurrentThreadId() != g_hPythonThreadID )
            Error( "GetWide: Client? %d. Thread ID is not the same as in which the python interpreter is initialized! %d != %d. Tell a developer.\n", CBaseEntity::IsClient(), g_hPythonThreadID, GetCurrentThreadId() );
        #endif // _DEBUG/PY_CHECKTHREADID
        #endif // _WIN32
        #if defined(_DEBUG) || defined(PY_CHECK_LOG_OVERRIDES)
        if( py_log_overrides.GetBool() )
            Msg("Calling GetWide(  ) of Class: vgui::Image\n");
        #endif // _DEBUG/PY_CHECK_LOG_OVERRIDES
        bp::override func_GetWide = this->get_override( "GetWide" );
        if( func_GetWide.ptr() != Py_None )
            try {
                return func_GetWide(  );
            } catch(bp::error_already_set &) {
                PyErr_Print();
                return this->vgui::Image::GetWide(  );
            }
        else
            return this->vgui::Image::GetWide(  );
    }
    
    int default_GetWide(  ) {
        return vgui::Image::GetWide( );
    }

    virtual void SetFrame( int nFrame ) {
        #if defined(_WIN32)
        #if defined(_DEBUG)
        Assert( SrcPySystem()->IsPythonRunning() );
        Assert( GetCurrentThreadId() == g_hPythonThreadID );
        #elif defined(PY_CHECKTHREADID)
        if( GetCurrentThreadId() != g_hPythonThreadID )
            Error( "SetFrame: Client? %d. Thread ID is not the same as in which the python interpreter is initialized! %d != %d. Tell a developer.\n", CBaseEntity::IsClient(), g_hPythonThreadID, GetCurrentThreadId() );
        #endif // _DEBUG/PY_CHECKTHREADID
        #endif // _WIN32
        #if defined(_DEBUG) || defined(PY_CHECK_LOG_OVERRIDES)
        if( py_log_overrides.GetBool() )
            Msg("Calling SetFrame( nFrame ) of Class: vgui::Image\n");
        #endif // _DEBUG/PY_CHECK_LOG_OVERRIDES
        bp::override func_SetFrame = this->get_override( "SetFrame" );
        if( func_SetFrame.ptr() != Py_None )
            try {
                func_SetFrame( nFrame );
            } catch(bp::error_already_set &) {
                PyErr_Print();
                this->vgui::Image::SetFrame( nFrame );
            }
        else
            this->vgui::Image::SetFrame( nFrame );
    }
    
    void default_SetFrame( int nFrame ) {
        vgui::Image::SetFrame( nFrame );
    }

    virtual void SetRotation( int iRotation ) {
        #if defined(_WIN32)
        #if defined(_DEBUG)
        Assert( SrcPySystem()->IsPythonRunning() );
        Assert( GetCurrentThreadId() == g_hPythonThreadID );
        #elif defined(PY_CHECKTHREADID)
        if( GetCurrentThreadId() != g_hPythonThreadID )
            Error( "SetRotation: Client? %d. Thread ID is not the same as in which the python interpreter is initialized! %d != %d. Tell a developer.\n", CBaseEntity::IsClient(), g_hPythonThreadID, GetCurrentThreadId() );
        #endif // _DEBUG/PY_CHECKTHREADID
        #endif // _WIN32
        #if defined(_DEBUG) || defined(PY_CHECK_LOG_OVERRIDES)
        if( py_log_overrides.GetBool() )
            Msg("Calling SetRotation( iRotation ) of Class: vgui::Image\n");
        #endif // _DEBUG/PY_CHECK_LOG_OVERRIDES
        bp::override func_SetRotation = this->get_override( "SetRotation" );
        if( func_SetRotation.ptr() != Py_None )
            try {
                func_SetRotation( iRotation );
            } catch(bp::error_already_set &) {
                PyErr_Print();
                this->vgui::Image::SetRotation( iRotation );
            }
        else
            this->vgui::Image::SetRotation( iRotation );
    }
    
    void default_SetRotation( int iRotation ) {
        vgui::Image::SetRotation( iRotation );
    }

    void SetSize( int wide, int tall ){
        vgui::Image::SetSize( wide, tall );
    }

};

void register_BitmapImage_class(){

    bp::class_< BitmapImage_wrapper, bp::bases< vgui::Image > >( "BitmapImage", bp::init< >() )    
        .def( bp::init< vgui::VPANEL, char const * >(( bp::arg("pPanelSize"), bp::arg("pFileName") )) )    
        .def( 
            "DoPaint"
            , (void ( ::BitmapImage::* )( ::vgui::VPANEL,float,float ) )( &::BitmapImage::DoPaint )
            , ( bp::arg("panel"), bp::arg("yaw")=0, bp::arg("flAlphaModulate")=1.0e+0f ) )    
        .def( 
            "DoPaint"
            , (void ( ::BitmapImage::* )( int,int,int,int,float,float ) )( &::BitmapImage::DoPaint )
            , ( bp::arg("x"), bp::arg("y"), bp::arg("w"), bp::arg("h"), bp::arg("yaw")=0, bp::arg("flAlphaModulate")=1.0e+0f ) )    
        .def( 
            "GetColor"
            , (::Color ( ::BitmapImage::* )(  ) )( &::BitmapImage::GetColor ) )    
        .def( 
            "GetColor_89096456469b8568582542035a4afba7"
            , (boost::python::tuple (*)( ::BitmapImage & ))( &BitmapImage_wrapper::GetColor_89096456469b8568582542035a4afba7 )
            , ( bp::arg("inst") ) )    
        .def( 
            "GetRenderSizePanel"
            , (::vgui::VPANEL ( ::BitmapImage::* )(  ) )( &::BitmapImage::GetRenderSizePanel ) )    
        .def( 
            "GetSize"
            , (boost::python::tuple (*)( ::BitmapImage & ))( &BitmapImage_wrapper::GetSize )
            , ( bp::arg("inst") ) )    
        .def( 
            "Init"
            , (bool ( ::BitmapImage::* )( ::vgui::VPANEL,char const * ) )( &::BitmapImage::Init )
            , ( bp::arg("pParent"), bp::arg("pFileName") ) )    
        .def( 
            "Init"
            , (bool ( ::BitmapImage::* )( ::vgui::VPANEL,::KeyValues * ) )( &::BitmapImage::Init )
            , ( bp::arg("pParent"), bp::arg("pInitData") ) )    
        .def( 
            "Paint"
            , (void ( ::BitmapImage::* )(  ) )( &::BitmapImage::Paint ) )    
        .def( 
            "SetColor"
            , (void ( ::BitmapImage::* )( ::Color const & ) )( &::BitmapImage::SetColor )
            , ( bp::arg("clr") ) )    
        .def( 
            "SetImageFile"
            , (void ( ::BitmapImage::* )( char const * ) )( &::BitmapImage::SetImageFile )
            , ( bp::arg("newImage") ) )    
        .def( 
            "SetPos"
            , (void ( ::BitmapImage::* )( int,int ) )( &::BitmapImage::SetPos )
            , ( bp::arg("x"), bp::arg("y") ) )    
        .def( 
            "SetRenderSize"
            , (void ( ::BitmapImage::* )( int,int ) )( &::BitmapImage::SetRenderSize )
            , ( bp::arg("x"), bp::arg("y") ) )    
        .def( 
            "SetViewport"
            , (void ( ::BitmapImage::* )( bool,float,float,float,float ) )( &::BitmapImage::SetViewport )
            , ( bp::arg("use"), bp::arg("left"), bp::arg("top"), bp::arg("right"), bp::arg("bottom") ) )    
        .def( 
            "UsePanelRenderSize"
            , (void ( ::BitmapImage::* )( ::vgui::VPANEL ) )( &::BitmapImage::UsePanelRenderSize )
            , ( bp::arg("pPanel") ) )    
        .def( 
            "DrawFilledRect"
            , (void ( BitmapImage_wrapper::* )( int,int,int,int ) )(&BitmapImage_wrapper::DrawFilledRect)
            , ( bp::arg("x0"), bp::arg("y0"), bp::arg("x1"), bp::arg("y1") ) )    
        .def( 
            "DrawLine"
            , (void ( BitmapImage_wrapper::* )( int,int,int,int ) )(&BitmapImage_wrapper::DrawLine)
            , ( bp::arg("x0"), bp::arg("y0"), bp::arg("x1"), bp::arg("y1") ) )    
        .def( 
            "DrawOutlinedRect"
            , (void ( BitmapImage_wrapper::* )( int,int,int,int ) )(&BitmapImage_wrapper::DrawOutlinedRect)
            , ( bp::arg("x0"), bp::arg("y0"), bp::arg("x1"), bp::arg("y1") ) )    
        .def( 
            "DrawPolyLine"
            , (void ( BitmapImage_wrapper::* )( int *,int *,int ) )(&BitmapImage_wrapper::DrawPolyLine)
            , ( bp::arg("px"), bp::arg("py"), bp::arg("numPoints") ) )    
        .def( 
            "DrawPrintChar"
            , (void ( BitmapImage_wrapper::* )( wchar_t ) )(&BitmapImage_wrapper::DrawPrintChar)
            , ( bp::arg("ch") ) )    
        .def( 
            "DrawPrintChar"
            , (void ( BitmapImage_wrapper::* )( int,int,wchar_t ) )(&BitmapImage_wrapper::DrawPrintChar)
            , ( bp::arg("x"), bp::arg("y"), bp::arg("ch") ) )    
        .def( 
            "DrawPrintText"
            , (void ( BitmapImage_wrapper::* )( wchar_t const *,int ) )(&BitmapImage_wrapper::DrawPrintText)
            , ( bp::arg("str"), bp::arg("strlen") ) )    
        .def( 
            "DrawPrintText"
            , (void ( BitmapImage_wrapper::* )( int,int,wchar_t const *,int ) )(&BitmapImage_wrapper::DrawPrintText)
            , ( bp::arg("x"), bp::arg("y"), bp::arg("str"), bp::arg("strlen") ) )    
        .def( 
            "DrawSetColor"
            , (void ( BitmapImage_wrapper::* )( ::Color ) )(&BitmapImage_wrapper::DrawSetColor)
            , ( bp::arg("color") ) )    
        .def( 
            "DrawSetColor"
            , (void ( BitmapImage_wrapper::* )( int,int,int,int ) )(&BitmapImage_wrapper::DrawSetColor)
            , ( bp::arg("r"), bp::arg("g"), bp::arg("b"), bp::arg("a") ) )    
        .def( 
            "DrawSetTextColor"
            , (void ( BitmapImage_wrapper::* )( ::Color ) )(&BitmapImage_wrapper::DrawSetTextColor)
            , ( bp::arg("color") ) )    
        .def( 
            "DrawSetTextColor"
            , (void ( BitmapImage_wrapper::* )( int,int,int,int ) )(&BitmapImage_wrapper::DrawSetTextColor)
            , ( bp::arg("r"), bp::arg("g"), bp::arg("b"), bp::arg("a") ) )    
        .def( 
            "DrawSetTextFont"
            , (void ( BitmapImage_wrapper::* )( ::vgui::HFont ) )(&BitmapImage_wrapper::DrawSetTextFont)
            , ( bp::arg("font") ) )    
        .def( 
            "DrawSetTextPos"
            , (void ( BitmapImage_wrapper::* )( int,int ) )(&BitmapImage_wrapper::DrawSetTextPos)
            , ( bp::arg("x"), bp::arg("y") ) )    
        .def( 
            "DrawSetTexture"
            , (void ( BitmapImage_wrapper::* )( int ) )(&BitmapImage_wrapper::DrawSetTexture)
            , ( bp::arg("id") ) )    
        .def( 
            "DrawTexturedRect"
            , (void ( BitmapImage_wrapper::* )( int,int,int,int ) )(&BitmapImage_wrapper::DrawTexturedRect)
            , ( bp::arg("x0"), bp::arg("y0"), bp::arg("x1"), bp::arg("y1") ) )    
        .def( 
            "Evict"
            , (bool ( ::vgui::Image::* )(  ) )(&::vgui::Image::Evict)
            , (bool ( BitmapImage_wrapper::* )(  ) )(&BitmapImage_wrapper::default_Evict) )    
        .def( 
            "GetContentSize"
            , (boost::python::tuple (*)( ::vgui::Image & ))( &BitmapImage_wrapper::default_GetContentSize )
            , ( bp::arg("inst") ) )    
        .def( 
            "GetID"
            , (::vgui::HTexture ( ::vgui::Image::* )(  ) )(&::vgui::Image::GetID)
            , (::vgui::HTexture ( BitmapImage_wrapper::* )(  ) )(&BitmapImage_wrapper::default_GetID) )    
        .def( 
            "GetNumFrames"
            , (int ( ::vgui::Image::* )(  ) )(&::vgui::Image::GetNumFrames)
            , (int ( BitmapImage_wrapper::* )(  ) )(&BitmapImage_wrapper::default_GetNumFrames) )    
        .def( 
            "GetPos"
            , (boost::python::tuple (*)( ::vgui::Image & ))( &BitmapImage_wrapper::default_GetPos )
            , ( bp::arg("inst") ) )    
        .def( 
            "GetTall"
            , (int ( ::vgui::Image::* )(  ) )(&::vgui::Image::GetTall)
            , (int ( BitmapImage_wrapper::* )(  ) )(&BitmapImage_wrapper::default_GetTall) )    
        .def( 
            "GetWide"
            , (int ( ::vgui::Image::* )(  ) )(&::vgui::Image::GetWide)
            , (int ( BitmapImage_wrapper::* )(  ) )(&BitmapImage_wrapper::default_GetWide) )    
        .def( 
            "SetFrame"
            , (void ( ::vgui::Image::* )( int ) )(&::vgui::Image::SetFrame)
            , (void ( BitmapImage_wrapper::* )( int ) )(&BitmapImage_wrapper::default_SetFrame)
            , ( bp::arg("nFrame") ) )    
        .def( 
            "SetRotation"
            , (void ( ::vgui::Image::* )( int ) )(&::vgui::Image::SetRotation)
            , (void ( BitmapImage_wrapper::* )( int ) )(&BitmapImage_wrapper::default_SetRotation)
            , ( bp::arg("iRotation") ) )    
        .def( 
            "SetSize"
            , (void ( BitmapImage_wrapper::* )( int,int ) )(&BitmapImage_wrapper::SetSize)
            , ( bp::arg("wide"), bp::arg("tall") ) );

}
