#ifndef __Project__DebugCamera__
#define __Project__DebugCamera__

#include "../Client.h"

class CDebugCamera
{
public:
	CDebugCamera();
	~CDebugCamera();

	void RecoverCamera();
	void EnableCamera(bool bEnable);

	void Move(float leftAndRight, float upAndDown, float forwardAndBackward);
	void Rotate(float fPitch, float fYaw);

	Mat4 GetViewMat();

private:
	void UpdateProjectionViewMat();

	Vec3 m_eyePos;
	Vec3 m_lookAtDir;
	Vec3 m_UpDir;

	float m_fPitch;
	float m_fYaw;

	Mat4 m_viewProjMat;
};

#endif
