#pragma once

#include "DataTypes.h"

namespace GeometryStage
{
	bool DoClipInClipSpace(CFaceRuntime& face, bool& bAddFace, CFaceRuntime& addFace);
	void TransformClipToScreen(CFaceRuntime& face);

	bool IsBackFace(CFaceRuntime& face, EVertexOrder eOrder = EVertexOrder_ClockWise);
	bool IsVertexInCVV(const Vec4& pos);
}