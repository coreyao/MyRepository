#pragma once

#include "DataTypes.h"

namespace GeometryStage
{
	void LerpVertex(SVertexRuntime* pVertex1, SVertexRuntime* pVertex2, float t, SVertexRuntime* pVertexOut);
	bool DoClipInClipSpace(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace);
	void TransformClipToScreen(SFaceRuntime& face);

	bool IsBackFace(SFaceRuntime& face, EVertexOrder eOrder = EVertexOrder_ClockWise);
	bool IsVertexInCVV(const Vec4& pos);
}