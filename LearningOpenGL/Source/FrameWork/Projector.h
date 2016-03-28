#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CProjector
{
public:
	CProjector(const std::string& sFileName);

	void Update(float dt);

	GLuint m_colorTex;
	STransform worldTransform;
	Mat4 ViewMat;

	float FOV;
	float AspectRatio;
	Mat4 ProjectionMat;
};