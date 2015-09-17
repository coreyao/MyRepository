#pragma once

#include "DataTypes.h"

namespace ApplicationStage
{
	void TransformLocalToWorld(SFaceRuntime& face, const Mat4& worldTransform);
	bool IsBackFace(SFaceRuntime& face, EVertexOrder eOrder = EVertexOrder_ClockWise);
}