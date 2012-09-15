// This file has been generated by Py++.

#include "cbase.h"
// This file has been generated by Py++.

#include "boost/python.hpp"
#include "videocfg/videocfg.h"
#include "cbase.h"
#include "takedamageinfo.h"
#include "c_baseanimating.h"
#include "c_baseanimatingoverlay.h"
#include "c_baseflex.h"
#include "c_basecombatcharacter.h"
#include "basegrenade_shared.h"
#include "c_baseplayer.h"
#include "c_hl2wars_player.h"
#include "unit_base_shared.h"
#include "wars_func_unit.h"
#include "c_playerresource.h"
#include "sprite.h"
#include "SpriteTrail.h"
#include "c_smoke_trail.h"
#include "c_wars_weapon.h"
#include "tempent.h"
#include "shared_classnames.h"
#include "npcevent.h"
#include "studio.h"
#include "src_python_entities.h"
#include "isaverestore.h"
#include "saverestore.h"
#include "mapentities_shared.h"
#include "vcollide_parse.h"
#include "hl2wars_player_shared.h"
#include "imouse.h"
#include "props_shared.h"
#include "beam_shared.h"
#include "basecombatweapon_shared.h"
#include "wars_mapboundary.h"
#include "src_python_converters_ents.h"
#include "src_python.h"
#include "tier0/memdbgon.h"
#include "_entities__entities_free_functions_pypp.hpp"

namespace bp = boost::python;

void register__entities_free_functions(){

    { //::GetPlayerRelationShip
    
        typedef ::Disposition_t ( *GetPlayerRelationShip_function_type )( int,int );
        
        bp::def( 
            "GetPlayerRelationShip"
            , GetPlayerRelationShip_function_type( &::GetPlayerRelationShip )
            , ( bp::arg("p1"), bp::arg("p2") ) );
    
    }

    { //::MapUnits
    
        typedef void ( *MapUnits_function_type )( ::boost::python::object );
        
        bp::def( 
            "MapUnits"
            , MapUnits_function_type( &::MapUnits )
            , ( bp::arg("method") ) );
    
    }

    { //::SetPlayerRelationShip
    
        typedef void ( *SetPlayerRelationShip_function_type )( int,int,::Disposition_t );
        
        bp::def( 
            "SetPlayerRelationShip"
            , SetPlayerRelationShip_function_type( &::SetPlayerRelationShip )
            , ( bp::arg("p1"), bp::arg("p2"), bp::arg("rel") ) );
    
    }

}

