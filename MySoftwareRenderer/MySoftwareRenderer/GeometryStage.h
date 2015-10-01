#pragma once

#include "DataTypes.h"

namespace GeometryStage
{
	void TransformWorldToCamera(SFaceRuntime& face);
	bool NearPlaneCulling(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace);
	bool IsVertexInFrustrum(const Vec4& pos);
	bool TransformCameraToScreen(SFaceRuntime& face);
	bool TransformCameraToScreen(SVertexRuntime& vertex, bool bUseNormalizedPos);
}