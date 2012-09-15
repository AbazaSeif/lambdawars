//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared util code between client and server.
//
//=============================================================================//

#ifndef HL2WARS_UTIL_SHARED_H
#define HL2WARS_UTIL_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "unit_base_shared.h"

struct positioninradius_t
{
	positioninradius_t( const Vector &startposition, const Vector &mins, const Vector &maxs, float radius, 
						QAngle fan=QAngle(0, 0, 0), float stepsize=0, CBaseEntity *ignore = NULL, float beneathlimit = 256.0f )
		: m_vStartPosition(startposition), m_vPosition(startposition), m_vMins(mins), m_vMaxs(maxs), m_fRadius(radius), m_vFan(fan), 
		  m_fStepSize(stepsize), m_hIgnore(ignore), m_bSuccess(false), m_fBeneathLimit(beneathlimit)
	{
		if( m_fStepSize == 0 )
			ComputeRadiusStepSize();
	}

	Vector m_vStartPosition;
	Vector m_vPosition;
	Vector m_vMins;
	Vector m_vMaxs;
	float m_fRadius;
	QAngle m_vFan;
	float m_fStepSize;
	EHANDLE m_hIgnore;
	float m_fBeneathLimit;
	bool m_bSuccess;

    void ComputeRadiusStepSize( void )
	{
        Assert( m_fRadius > 0 );
        float perimeter = 2 * M_PI * m_fRadius;
        float sizeunit = int(sqrt(pow(m_vMins.x - m_vMaxs.x, 2)+pow(m_vMins.y - m_vMaxs.y, 2))*1.25);
        m_fStepSize = int(360 / (perimeter / sizeunit));
	}
};

void UTIL_FindPositionInRadius( positioninradius_t &info );

struct positioninfo_t
{
	positioninfo_t( const Vector &startposition, const Vector &mins, const Vector &maxs, float startradius=0, 
		            float maxradius=0.0, float radiusstep=0.0, CBaseEntity *ignore = NULL , float zoffset=0.0, float beneathlimit = 256.0f ) 
		: m_vPosition(startposition), m_vMins(mins), m_vMaxs(maxs), m_fRadius(startradius), m_fMaxRadius(maxradius), m_fRadiusStep(radiusstep), 
		  m_hIgnore(ignore), m_bSuccess(false), m_InRadiusInfo(startposition, mins, maxs, 0, QAngle(0, 0, 0), 0, ignore), m_fBeneathLimit(beneathlimit)
	{
		m_vPosition.z += zoffset;
		m_InRadiusInfo.m_vStartPosition.z += zoffset;
		m_InRadiusInfo.m_vPosition.z += zoffset;

        if( m_fRadiusStep == 0 )
            m_fRadiusStep = int(abs(mins.x - maxs.x));

        if( m_fMaxRadius == 0 )
            m_fMaxRadius = m_fRadiusStep*100;
	}

	Vector m_vPosition;
	Vector m_vMins;
	Vector m_vMaxs;
	float m_fRadius;
	float m_fMaxRadius;
	float m_fRadiusStep;
	EHANDLE m_hIgnore;
	float m_fBeneathLimit;
	bool m_bSuccess;

	positioninradius_t m_InRadiusInfo;
};

void UTIL_FindPosition( positioninfo_t &info );

#ifdef CLIENT_DLL
class C_HL2WarsPlayer;

Vector UTIL_GetMouseAim( C_HL2WarsPlayer *player, int x, int y );

void UTIL_GetScreenMinsMaxs( const Vector &origin, const QAngle &angle, const Vector &mins, const Vector &maxs, 
							int &minsx, int &minsy, int &maxsx, int &maxsy );
#endif // CLIENT_DLL

class CTraceFilterSkipFriendly : public CTraceFilterSimple
{
public:
	CTraceFilterSkipFriendly( const IHandleEntity *passentity, int collisionGroup, CUnitBase *pUnit );

	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask );

private:
	CUnitBase *m_pUnit;
};

class CTraceFilterSkipEnemies : public CTraceFilterSimple
{
public:
	CTraceFilterSkipEnemies( const IHandleEntity *passentity, int collisionGroup, CUnitBase *pUnit );

	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask );

private:
	CUnitBase *m_pUnit;
};

//-----------------------------------------------------------------------------
// Special tracefilter for movement
//-----------------------------------------------------------------------------
class CTraceFilterWars : public CTraceFilterSimple
{
public:
	DECLARE_CLASS_NOBASE( CTraceFilterWars );

	CTraceFilterWars( const IHandleEntity *passentity, int collisionGroup ) : CTraceFilterSimple(passentity, collisionGroup) {}
	
	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask );

	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_ENTITIES_ONLY;
	}
};

#endif // HL2WARS_UTIL_SHARED_H