//====== Copyright � Sandern Corporation, All rights reserved. ===========//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "wars_flora.h"
#include "wars_grid.h"
#include "mapentities_shared.h"
#include "gamestringpool.h"
#include "worldsize.h"
#include "hl2wars_util_shared.h"
#include <filesystem.h>
#include "vstdlib/ikeyvaluessystem.h"

#ifdef ENABLE_PYTHON
#include "srcpy.h"
#endif // ENABLE_PYTHON

#ifdef CLIENT_DLL
#include "c_hl2wars_player.h"
#include "cdll_util.h"
#include "playerandunitenumerator.h"
#include "c_entityflame.h"
#include "c_basetempentity.h"
#define CBaseTempEntity C_BaseTempEntity
#define CTEIgniteFlora C_TEIgniteFlora
#include "deferred/deferred_shared_common.h"
#else
#include "hl2wars_player.h"
#endif // CLIENT_DLL

#include "editor/editorsystem.h"
#include "wars/iwars_extension.h"
#include "editor/editorwarsmapmgr.h"

extern "C"
{
#ifdef WIN32
#define RPC_NO_WINDOWS_H
#include <Rpc.h>
#undef GetObject
#else
#include <uuid/uuid.h>
#endif
}

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifndef CLIENT_DLL
#define VarArgs UTIL_VarArgs
#else
ConVar cl_flora_disable( "cl_flora_disable", "0" );
ConVar cl_flora_animate( "cl_wars_flora_animate", "1", FCVAR_ARCHIVE );
ConVar cl_flora_avoid_units( "cl_wars_flora_avoid_units", "1", FCVAR_ARCHIVE );
ConVar cl_flora_avoid_units_debug( "cl_flora_avoid_units_debug", "0", 0 );
ConVar cl_flora_sway_speed( "cl_wars_flora_sway_speed", "10" );
#endif // CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: Flame/Ignite event
//-----------------------------------------------------------------------------
class CTEIgniteFlora : public CBaseTempEntity
{
public:
	
	DECLARE_CLASS( CTEIgniteFlora, CBaseTempEntity );
	DECLARE_NETWORKCLASS();

#ifndef CLIENT_DLL
	CTEIgniteFlora( const char *name )  : CBaseTempEntity( name ) {}
#else
	virtual void PostDataUpdate( DataUpdateType_t updateType );
#endif // CLIENT_DLL

	CNetworkVector( m_vecOrigin );
	CNetworkVar( float, m_fRadius );
	CNetworkVar( float, m_fLifetime );
};

#ifdef CLIENT_DLL
IMPLEMENT_CLIENTCLASS_EVENT( C_TEIgniteFlora, DT_TEIgniteFlora, CTEIgniteFlora );

BEGIN_RECV_TABLE_NOBASE( C_TEIgniteFlora, DT_TEIgniteFlora )
	RecvPropVector(RECVINFO( m_vecOrigin )),
	RecvPropFloat( RECVINFO( m_fRadius ) ),
	RecvPropFloat( RECVINFO( m_fLifetime ) ),
END_RECV_TABLE()

void CTEIgniteFlora::PostDataUpdate( DataUpdateType_t updateType )
{
	CWarsFlora::IgniteFloraInRadius( m_vecOrigin, m_fRadius, m_fLifetime );
}
#else
IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEIgniteFlora, DT_TEIgniteFlora )
	SendPropVector( SENDINFO( m_vecOrigin ) ),
	SendPropFloat( SENDINFO( m_fRadius ) ),
	SendPropFloat( SENDINFO( m_fLifetime ) ),
END_SEND_TABLE()

static CTEIgniteFlora g_TEIgniteFloraEvent( "IgniteFloraEvent" );
#endif // CLIENT_DLL

KeyValues *CWarsFlora::m_pKVFloraData = NULL;

LINK_ENTITY_TO_CLASS( wars_flora, CWarsFlora );

BEGIN_DATADESC( CWarsFlora )
	DEFINE_KEYFIELD( m_bEditorManaged,		FIELD_BOOLEAN,	"editormanaged" ),
	DEFINE_KEYFIELD( m_iszIdleAnimationName,		FIELD_STRING,	"idleanimation" ),
	DEFINE_KEYFIELD( m_iszSqueezeDownAnimationName,		FIELD_STRING,	"squeezedownanimation" ),
	DEFINE_KEYFIELD( m_iszSqueezeDownIdleAnimationName,	FIELD_STRING,	"squeezedownidleanimation" ),
	DEFINE_KEYFIELD( m_iszSqueezeUpAnimationName,		FIELD_STRING,	"squeezeupanimation" ),
	DEFINE_KEYFIELD( m_iszDestructionAnimationName,		FIELD_STRING,	"destructionanimation" ),
	// Custom parse florauuid, to avoid allocating many strings in the stringpool
	//DEFINE_KEYFIELD( m_iszFloraUUID,		FIELD_STRING,	"florauuid" ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWarsFlora::CWarsFlora() : m_iKey(-1)
{
	UseClientSideAnimation();
#ifndef CLIENT_DLL
	AddEFlags( EFL_SERVER_ONLY );
#else
	AddToClientSideAnimationList();
#endif // CLIENT_DLL

	m_iszIdleAnimationName = MAKE_STRING( "idle" );
	m_iszSqueezeDownAnimationName = MAKE_STRING( "down" );
	m_iszSqueezeDownIdleAnimationName = MAKE_STRING( "downidle" );
	m_iszSqueezeUpAnimationName = MAKE_STRING( "up" );
	m_iszDestructionAnimationName = MAKE_STRING( "kill" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWarsFlora::~CWarsFlora()
{
	RemoveFromFloraGrid();
}

#ifndef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::Precache( void )
{
	if ( GetModelName() == NULL_STRING )
	{
		Msg( "%s at (%.3f, %.3f, %.3f) has no model name!\n", GetClassname(), GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z );
		SetModelName( AllocPooledString( "models/error.mdl" ) );
	}

	PrecacheModel( STRING( GetModelName() ) );

	BaseClass::Precache();
}
#endif // CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::Spawn()
{
	InsertInFloraGrid();

#ifndef CLIENT_DLL
	Precache();
#endif // CLIENT_DLL

	BaseClass::Spawn();

#ifndef CLIENT_DLL
	SetModel( STRING( GetModelName() ) );
#endif // CLIENT_DLL

	InitFloraData();
	InitFloraSequences();

	m_iPoseX = LookupPoseParameter( "x" );
	m_iPoseY = LookupPoseParameter( "y" );
	m_iPoseZ = LookupPoseParameter( "z" );

	SetSolid( SOLID_NONE );
	Vector vecMins = CollisionProp()->OBBMins();
	Vector vecMaxs = CollisionProp()->OBBMaxs();
	SetSize( vecMins, vecMaxs );

	if( m_iPoseX != -1 )
		SetPoseParameter( m_iPoseX, 0.0f );
	if( m_iPoseY != -1 )
		SetPoseParameter( m_iPoseY, 0.0f );
	if( m_iPoseZ != -1 )
		SetPoseParameter( m_iPoseZ, 0.0f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::InitFloraData()
{
	static int keyIdle = KeyValuesSystem()->GetSymbolForString( "idlesequence" );
	static int keySqueezeDown = KeyValuesSystem()->GetSymbolForString( "squeezedownsequence" );
	static int keySqueezeDownIdle = KeyValuesSystem()->GetSymbolForString( "squeezedownidlesequence" );
	static int keySqueezeUp = KeyValuesSystem()->GetSymbolForString( "squeezeupsequence" );
	static int keyDestruct = KeyValuesSystem()->GetSymbolForString( "destructsequence" );
	static int keyIgnite = KeyValuesSystem()->GetSymbolForString( "ignite" );

	static int keyMinDist = KeyValuesSystem()->GetSymbolForString( "fademindist" );
	static int keyMaxDist = KeyValuesSystem()->GetSymbolForString( "fademaxdist" );

	// Find our section. No section? nothing to apply!
#ifdef CLIENT_DLL
	const char *pModelName = modelinfo->GetModelName( GetModel() );
#else
	const char *pModelName = STRING( GetModelName() );
#endif // CLIENT_DLL
	KeyValues *pSection = m_pKVFloraData->FindKey( pModelName );
	if ( !pSection )
	{
#ifdef CLIENT_DLL
		// Make sure it's not set to render everywhere
		SetDistanceFade( 
			GetMinFadeDist() < 1.0f ? 2200.0f : GetMinFadeDist(), 
			GetMaxFadeDist() < 1.0f ? 2500.0f : GetMaxFadeDist() 
		);
#endif // CLIENT_DLL
		return;
	}

	// Can be put on fire?
	m_bCanBeIgnited = pSection->GetBool( keyIgnite, false );

	// Sequence label names
	const char *pIdleAnimationName = pSection->GetString( keyIdle, NULL );
	if( pIdleAnimationName )
		m_iszIdleAnimationName = AllocPooledString( pIdleAnimationName );

	const char *pDownAnimationName = pSection->GetString( keySqueezeDown, NULL );
	if( pDownAnimationName )
		m_iszSqueezeDownAnimationName = AllocPooledString( pDownAnimationName );

	const char *pDownIdleAnimationName = pSection->GetString( keySqueezeDownIdle, NULL );
	if( pDownIdleAnimationName )
		m_iszSqueezeDownIdleAnimationName = AllocPooledString( pDownIdleAnimationName );

	const char *pUpAnimationName = pSection->GetString( keySqueezeUp, NULL );
	if( pUpAnimationName )
		m_iszSqueezeUpAnimationName = AllocPooledString( pUpAnimationName );

	const char *pDestructAnimationName = pSection->GetString( keyDestruct, NULL );
	if( pDestructAnimationName )
		m_iszDestructionAnimationName = AllocPooledString( pDestructAnimationName );

#ifdef CLIENT_DLL
	// Fade distance
	float fademindist = pSection->GetFloat( keyMinDist, GetMinFadeDist() < 1.0f ? 2200.0f : GetMinFadeDist() );
	float fademaxdist = pSection->GetFloat( keyMaxDist, GetMaxFadeDist() < 1.0f ? 2500.0f : GetMaxFadeDist() );
	SetDistanceFade( fademindist, fademaxdist );
#endif // CLIENT_DLL
}

//-----------------------------------------------------------------------------
// Purpose: Reads out sequences
//-----------------------------------------------------------------------------
void CWarsFlora::InitFloraSequences()
{
	m_iIdleSequence = (m_iszIdleAnimationName != NULL_STRING ? LookupSequence( STRING( m_iszIdleAnimationName ) ) : -1);
	m_iSqueezeDownSequence = (m_iszSqueezeDownAnimationName != NULL_STRING ? LookupSequence( STRING( m_iszSqueezeDownAnimationName ) ) : -1);
	m_iSqueezeDownIdleSequence = (m_iszSqueezeDownIdleAnimationName != NULL_STRING ? LookupSequence( STRING( m_iszSqueezeDownIdleAnimationName ) ) : -1);
	m_iSqueezeUpSequence = (m_iszSqueezeUpAnimationName != NULL_STRING ? LookupSequence( STRING( m_iszSqueezeUpAnimationName ) ) : -1);
	m_iDestructSequence = (m_iszDestructionAnimationName != NULL_STRING ? LookupSequence( STRING( m_iszDestructionAnimationName ) ) : -1);

	if( m_iIdleSequence != -1 )
		SetSequence( m_iIdleSequence );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWarsFlora::HasFloraUUID()
{
	return m_iszFloraUUID[0] != '\0';
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::GenerateFloraUUID()
{
#ifdef WIN32
    UUID uuid;
    UuidCreate ( &uuid );

    unsigned char * str;
    if( UuidToStringA ( &uuid, &str ) == RPC_S_OK )
	{
		V_strncpy( m_iszFloraUUID, (char *)str, sizeof( m_iszFloraUUID ) );

		RpcStringFreeA ( &str );
	}
	else
	{
		Warning( "WarsFlora::GenerateFloraUUID: Could not generate flora UUID\n");
	}
#else
    uuid_t uuid;
    uuid_generate_random ( uuid );
    uuid_unparse ( uuid, m_iszFloraUUID );
#endif
	//DevMsg("Generated GUID: %s\n", STRING(m_iszFloraUUID));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CWarsFlora::GetFloraUUID()
{
	return m_iszFloraUUID;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::PlayDestructionAnimation()
{
	if( m_iDestructSequence == -1 )
		return;

	SetSequence( m_iDestructSequence );
	ResetSequenceInfo();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWarsFlora::FillKeyValues( KeyValues *pEntityKey, int iVisGroupId )
{
	pEntityKey->SetInt( "editormanaged", m_bEditorManaged );
	pEntityKey->SetString( "idleanimation", STRING( m_iszIdleAnimationName ) );
	pEntityKey->SetString( "squeezedownanimation", STRING( m_iszSqueezeDownAnimationName ) );
	pEntityKey->SetString( "squeezedownidleanimation", STRING( m_iszSqueezeDownIdleAnimationName ) );
	pEntityKey->SetString( "squeezeupanimation", STRING( m_iszSqueezeUpAnimationName ) );
	pEntityKey->SetString( "destructionanimation", STRING( m_iszDestructionAnimationName ) );

	pEntityKey->SetString( "rendercolor", VarArgs("%d %d %d", GetRenderColor().r, GetRenderColor().g, GetRenderColor().b ) );
	pEntityKey->SetInt( "spawnflags", GetSpawnFlags() );
	pEntityKey->SetInt( "skin", GetSkin() );

	pEntityKey->SetString( "florauuid", GetFloraUUID() );

	if( iVisGroupId != -1 )
	{
		KeyValues *pKVEditorGroup = new KeyValues("editor");
		pKVEditorGroup->SetString( "color", "0 255 0" );
		pKVEditorGroup->SetInt( "visgroupid", iVisGroupId );
		pKVEditorGroup->SetInt( "visgroupshown", 1 );
		pKVEditorGroup->SetInt( "visgroupautoshown", 1 );
		pKVEditorGroup->SetString( "logicalpos", "[0 0]" );
		pEntityKey->AddSubKey( pKVEditorGroup );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWarsFlora::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq(szKeyName, "health") )
	{
		m_iHealth = V_atoi(szValue);
	}
	else if (FStrEq(szKeyName, "spawnflags"))
	{
		m_spawnflags = V_atoi(szValue);
	}
	else if (FStrEq(szKeyName, "model"))
	{
		SetModelName( AllocPooledString( szValue ) );
	}
#ifdef CLIENT_DLL
	else if (FStrEq(szKeyName, "fademaxdist"))
	{
		float flFadeMaxDist = V_atof(szValue);
		SetDistanceFade( GetMinFadeDist(), flFadeMaxDist );
	}
	else if (FStrEq(szKeyName, "fademindist"))
	{
		float flFadeMinDist = V_atof(szValue);
		SetDistanceFade( flFadeMinDist, GetMaxFadeDist() );
	}
	else if (FStrEq(szKeyName, "fadescale"))
	{
		SetGlobalFadeScale( V_atof(szValue) );
	}
	else if (FStrEq(szKeyName, "skin"))
	{
		SetSkin( V_atoi(szValue) );
	}
#endif // CLIENT_DLL
	else if (FStrEq(szKeyName, "idleanimation"))
	{
		m_iszIdleAnimationName = AllocPooledString( szValue );
	}
	else if (FStrEq(szKeyName, "squeezedownanimation"))
	{
		m_iszSqueezeDownAnimationName = AllocPooledString( szValue );
	}
	else if (FStrEq(szKeyName, "squeezedownidleanimation"))
	{
		m_iszSqueezeDownIdleAnimationName = AllocPooledString( szValue );
	}
	else if (FStrEq(szKeyName, "squeezeupanimation"))
	{
		m_iszSqueezeUpAnimationName = AllocPooledString( szValue );
	}
	else if (FStrEq(szKeyName, "destructionanimation"))
	{
		m_iszDestructionAnimationName = AllocPooledString( szValue );
	}
	else if (FStrEq(szKeyName, "florauuid"))
	{
		V_strncpy( m_iszFloraUUID, szValue, sizeof( m_iszFloraUUID ) );
	}
	else
	{
		if ( !BaseClass::KeyValue( szKeyName, szValue ) )
		{
			// key hasn't been handled
			return false;
		}
	}

	return true;
}

// Client Spawning Code
#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::UpdateUnitAvoid()
{
	if( !cl_flora_avoid_units.GetBool() )
		return;

	const Vector &vOrigin = GetAbsOrigin();

	if( m_iPoseX != -1 && m_iPoseY != -1 && m_iPoseZ != -1 )
	{
		float flRadius = CollisionProp()->BoundingRadius();
		CUnitEnumerator avoid( flRadius, vOrigin );
		partition->EnumerateElementsInSphere( PARTITION_CLIENT_SOLID_EDICTS, vOrigin, flRadius, false, &avoid );

		int i;
		Vector vecAvoidAvg( 0, 0, 0 );

		int c = avoid.GetObjectCount();
		if( c > 0 ) 
		{
			Vector vecAvoid;
			for ( i = 0; i < c; i++ )
			{
				CBaseEntity *pEnt = avoid.GetObject( i );
		
				vecAvoid = vOrigin - pEnt->GetAbsOrigin();
				vecAvoid.z = 0;
				vecAvoid.z = 1.0f - Min( Max( vecAvoid.Length() / flRadius, 0.0f ), 1.0f );
				vecAvoidAvg += vecAvoid;
			}

			vecAvoidAvg /= c;
			VectorNormalize( vecAvoidAvg );

			if( cl_flora_avoid_units_debug.GetBool() )
			{
				NDebugOverlay::HorzArrow( vOrigin, vOrigin + vecAvoidAvg * 64.0f, 
									2.0f, 0, 255, 0, 200, true, 0 );
			}

			VectorYawRotate( vecAvoidAvg, -GetAbsAngles()[YAW], vecAvoidAvg );

			if( cl_flora_avoid_units_debug.GetBool() )
			{
				NDebugOverlay::EntityTextAtPosition( vOrigin, 0, VarArgs("Yaw: %.2f\n", GetAbsAngles()[YAW]), gpGlobals->frametime, 255, 255, 255, 255 );
				NDebugOverlay::EntityTextAtPosition( vOrigin, 1, VarArgs("x: %.2f, y: %.2f, z: %.2f\n", vecAvoidAvg.x, vecAvoidAvg.y, vecAvoidAvg.z), gpGlobals->frametime, 255, 255, 255, 255 );
			}
		}

		float frac = gpGlobals->frametime * cl_flora_sway_speed.GetFloat();
		for ( i = 0; i < 3; i++ )
		{
			m_vCurrentSway[ i ] = m_vCurrentSway[ i ] + frac * ( vecAvoidAvg[ i ] - m_vCurrentSway[ i ] );
		}

		SetPoseParameter( m_iPoseX, m_vCurrentSway.y );
		SetPoseParameter( m_iPoseY, -m_vCurrentSway.x );
		SetPoseParameter( m_iPoseZ, m_bFlattened ? 1.0f : m_vCurrentSway.z );
	}
	else if( m_iSqueezeDownSequence != -1 )
	{
		float flRadius = CollisionProp()->BoundingRadius();
		CUnitEnumerator avoid( flRadius, vOrigin );
		partition->EnumerateElementsInSphere( PARTITION_CLIENT_SOLID_EDICTS, vOrigin, flRadius, false, &avoid );

		// Okay, decide how to avoid if there's anything close by
		int c = avoid.GetObjectCount();
		if( c > 0 )
		{
			int iSequence = GetSequence();
			if( iSequence != m_iSqueezeDownSequence && iSequence != m_iSqueezeDownIdleSequence  )
			{
				SetSequence( m_iSqueezeDownSequence );
				ResetSequenceInfo();
			}
			else if( iSequence == m_iSqueezeDownSequence && IsSequenceFinished() && m_iSqueezeDownIdleSequence != -1 )
			{
				SetSequence( m_iSqueezeDownIdleSequence );
				ResetSequenceInfo();
			}

			m_fAvoidTimeOut = gpGlobals->curtime + 0.1f;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::UpdateClientSideAnimation()
{
	if( !cl_flora_animate.GetBool() )
		return;

	// Don't update any flora not in screen
	const Vector &vOrigin = GetAbsOrigin();

	int iX, iY;
	bool bInScreen = GetVectorInScreenSpace( vOrigin, iX, iY );
	if( !bInScreen|| iX < 0 || iX > ScreenWidth() || iY < 0 || iY > ScreenHeight() )
		return;

	BaseClass::UpdateClientSideAnimation();

	UpdateUnitAvoid();

	if( m_fAvoidTimeOut < gpGlobals->curtime )
	{
		int iSequence = GetSequence();
		if( iSequence == m_iSqueezeDownSequence || iSequence == m_iSqueezeDownIdleSequence )
		{
			SetSequence( m_iSqueezeUpSequence );
			ResetSequenceInfo();
		}
		else if( iSequence == m_iSqueezeUpSequence && IsSequenceFinished() )
		{
			SetSequence( m_iIdleSequence );
			ResetSequenceInfo();
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CWarsFlora::Ignite( float flFlameLifetime, float flSize )
{
	if( !m_bCanBeIgnited )
		return;

	// Crash on trying attach particle flames to hitboxes if they don't exist...
	if( GetHitboxSetCount() == 0 )
		return;

	if( IsOnFire() )
		return;

	C_EntityFlame *pFlame = C_EntityFlame::Create( this, flFlameLifetime, flSize );
	AddFlag( FL_ONFIRE );
	SetEffectEntity( pFlame );
}

void CWarsFlora::IgniteLifetime( float flFlameLifetime )
{
	if( !m_bCanBeIgnited )
		return;

	if( !IsOnFire() )
		Ignite( 30, 0.0f );

	C_EntityFlame *pFlame = dynamic_cast<C_EntityFlame*>( GetEffectEntity() );

	if ( !pFlame )
		return;

	pFlame->SetLifetime( flFlameLifetime );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWarsFlora::Initialize()
{
	if ( InitializeAsClientEntity( STRING(GetModelName()), false ) == false )
	{
		return false;
	}

	Spawn();

	const model_t *model = GetModel();
	if( model )
	{
		Vector mins, maxs;
		ExtractBbox( GetSequence(), mins, maxs );
		SetSize( mins, maxs );
	}

	SetCollisionGroup( COLLISION_GROUP_DEBRIS );

	//UpdatePartitionListEntry();
	//CollisionProp()->UpdatePartition();

	UpdateVisibility();

	SetNextClientThink( CLIENT_THINK_NEVER );

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CWarsFlora::ParseEntity( const char *pEntData )
{
	CEntityMapData entData( (char*)pEntData );
	char className[MAPKEY_MAXLENGTH];
	
	MDLCACHE_CRITICAL_SECTION();

	if (!entData.ExtractValue("classname", className))
	{
		Error( "classname missing from entity!\n" );
	}

	if ( !V_strcmp( className, "wars_flora" ) )
	{
		// always force clientside entities placed in maps
		CWarsFlora *pEntity = new CWarsFlora();

		// Set up keyvalues.
		pEntity->ParseMapData(&entData);
			
		if ( !pEntity->Initialize() )
			pEntity->Release();
		
		return entData.CurrentBufferPosition();
	}

	// Just skip past all the keys.
	char keyName[MAPKEY_MAXLENGTH];
	char value[MAPKEY_MAXLENGTH];
	if ( entData.GetFirstKey(keyName, value) )
	{
		do 
		{
		} 
		while ( entData.GetNextKey(keyName, value) );
	}

	//
	// Return the current parser position in the data block
	//
	return entData.CurrentBufferPosition();
}

#endif // CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::SpawnMapFlora()
{
#ifdef CLIENT_DLL
	if( cl_flora_disable.GetBool() )
		return;

	if( GetDeferredManager()->IsDeferredRenderingEnabled() == false )
		return;

	const char *pMapData = engine->GetMapEntitiesString();
	if( !pMapData )
	{
		Warning("CWarsFlora::SpawnMapFlora: Failed to get map entities data (no map loaded?)\n");
		return;
	}

	int nEntities = 0;

	char szTokenBuffer[MAPKEY_MAXLENGTH];

	//
	//  Loop through all entities in the map data, creating each.
	//
	for ( ; true; pMapData = MapEntity_SkipToNextEntity(pMapData, szTokenBuffer) )
	{
		//
		// Parse the opening brace.
		//
		char token[MAPKEY_MAXLENGTH];
		pMapData = MapEntity_ParseToken( pMapData, token );

		//
		// Check to see if we've finished or not.
		//
		if (!pMapData)
			break;

		if (token[0] != '{')
		{
			Error( "CWarsFlora::SpawnMapFlora: found %s when expecting {", token);
			continue;
		}

		//
		// Parse the entity and add it to the spawn list.
		//

		pMapData = ParseEntity( pMapData );

		nEntities++;
	}
#endif // CLIENT_DLL

	//
	// Loop through wars flora 
	//

	// For now, just load the keyvalues directly
	char curMap[MAX_PATH];
	CEditorWarsMapMgr::BuildCurrentWarsPath( curMap, sizeof( curMap ) );

	KeyValues *pKVWars = new KeyValues( "WarsMap" );
	KeyValues::AutoDelete autodelete( pKVWars );
	if( pKVWars->LoadFromFile( filesystem, curMap, NULL ) )
	{
		KeyValues *pFlora = pKVWars->FindKey( "flora" );
		if( pFlora )
		{
			for ( KeyValues *pKey = pFlora->GetFirstTrueSubKey(); pKey; pKey = pKey->GetNextTrueSubKey() )
			{
				if ( V_strcmp( pKey->GetName(), "entity" ) )
					continue;

#ifdef CLIENT_DLL
				CWarsFlora *pEntity = new CWarsFlora();
#else
				const char *className = pKey->GetString( "classname", NULL );
				if( !className )
					continue;

				CWarsFlora *pEntity = (CWarsFlora *)CreateEntityByName( className );
#endif // CLIENT_DLL

				FOR_EACH_VALUE( pKey, pValue )
				{
					pEntity->KeyValue( pValue->GetName(), pValue->GetString() );
				}

#ifdef CLIENT_DLL
				if ( !pEntity->Initialize() )
					pEntity->Release();
#else
				DispatchSpawn( pEntity );
				pEntity->Activate();
#endif // CLIENT_DLL
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::InitFloraGrid()
{
	InitFloraDataKeyValues();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::DestroyFloraGrid()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::InsertInFloraGrid()
{
	m_iKey = WARSGRID_KEY( GetAbsOrigin() );
	if( m_iKey < 0 || m_iKey > WarsGrid().GetGrid().Count() )
	{
#ifdef CLIENT_DLL
		Warning("CLIENT_");
#else
		Warning("SERVER_");
#endif // CLIENT_DLL
		Warning("InsertInFloraGrid: Could not insert flora entity with key %d (max %d)\n", m_iKey, WarsGrid().GetGrid().Count() );
		m_iKey = -1;
		return;
	}
	WarsGrid().GetGrid()[m_iKey].flora.AddToTail( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::RemoveFromFloraGrid()
{
	if( m_iKey == -1 )
		return;

	WarsGrid().GetGrid()[m_iKey].flora.FindAndRemove( this );
	m_iKey = -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWarsFlora::InitFloraDataKeyValues()
{
	if( m_pKVFloraData )
	{
		m_pKVFloraData->deleteThis();
		m_pKVFloraData = NULL;
	}

	FileFindHandle_t findHandle;
	const char *floraFilename = filesystem->FindFirstEx( "scripts/flora/*.txt", NULL, &findHandle );
	while ( floraFilename )
	{
		char floraPath[MAX_PATH];
		V_snprintf( floraPath, sizeof(floraPath), "scripts/flora/%s", floraFilename );
		//DevMsg("Found Flora data file: %s\n", floraPath);

		if( !m_pKVFloraData )
		{
			m_pKVFloraData = new KeyValues( "FloraDataFile" );
			if ( !m_pKVFloraData->LoadFromFile( filesystem, floraPath ) )
			{
				Warning( "InitFlora: Could not load flora data file %s\n", floraPath );
				m_pKVFloraData->deleteThis();
				m_pKVFloraData = NULL;
				return;
			}
		}
		else
		{
			KeyValues *pToMergeKV = new KeyValues( "FloraDataFile" );
			if ( pToMergeKV->LoadFromFile( filesystem, floraPath ) )
			{
				m_pKVFloraData->MergeFrom( pToMergeKV );
				return;
			}
			else
			{
				Warning( "InitFlora: Could not load flora data file %s\n", floraPath );
			}
			pToMergeKV->deleteThis();
			pToMergeKV = NULL;
		}

		floraFilename = filesystem->FindNext( findHandle );
	}
	filesystem->FindClose( findHandle );
}

//-----------------------------------------------------------------------------
// Purpose: Apply the functor to all flora in radius.
//-----------------------------------------------------------------------------
template < typename Functor >
bool ForAllFloraInRadius( Functor &func, const Vector &vPosition, float fRadius )
{
	CUtlVector<CWarsFlora *> foundFlora;
	float fRadiusSqr = fRadius * fRadius;

	int originX = WARSGRID_KEYSINGLE( vPosition.x );
	int originY = WARSGRID_KEYSINGLE( vPosition.y );
	int shiftLimit = ceil( fRadius / WARSGRID_TILESIZE );

	for( int x = originX - shiftLimit; x <= originX + shiftLimit; ++x )
	{
		if ( x < 0 || x >= WARSGRID_GRIDSIZE )
			continue;

		for( int y = originY - shiftLimit; y <= originY + shiftLimit; ++y )
		{
			if ( y < 0 || y >= WARSGRID_GRIDSIZE )
				continue;

			const FloraVector &floraVector = WarsGrid().GetGrid()[x + (y * WARSGRID_GRIDSIZE)].flora;
			for( int i = 0; i < floraVector.Count(); i++ )
			{
				CWarsFlora *pFlora = floraVector[i];
				if( pFlora && pFlora->GetAbsOrigin().DistToSqr( vPosition ) < fRadiusSqr )
				{
					foundFlora.AddToTail( pFlora );
				}
			}
		}
	}

	FOR_EACH_VEC( foundFlora, idx )
	{
		CWarsFlora *pFlora = foundFlora[idx];
		if( func( pFlora ) == false )
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function mainly intended for editor or ingame testing
//			The editor mode, the flora entity gets synced to the editing client
//-----------------------------------------------------------------------------
bool CWarsFlora::SpawnFlora( const char *pModelname, const Vector &vPosition, const QAngle &vAngle, KeyValues *pExtraKV
#ifdef ENABLE_PYTHON
	, boost::python::object fnpostspawn 
#endif // ENABLE_PYTHON
	)
{
	bool bEditorManaged = EditorSystem()->IsActive();

	// Create flora entity
	CWarsFlora *pEntity = (CWarsFlora *)CreateEntityByName("wars_flora");
	if ( !pEntity )
	{	
		Warning("wars_flora_spawn: Failed to create entity\n");
		return false;
	}

	// Apply data
	pEntity->KeyValue( "model", pModelname );
	pEntity->KeyValue( "angles", VarArgs( "%f %f %f", vAngle.x, vAngle.y, vAngle.z ) );
	pEntity->KeyValue( "origin", VarArgs( "%f %f %f", vPosition.x, vPosition.y, vPosition.z ) );
	pEntity->KeyValue( "editormanaged", bEditorManaged ? "1" : "0" );

	if( pExtraKV )
	{
		FOR_EACH_VALUE( pExtraKV, pValue )
		{
			pEntity->KeyValue( pValue->GetName(), pValue->GetString() );
		}
	}

	pEntity->GenerateFloraUUID();

#ifdef CLIENT_DLL
	// Initialize
	if ( !pEntity->Initialize() )
	{	
		pEntity->Release();
		Warning("CWarsFlora::SpawnFlora: Failed to initialize entity\n");
		return false;
	}
#else
	DispatchSpawn( pEntity );
	pEntity->Activate();

#ifdef ENABLE_PYTHON
	if( fnpostspawn != boost::python::object() )
	{
		try
		{
			fnpostspawn( pEntity->GetPyHandle() );
		}
		catch( boost::python::error_already_set & )
		{
			PyErr_Print();
			UTIL_Remove( pEntity );
			return false;
		}
	}
#endif // ENABLE_PYTHON

	if( bEditorManaged )
	{
		KeyValues *pOperation = new KeyValues( "data" );
		pOperation->SetString("operation", "create");
		pOperation->SetString("classname", "wars_flora");

		KeyValues *pEntValues = new KeyValues( "keyvalues" );
		pEntity->FillKeyValues( pEntValues );
		pEntValues->SetString( "model", pModelname );
		pEntValues->SetString( "angles", VarArgs( "%f %f %f", pEntity->GetAbsAngles().x, pEntity->GetAbsAngles().y, pEntity->GetAbsAngles().z ) );
		pEntValues->SetString( "origin", VarArgs( "%f %f %f", pEntity->GetAbsOrigin().x, pEntity->GetAbsOrigin().y, pEntity->GetAbsOrigin().z ) );

		pOperation->AddSubKey( pEntValues );

		warsextension->QueueClientCommand( pOperation );
	}
#endif // CLIENT_DLL

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Removes flora by UUID
//-----------------------------------------------------------------------------
void CWarsFlora::RemoveFloraByUUID( const char *pUUID )
{
#ifdef CLIENT_DLL
	for( CBaseEntity *pEntity = ClientEntityList().FirstBaseEntity(); pEntity; pEntity = ClientEntityList().NextBaseEntity( pEntity ) )
#else
	for( CBaseEntity *pEntity = gEntList.FirstEnt(); pEntity != NULL; pEntity = gEntList.NextEnt( pEntity ) )
#endif // CLIENT_DLL
	{
		CWarsFlora *pFlora = dynamic_cast<CWarsFlora *>( pEntity );
		if( pFlora && pFlora->HasFloraUUID() && V_strcmp( pFlora->GetFloraUUID(), pUUID ) == 0 )
		{
			pFlora->Remove();
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Removes flora in a radius. In Editor mode on server, this gets synced from
//		    server to client for the editing player (so should not be called by client)
//-----------------------------------------------------------------------------
void CWarsFlora::RemoveFloraInRadius( const Vector &vPosition, float fRadius, int nMax )
{
#ifdef CLIENT_DLL
	if( EditorSystem()->IsActive() )
	{
		Warning("CWarsFlora::RemoveFloraInRadius: Incorrect usage in editing mode.\n");
		return;
	}
#endif // CLIENT_DLL

	int iCount = 0;
	CUtlVector<CWarsFlora *> removeFlora;
	float fRadiusSqr = fRadius * fRadius;
#ifdef CLIENT_DLL
	for( CBaseEntity *pEntity = ClientEntityList().FirstBaseEntity(); pEntity; pEntity = ClientEntityList().NextBaseEntity( pEntity ) )
#else
	for( CBaseEntity *pEntity = gEntList.FirstEnt(); pEntity != NULL; pEntity = gEntList.NextEnt( pEntity ) )
#endif // CLIENT_DLL
	{
		CWarsFlora *pFlora = dynamic_cast<CWarsFlora *>( pEntity );
		if( pFlora && pFlora->GetAbsOrigin().DistToSqr( vPosition ) < fRadiusSqr )
		{
			if( EditorSystem()->IsActive() )
			{
				// TODO: Restrict to editor managed flora?
				//if( !pFlora->IsEditorManaged() )
				//	continue;

				if( !pFlora->HasFloraUUID() )
				{
					Warning("Flora at %f %f %f has no UUID! Please save and reload map\n", pFlora->GetAbsOrigin().x, pFlora->GetAbsOrigin().y, pFlora->GetAbsOrigin().z);
					continue;
				}
			}


			removeFlora.AddToTail( pFlora );
			iCount++;
			if( nMax != -1 && iCount >= nMax )
				break;
		}
		
	}

#ifndef CLIENT_DLL
	if( EditorSystem()->IsActive() )
	{
		// Tell editing player to sync deleted flora
		KeyValues *pOperation = new KeyValues( "data" );
		pOperation->SetString("operation", "deleteflora");
		
		FOR_EACH_VEC( removeFlora, idx )
		{
			pOperation->AddSubKey( new KeyValues( "flora", "uuid", removeFlora[idx]->GetFloraUUID() ) );
		}
		warsextension->QueueClientCommand( pOperation );
	}
#endif // CLIENT_DLL

	FOR_EACH_VEC( removeFlora, idx )
	{
		removeFlora[idx]->Remove();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWarsFlora *CWarsFlora::FindFloraByUUID( const char *pUUID )
{
#ifdef CLIENT_DLL
	for( CBaseEntity *pEntity = ClientEntityList().FirstBaseEntity(); pEntity; pEntity = ClientEntityList().NextBaseEntity( pEntity ) )
#else
	for( CBaseEntity *pEntity = gEntList.FirstEnt(); pEntity != NULL; pEntity = gEntList.NextEnt( pEntity ) )
#endif // CLIENT_DLL
	{
		CWarsFlora *pFlora = dynamic_cast<CWarsFlora *>( pEntity );
		if( pFlora && pFlora->HasFloraUUID() && V_strcmp( pFlora->GetFloraUUID(), pUUID ) == 0 )
		{
			return pFlora;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CWarsFlora::CountFloraInRadius( const Vector &vPosition, float fRadius )
{
	CUtlVector<int> restrictModels; // Empty for no restriction
	return CountFloraInRadius( vPosition, fRadius, restrictModels );
}

int CWarsFlora::CountFloraInRadius( const Vector &vPosition, float fRadius, CUtlVector<int> &restrictModels )
{
	int iCount = 0;
	float fRadiusSqr = fRadius * fRadius;
#ifdef CLIENT_DLL
	for( CBaseEntity *pEntity = ClientEntityList().FirstBaseEntity(); pEntity; pEntity = ClientEntityList().NextBaseEntity( pEntity ) )
#else
	for( CBaseEntity *pEntity = gEntList.FirstEnt(); pEntity != NULL; pEntity = gEntList.NextEnt( pEntity ) )
#endif // CLIENT_DLL
	{
		CWarsFlora *pFlora = dynamic_cast<CWarsFlora *>( pEntity );
		if( pFlora && pFlora->GetAbsOrigin().DistToSqr( vPosition ) < fRadiusSqr && (!restrictModels.Count() || restrictModels.Find(pFlora->GetModelIndex()) != -1) )
		{
			iCount++;
		}
		
	}

	return iCount;
}

#ifdef ENABLE_PYTHON
int CWarsFlora::PyCountFloraInRadius( const Vector &vPosition, float fRadius, boost::python::list pyRestrictModels )
{
	CUtlVector<int> restrictModels;
	ListToUtlVectorByValue<int>( pyRestrictModels, restrictModels );
	return CountFloraInRadius( vPosition, fRadius, restrictModels );
}
#endif // ENABLE_PYTHON

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class WarsFloraDestructAnimation
{
public:
	bool operator()( CWarsFlora *flora )
	{
		flora->PlayDestructionAnimation();
		return true;
	}
};

void CWarsFlora::DestructFloraInRadius( const Vector &vPosition, float fRadius )
{
	WarsFloraDestructAnimation functor;
	ForAllFloraInRadius<WarsFloraDestructAnimation>( functor, vPosition, fRadius );
}

//-----------------------------------------------------------------------------
// Purpose: Put flora on fire!
//-----------------------------------------------------------------------------
#ifdef CLIENT_DLL
class WarsFloraIgnite
{
public:
	WarsFloraIgnite( float fLifetime ) : m_fLifetime(fLifetime) {}

	bool operator()( CWarsFlora *flora )
	{
		flora->IgniteLifetime( m_fLifetime );
		return true;
	}

private:
	float m_fLifetime;
};
#endif // CLIENT_DLL
void CWarsFlora::IgniteFloraInRadius( const Vector &vPosition, float fRadius, float fLifetime )
{
#ifdef CLIENT_DLL
	WarsFloraIgnite functor( fLifetime );
	ForAllFloraInRadius<WarsFloraIgnite>( functor, vPosition, fRadius );
#else
	CPVSFilter filter( vPosition );
	g_TEIgniteFloraEvent.m_vecOrigin = vPosition;
	g_TEIgniteFloraEvent.m_fRadius = fRadius;
	g_TEIgniteFloraEvent.m_fLifetime = fLifetime;
	g_TEIgniteFloraEvent.Create( filter, 0.0f );
#endif // CLIENT_DLL
}

//-----------------------------------------------------------------------------
// Purpose: Flattens flora in radius (if supported on the flora model)
//-----------------------------------------------------------------------------
#ifdef CLIENT_DLL
class WarsFloraFlatten
{
public:
	WarsFloraFlatten( bool bFlatten ) : m_bFlatten(bFlatten) {}

	bool operator()( CWarsFlora *flora )
	{
		flora->Flatten( m_bFlatten );
		return true;
	}

private:
	bool m_bFlatten;
};
#endif // CLIENT_DLL

void CWarsFlora::FlattenFloraInRadius( const Vector &vPosition, float fRadius, bool bFlatten )
{
#ifdef CLIENT_DLL
	WarsFloraFlatten functor( bFlatten );
	ForAllFloraInRadius<WarsFloraFlatten>( functor, vPosition, fRadius );
#else
	Warning("FlattenFloraInRadius not supported on server\n");
#endif // CLIENT_DLL
}

#ifdef CLIENT_DLL
CON_COMMAND_F( cl_wars_flora_spawn, "Spawns the specified flora model", FCVAR_CHEAT )
#else
CON_COMMAND_F( wars_flora_spawn, "Spawns the specified flora model", FCVAR_CHEAT )
#endif // CLIENT_DLL
{
	if( args.ArgC() < 2 )
	{
		Warning("wars_flora_spawn: Not enough arguments.\n Example usage: wars_flora_spawn <modelname> <randomradius>");
		return;
	}
	
#ifdef CLIENT_DLL
	CHL2WarsPlayer *pPlayer = CHL2WarsPlayer::GetLocalHL2WarsPlayer(); 
#else
	CHL2WarsPlayer *pPlayer = ToHL2WarsPlayer( UTIL_PlayerByIndex( UTIL_GetCommandClientIndex() ) );
#endif // CLIENT_DLL
	if( !pPlayer )
		return;

	const MouseTraceData_t &data = pPlayer->GetMouseData();

	// Collect arguments
	const char *pModelName = args[1];
	float fRandomMaxRadius = args.ArgC() > 2 ? atof(args[2]) : 0;

#ifndef CLIENT_DLL
	CBaseEntity::PrecacheModel( pModelName );
#endif // CLIENT_DLL

	// Load model and determine size
	const model_t *pModel = modelinfo->FindOrLoadModel( pModelName );
	if( !pModel )
	{
		Warning("wars_flora_spawn: Failed to load model\n");
		return;
	}

	Vector mins, maxs;
	modelinfo->GetModelBounds( pModel, mins, maxs );

	// Determine position
	float startradius = 0;
	float maxradius = 0.0f;
	float radiusgrow = 0.0f;
	float radiusstep = 0.0f;
	CBaseEntity *ignore = NULL;

	float fRandomDegree = random->RandomFloat() * 2 * M_PI;
	float fRandomRadius = random->RandomFloat(0.0f, fRandomMaxRadius);
	Vector vRandomPosOffset( cos(fRandomDegree) * fRandomRadius, sin(fRandomDegree) * fRandomRadius, 0.0f );

	positioninfo_t info( data.m_vEndPos + vRandomPosOffset, mins, maxs, startradius, maxradius, radiusgrow, radiusstep, ignore );
	UTIL_FindPosition( info );

	if( !info.m_bSuccess || info.m_vPosition == vec3_origin )
	{
		Warning("wars_flora_spawn: Could not find a valid position\n");
		return;
	}

	QAngle angles(0, random->RandomFloat(0, 360), 0);
	CWarsFlora::SpawnFlora( pModelName, info.m_vPosition, angles );

#ifndef CLIENT_DLL
	if( !EditorSystem()->IsActive() )
	{
		for( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
			if( !pPlayer || !pPlayer->IsConnected() )
				continue;
			engine->ClientCommand( pPlayer->edict(), VarArgs( "cl_wars_flora_spawn %s", args.ArgS() ) );
		}
	}
#endif // CLIENT_DLL
}

#ifdef CLIENT_DLL
CON_COMMAND_F( cl_wars_flora_grid_mouse, "", FCVAR_CHEAT )
#else
CON_COMMAND_F( wars_flora_grid_mouse, "", FCVAR_CHEAT )
#endif // CLIENT_DLL
{
#ifdef CLIENT_DLL
	CHL2WarsPlayer *pPlayer = CHL2WarsPlayer::GetLocalHL2WarsPlayer(); 
#else
	CHL2WarsPlayer *pPlayer = ToHL2WarsPlayer( UTIL_PlayerByIndex( UTIL_GetCommandClientIndex() ) );
#endif // CLIENT_DLL
	if( !pPlayer )
		return;

	const MouseTraceData_t &data = pPlayer->GetMouseData();

	int iKey = WARSGRID_KEY( data.m_vWorldOnlyEndPos );

	Msg( "%d Flora at mouse position\n", WarsGrid().GetGrid()[iKey].flora.Count() );
	for( int i = 0; i < WarsGrid().GetGrid()[iKey].flora.Count(); i++ )
	{
		Msg("%d: uuid: %s (%d)\n", i, WarsGrid().GetGrid()[iKey].flora[i]->GetFloraUUID(), WarsGrid().GetGrid()[iKey].flora[i]->HasFloraUUID() );
	}
}

#ifdef CLIENT_DLL
CON_COMMAND_F( cl_wars_flora_ignite, "", FCVAR_CHEAT )
#else
CON_COMMAND_F( wars_flora_ignite, "", FCVAR_CHEAT )
#endif // CLIENT_DLL
{
#ifdef CLIENT_DLL
	CHL2WarsPlayer *pPlayer = CHL2WarsPlayer::GetLocalHL2WarsPlayer(); 
#else
	CHL2WarsPlayer *pPlayer = ToHL2WarsPlayer( UTIL_PlayerByIndex( UTIL_GetCommandClientIndex() ) );
#endif // CLIENT_DLL
	if( !pPlayer )
		return;

	const MouseTraceData_t &data = pPlayer->GetMouseData();

	CWarsFlora::IgniteFloraInRadius( data.m_vWorldOnlyEndPos, args.ArgC() > 1 ? atof( args[1] ) : 128.0f );
}


#ifdef CLIENT_DLL
CON_COMMAND_F( cl_wars_flora_data_reload, "", FCVAR_CHEAT )
#else
CON_COMMAND_F( wars_flora_data_reload, "", FCVAR_CHEAT )
#endif // CLIENT_DLL
{
	CWarsFlora::InitFloraDataKeyValues();
}


#ifdef CLIENT_DLL
CON_COMMAND_F( cl_wars_flora_debug_list_animated, "", FCVAR_CHEAT )
{
	int iTotalCount = 0;
	int iAnimatedCount = 0;

	for( CBaseEntity *pEntity = ClientEntityList().FirstBaseEntity(); pEntity; pEntity = ClientEntityList().NextBaseEntity( pEntity ) )
	{
		CWarsFlora *pFlora = dynamic_cast<CWarsFlora *>( pEntity );
		if( pFlora )
		{
			iTotalCount += 1;
			const Vector &vOrigin = pFlora->GetAbsOrigin();

			int iX, iY;
			bool bInScreen = GetVectorInScreenSpace( vOrigin, iX, iY );
			if( !bInScreen || iX < 0 || iX > ScreenWidth() || iY < 0 || iY > ScreenHeight() )
				continue;

			 iAnimatedCount += 1;
		}
	}

	Msg( "Animating %d out %d flora\n", iAnimatedCount, iTotalCount );
}
#endif // CLIENT_DLL

#ifdef CLIENT_DLL
CON_COMMAND_F( cl_wars_flora_removeall, "", FCVAR_CHEAT )
#else
CON_COMMAND_F( wars_flora_removeall, "", FCVAR_CHEAT )
#endif // CLIENT_DLL
{
	const char *pModelName = args.ArgC() > 1 ? args[1] : NULL;

	CUtlVector< CWarsFlora * > flora;
#ifdef CLIENT_DLL
	for( CBaseEntity *pEntity = ClientEntityList().FirstBaseEntity(); pEntity; pEntity = ClientEntityList().NextBaseEntity( pEntity ) )
#else
	for( CBaseEntity *pEntity = gEntList.FirstEnt(); pEntity != NULL; pEntity = gEntList.NextEnt( pEntity ) )
#endif // CLIENT_DLL
	{
		CWarsFlora *pFlora = dynamic_cast<CWarsFlora *>( pEntity );
		if( pFlora && ( !pModelName || V_strcmp( pModelName, STRING(pFlora->GetModelName()) ) == 0 ) )
		{
			flora.AddToTail( pFlora );
		}
	}

	Msg( "Removed %d flora!\n", flora.Count() );
	FOR_EACH_VEC( flora, idx )
	{
		flora[idx]->Remove();
	}
}