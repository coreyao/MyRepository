#pragma once

#include "DataTypes.h"

namespace GeometryStage
{
	void TransformWorldToCamera(SFaceRuntime& face);
	bool DoClipInClipSpace(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace);
	void TransformCameraToClip(SFaceRuntime& face);
	void TransformClipToScreen(SFaceRuntime& face);

	bool IsBackFace(SFaceRuntime& face, EVertexOrder eOrder = EVertexOrder_ClockWise);
	bool IsVertexInCVV(const Vec4& pos);
}