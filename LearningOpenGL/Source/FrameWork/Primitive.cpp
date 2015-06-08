#include "Primitive.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"

CPrimitive::CPrimitive(EPrimitiveType eType)
	: m_eType(eType)
	, m_fPointSize(1.0f)
{
	InitVBOAndVAO();
}

void CPrimitive::InitVBOAndVAO()
{
	glGenBuffers(1, &m_vertexDataObj);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_vertexAttributeObj);
	glBindVertexArray(0);

	SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("Primitive") );
}

void CPrimitive::InitUniform()
{
	glUseProgram(m_theProgram);

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetCurProjectionMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	glUseProgram(0);
}

void CPrimitive::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
	InitUniform();
}

void CPrimitive::Render()
{
	if ( m_eType == EPrimitiveType_None || m_vVertex.empty() )
		return;

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_theProgram);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, m_vVertex.size() * sizeof(SCommonVertex), &m_vVertex.front(), GL_STATIC_DRAW);

	glBindVertexArray(m_vertexAttributeObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_UV));

	GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
	if ( modelViewMatrixUnif >= 0 )
	{
		Mat4 viewMatrix = CDirector::GetInstance()->GetCurViewMat();
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, (viewMatrix * m_transform.GetTransformMat()).m);
	}

	if ( m_eType == EPrimitiveType_Line )
	{
		glDrawArrays(GL_LINES, 0, m_vVertex.size());
	}
	else if ( m_eType == EPrimitiveType_Point )
	{
		glPointSize(m_fPointSize);
		glDrawArrays(GL_POINTS, 0, m_vVertex.size());
		glPointSize(1.0f);
	}
	else if ( m_eType == EPrimitiveType_Box )
	{

	}

	glUseProgram(0);
	glBindVertexArray(0);
}

void CPrimitive::DrawLine( const Vec3& startLoc, const Vec3& endLoc )
{
	if ( m_eType != EPrimitiveType_Line )
		return;

	m_eType = EPrimitiveType_Line;

	m_vVertex.clear();

	SCommonVertex StartVertex;
	StartVertex.m_pos = startLoc;
	m_vVertex.push_back(StartVertex);

	SCommonVertex EndVertex;
	EndVertex.m_pos = endLoc;
	m_vVertex.push_back(EndVertex);
}

void CPrimitive::DrawPoint( const Vec3& loc, float fPointSize /*= 1.0f*/ )
{
	if ( m_eType != EPrimitiveType_Point )
		return;

	m_eType = EPrimitiveType_Point;
	m_fPointSize = fPointSize;

	SCommonVertex StartVertex;
	StartVertex.m_pos = loc;
	m_vVertex.push_back(StartVertex);
}
