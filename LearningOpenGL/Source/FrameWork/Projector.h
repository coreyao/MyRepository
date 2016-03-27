#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CProjector : public CObject
{
public:
	CProjector(const std::string& sFileName);

	virtual void Update(float dt);
	virtual void Render();

	GLuint m_colorTex;
	STransform worldTransform;
};