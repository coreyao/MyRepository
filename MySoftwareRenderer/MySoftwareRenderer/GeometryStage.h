#pragma once

#include "DataTypes.h"

namespace GeometryStage
{
	bool DoClipInClipSpace(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace);
	void TransformClipToScreen(SFaceRuntime& face);

	bool IsBackFace(SFaceRuntime& face, EVertexOrder eOrder = EVertexOrder_ClockWise);
	bool IsVertexInCVV(const Vec4& pos);
}