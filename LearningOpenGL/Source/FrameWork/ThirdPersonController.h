#pragma once

#include "Utility.h"

class CCamera;
class CMesh;
class CThirdPersonController
{
public:
	CThirdPersonController();

	void SetCharactor(CMesh* pCharactor);

	void Move(float leftAndRight, float upAndDown, float forwardAndBackward);
	void Rotate(float fPitch, float fYaw);

	STransform m_transform;

private:
	void UpdateChildTransform();

	CMesh* m_pCharactor;
	CCamera* m_pCamera;
	Vec3 m_cameraInitPos;
};