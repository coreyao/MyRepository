#pragma once

#include "DataTypes.h"

namespace GeometryStage
{
	void TransformWorldToCamera(SFaceRuntime& face);
	bool DoClipInClipSpace(SFaceRuntime& face, bool& bAddFace, SFaceRuntime& addFace);
	void TransformCameraToClip(SFaceRuntime& face);
	void TransformClipToScreen(SFaceRuntime& face);
	bool IsVertexInCVV(const Vec4& pos);
}