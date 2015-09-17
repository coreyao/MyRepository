#pragma once

#include "DataTypes.h"

namespace GeometryStage
{
	void TransformWorldToCamera(SFaceRuntime& face);
	bool FrustrumCulling(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace);
	bool IsVertexInFrustrum(SVertexRuntime& vertex);
	void TransformCameraToScreen(SFaceRuntime& face);
	void TransformCameraToScreen(SVertexRuntime& vertex);
}