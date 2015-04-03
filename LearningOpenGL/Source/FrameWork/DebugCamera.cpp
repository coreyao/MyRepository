#include "DebugCamera.h"

CDebugCamera::CDebugCamera()
	: m_fPitch(0)
	, m_fYaw(90)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	float eyeZ = CCDirector::sharedDirector()->getZEye();
	m_eyePos = Vec3(winSize.width/2, winSize.height/2, eyeZ);
	m_lookAtDir = Vec3(0, 0, -1);
	m_UpDir = Vec3(0, 1, 0);
	m_viewProjMat = Mat4::IDENTITY;
}

cocos2d::Mat4 CDebugCamera::GetViewMat()
{
	return m_viewProjMat;
}

void CDebugCamera::Move( float leftAndRight, float upAndDown, float forwardAndBackward )
{
	Vec3 forwardAndBackwardOffset = m_lookAtDir * forwardAndBackward;
	//forwardAndBackwardOffset.y = 0;
	m_eyePos += forwardAndBackwardOffset;

	m_eyePos.y += upAndDown;

	Vec3 dirX;
	Vec3::cross(m_lookAtDir, m_UpDir, &dirX);
	dirX.normalize();
	dirX.y = 0;
	m_eyePos += dirX * leftAndRight;

	UpdateProjectionViewMat();
}

void CDebugCamera::UpdateProjectionViewMat()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	Mat4 matrixPerspective, matrixLookup;

	m_lookAtDir.normalize();

	Mat4::createPerspective(60
		, (GLfloat)winSize.width/winSize.height
		, 10
		, 10000/*Director::getInstance()->getZEye() + winSize.height/2*/
		, &matrixPerspective);

	Mat4::createLookAt(m_eyePos, m_eyePos + m_lookAtDir, m_UpDir, &matrixLookup);
	matrixPerspective *= matrixLookup;

	m_viewProjMat = matrixPerspective;

	Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, m_viewProjMat);
}

void CDebugCamera::EnableCamera(bool bEnable)
{
	if ( bEnable )
	{
		Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
		UpdateProjectionViewMat();
	}
	else
	{
		RecoverCamera();
	}
}

CDebugCamera::~CDebugCamera()
{
}

void CDebugCamera::RecoverCamera()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	float eyeZ = CCDirector::sharedDirector()->getZEye();
	m_eyePos = Vec3(winSize.width/2, winSize.height/2, eyeZ);
	m_lookAtDir = Vec3(0, 0, -1);
	m_UpDir = Vec3(0, 1, 0);
	m_viewProjMat = Mat4::IDENTITY;

	m_fYaw = 90;
	m_fPitch = 0;

	Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
}

void CDebugCamera::Rotate( float fPitch, float fYaw )
{
	m_fYaw += fYaw;
	m_fPitch += fPitch;

	m_lookAtDir.x = cosf(CC_DEGREES_TO_RADIANS(m_fPitch)) * cosf(CC_DEGREES_TO_RADIANS(m_fYaw));
	m_lookAtDir.y = sinf(CC_DEGREES_TO_RADIANS(m_fPitch));
	m_lookAtDir.z = -cosf(CC_DEGREES_TO_RADIANS(m_fPitch)) * sinf(CC_DEGREES_TO_RADIANS(m_fYaw));

	UpdateProjectionViewMat();
}
