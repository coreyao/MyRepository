#pragma once

#include "DataTypes.h"

namespace GeometryStage
{
	void TransformWorldToCamera(SFaceRuntime& face);
	bool CameraNearPlaneCulling(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace);
	void TransformCameraToClip(SFaceRuntime& face);
	void TransformCameraToScreen(SFaceRuntime& face);
	bool DoClipInClipSpaceWithoutNear(SFaceRuntime& face);
	void TransformClipToScreen(SFaceRuntime& face);
	bool IsVertexInFrustrum(const Vec4& pos);
}