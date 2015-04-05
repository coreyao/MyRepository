#pragma once

#include "Utility.h"

class CCamera
{
public:
	CCamera(Vec3 eyePos, Vec3 lookAtDir, Vec3 upDir);

	void Move(float leftAndRight, float upAndDown, float forwardAndBackward);
	void Rotate(float fPitch, float fYaw);

	Mat4 GetViewMat();
	Mat4 GetProjMat();

private:
	void UpdateProjectionViewMat();

	Vec3 m_eyePos;
	Vec3 m_lookAtDir;
	Vec3 m_UpDir;

	float m_fPitch;
	float m_fYaw;

	Mat4 m_viewMat;
	Mat4 m_ProjMat;
};