#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CGLPrimitive
{
public:
	enum EPrimitiveType
	{
		EPrimitiveType_None,
		EPrimitiveType_Line,
		EPrimitiveType_Point,
		EPrimitiveType_Box,
	};

	CGLPrimitive(EPrimitiveType eType);

	void DrawPoint( const Vec3& loc, float fPointSize = 1.0f );
	void DrawLine( const Vec3& startLoc, const Vec3& endLoc );

	void Render();

private:
	void InitVBOAndVAO();
	void InitUniform();
	void SetGLProgram(GLuint theProgram);

private:
	STransform m_transform;
	std::vector<SCommonVertex> m_vVertex;

	Mat4 m_MV;

	GLuint m_theProgram;
	GLuint m_vertexDataObj;
	GLuint m_vertexAttributeObj;

	EPrimitiveType m_eType;

	float m_fPointSize;
};