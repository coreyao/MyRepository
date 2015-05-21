#pragma once

#include "Utility.h"
#include "Frustrum.h"

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
	void SetCameraPos(const Vec3& dst);
	void Rotate(float fPitch, float fYaw);
	void Zoom(float fDegOffset);

	Mat4 GetViewMat() const;
	Mat4 GetProjMat() const;

	Vec3 GetCameraPos() const;
	Vec3 GetLookAtDir() const;

	float GetFarZ() const;

	const CFrustrum& GetFrustrum() const;
	bool IsInFrustrum(const CAABB& worldRect) const;

private:
	void UpdateProjectionViewMat();
	void UpdateFrustrum();

	Vec3 m_eyePos;
	Vec3 m_lookAtDir;
	Vec3 m_UpDir;

	float m_fNearZ;
	float m_fFarZ;

	float m_fPitch;
	float m_fYaw;
	float m_fFOV;

	Mat4 m_viewMat;
	Mat4 m_ProjMat;

	EProjectionMode m_eMode;

	CFrustrum m_frustrum;
};