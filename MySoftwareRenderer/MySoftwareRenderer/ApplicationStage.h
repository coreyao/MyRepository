#pragma once

#include "DataTypes.h"

namespace ApplicationStage
{
	bool IsBackFace(const SVertex& v1, const SVertex& v2, const SVertex& v3, EVertexOrder eOrder = EVertexOrder_ClockWise);
}