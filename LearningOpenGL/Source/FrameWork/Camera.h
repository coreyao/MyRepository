#pragma once

#include "Utility.h"

class CCamera
{
public:
	enum EProjectionMode
	{
		EProjectionMode_Perspective,
		EProjectionMode_Orthographic,
	};

	CCamera(Vec3 eyePos, Vec3 lookAtDir, Vec3 upDir, EProjectionMode eMode = EProjectionMode_Perspective);

	void Move(float leftAndRight, float upAndDown, float forwardAndBackward);
	void Rotate(float fPitch, float fYaw);

	Mat4 GetViewMat();
	Mat4 GetProjMat();

	Vec3 GetEyePos();
	Vec3 GetLookAtDir();

private:
	void UpdateProjectionViewMat();

	Vec3 m_eyePos;
	Vec3 m_lookAtDir;
	Vec3 m_UpDir;

	float m_fPitch;
	float m_fYaw;

	Mat4 m_viewMat;
	Mat4 m_ProjMat;

	EProjectionMode m_eMode;
};