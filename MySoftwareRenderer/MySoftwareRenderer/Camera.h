#pragma once

#include "Utility.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "DataTypes.h"

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
	void Zoom(float fDegOffset);

	const Mat4& GetViewMat() const;
	const Mat4& GetProjMat() const;

	Vec3 GetCameraPos() const;
	void SetCameraPos(const Vec3& pos);

	Vec3 GetLookAtDir();

	float GetNearZ() const;
	float GetFarZ() const;

	float GetFOV() const;
	float GetAspectRatio() const;

private:
	void UpdateProjectionViewMat();

	STransform m_transform;
	float m_fNearZ;
	float m_fFarZ;

	float m_fFOV;
	float m_aspectRatio;

	Mat4 m_viewMat;
	Mat4 m_ProjMat;

	EProjectionMode m_eMode;
};