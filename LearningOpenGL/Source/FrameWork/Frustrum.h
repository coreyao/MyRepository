#pragma once

#include "Utility.h"
#include "Math/Plane.h"
#include "Collision.h"

class CCamera;
class CFrustrum
{
public:
	void Init( const CCamera& cam );
	bool IsInFrustrum(const CAABB& worldRect) const;

private:
	CPlane m_vPlane[6];
};