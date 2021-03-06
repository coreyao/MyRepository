#include "ParticleSystem.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include "Image/PNGReader.h"
#include "Mesh.h"

#include <utility>

void CParticleSystem::AddEmitter( CEmitter* pNewEmitter )
{
	pNewEmitter->m_pParticleSystem = this;
	m_vEmitter.push_back(pNewEmitter);
}

void CParticleSystem::Update( float dt )
{
	for ( auto& pEmitter : m_vEmitter )
	{
		pEmitter->Update(dt);
	}
}

void CParticleSystem::Render()
{
	for ( auto& pEmitter : m_vEmitter )
	{
		pEmitter->Render();
	}
}

STransform& CParticleSystem::GetTransformData()
{
	return m_transform;
}

void CEmitter::Update( float dt )
{
	m_fCurDuration += dt;
	if ( m_fCurDuration > m_fTotalDuration )
		m_fCurDuration = 0.0f;

	if ( m_fEmissionRate > 0 )
	{
		float fRate = 1.0f / m_fEmissionRate;
		m_fCurEmissionTime += dt;
		if ( m_fCurEmissionTime >= fRate )
		{
			this->AddParticle();
			m_fCurEmissionTime = 0.0f;
		}
	}

	std::vector<CParticleInstance*> toRecycle;
	for (auto& pParticle : m_vActiveParticle)
	{
		pParticle->Update(dt);
		if ( pParticle->m_fCurLifeTime <= 0 )
		{
			toRecycle.push_back(pParticle);
		}
	}

	for (auto& pParticle : toRecycle)
	{
		RecycleParticle(pParticle);
	}
}

void CEmitter::Render()
{
	for (auto& pParticle : m_vActiveParticle)
	{
		pParticle->Render();
	}
}

void CEmitter::AddParticle()
{
	CParticleInstance* pParticle = nullptr;
	if ( !m_vInactiveParticle.empty() )
	{
		pParticle = m_vInactiveParticle.back();
		m_vInactiveParticle.pop_back();
		pParticle->Reset();
	}
	else
	{
		pParticle = new CParticleInstance;
		pParticle->Init(this);
	}

	m_vActiveParticle.push_back(pParticle);
}

void CEmitter::RecycleParticle(CParticleInstance* pParticle)
{
	auto it = std::find(m_vActiveParticle.begin(), m_vActiveParticle.end(), pParticle);
	if ( it != m_vActiveParticle.end() )
	{
		m_vActiveParticle.erase(it);
		m_vInactiveParticle.push_back(pParticle);
	}
}

void CEmitter::SetTexture( const std::string& sTexFileName )
{
	CPNGReader pngReader(sTexFileName);
	if ( pngReader.GetData() )
	{
		glGenTextures(1, &m_iTexture);
		glBindTexture(GL_TEXTURE_2D, m_iTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pngReader.GetWidth(), pngReader.GetHeight(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pngReader.GetData());

		glBindTexture(GL_TEXTURE_2D, 0);

		delete [] pngReader.GetData();
	}
}

void CEmitter::SetEmitMode( EEmitMode mode )
{
	m_emitMode = mode;
}

STransform& CEmitter::GetTransformData()
{
	return m_transform;
}

CEmitterShape& CEmitter::GetEmitterShapeRef()
{
	return m_EmiterShape;
}

void CEmitter::SetEmissionRate( float fEmitRate )
{
	m_fEmissionRate = fEmitRate;
}

void CEmitter::SetTotalDuration( float fTotalDuration )
{
	m_fTotalDuration = fTotalDuration;
}

void CEmitter::SetMaxParticles(int iMaxParticles)
{
	m_iMaxParticles = iMaxParticles;
}

void CEmitter::SetShaderColor( const Color4F& rColor )
{
	m_ShaderColor = rColor;
}

void CEmitter::SetBlendMode( EBlendMode eMode )
{
	m_eBlendMode = eMode;
}

void CEmitter::SetTextureAnimationInfo( int iRow, int iCol, int iLoopTime )
{
	m_texAnimInfo.z = iRow;
	m_texAnimInfo.w = iCol;
	m_iTexAnimLoopTime = iLoopTime;
}

void CEmitter::SetRenderMode( ERenderMode eRenderMode )
{
	m_eRenderMode = eRenderMode;
}

void CParticleInstance::Update( float dt )
{
	m_fCurLifeTime -= dt;
	m_fCurLifeTime = max(m_fCurLifeTime, 0.0f);
	float fTotalLifeTime = m_pEmitter->m_fParticleLifeTime.GetValue(m_fElapsedRatio);
	float fLifeTimeRatio = ( fTotalLifeTime - m_fCurLifeTime ) / fTotalLifeTime;

	float fFinalSize = m_SizeOverLifeTime.GetValue(fLifeTimeRatio, 1.0f) * m_fStartSize;

	Color4F finalVertexColor = Color4F(m_startVertexColor, 1.0f) * Color4F(m_colorOverLifeTime.GetValue(fLifeTimeRatio, Color3B::WHITE), 1.0f);
	float fFinalAlpha = ( m_fStartVertexAlpha / 255.0f ) * ( m_AlphaOverLifeTime.GetValue(fLifeTimeRatio, 255) / 255.0f );
	m_VertexColor = Color4F( finalVertexColor.r, finalVertexColor.g, finalVertexColor.b, fFinalAlpha );

	m_fCurZRotation = m_ZRotationOverLifeTime.GetValue(fLifeTimeRatio, 0.0f);

	if ( m_pEmitter->m_texAnimInfo.z > 0 && m_pEmitter->m_texAnimInfo.w > 0 )
	{
		float fModifiedRatio = m_pEmitter->m_iTexAnimLoopTime * fLifeTimeRatio - (int)(m_pEmitter->m_iTexAnimLoopTime * fLifeTimeRatio);
		m_iCurTexSheetFrame = m_pEmitter->m_TexSheetFrameOverLifeTime.GetValue(fModifiedRatio);
	}

	m_position += m_moveDir * m_fCurSpeed * dt;

	Mat4 viewMatrix = CDirector::GetInstance()->GetCurViewMat();
	Mat4 TranslationMatrix = Mat4::CreateTranslationMat(m_position.x,m_position.y, m_position.z);
	Mat4 ScaleMatrix = Mat4::CreateScaleMat(fFinalSize, fFinalSize, fFinalSize);
	Mat4 ZRotationMatrix = Mat4::CreateRotationMat(0, 0, m_fCurZRotation);

	Mat4 BillboardMatrix = Mat4::IDENTITY;
	Vec3 forward;
	if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Free )
		forward = CDirector::GetInstance()->GetPerspectiveCamera()->GetLookAtDir() * (-1);
	else if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Relative )
		forward = m_parentMat.GetInversed() * Vec4(CDirector::GetInstance()->GetPerspectiveCamera()->GetLookAtDir() * (-1), 0.0f);
	if ( m_pEmitter->m_eRenderMode == CEmitter::ERenderMode_VerticalBillboard || m_pEmitter->m_eRenderMode == CEmitter::ERenderMode_HorizontalBillboard )
		forward.y = 0;
	forward.Normalize();
	Vec3 up(0, 1, 0);
	Vec3 right = up.Cross(forward);
	right.Normalize();
	up = forward.Cross(right);
	up.Normalize();
	BillboardMatrix.SetRight(Vec3(right.x, right.y, right.z));
	BillboardMatrix.SetForward(Vec3(forward.x, forward.y, forward.z));
	BillboardMatrix.SetUp(Vec3(up.x, up.y, up.z));

	if (m_pEmitter->m_eRenderMode == CEmitter::ERenderMode_HorizontalBillboard)
	{
		BillboardMatrix = m_parentMat.GetInversed() * BillboardMatrix * Mat4::CreateRotationMat(-90, 0, 0);
	}

	if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Free )
	{
		m_MV = viewMatrix * TranslationMatrix * BillboardMatrix * ZRotationMatrix * ScaleMatrix;
	}
	else if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Relative )
	{
		m_parentMat = m_pEmitter->m_pParticleSystem->m_transform.GetTransformMat() * m_pEmitter->m_transform.GetTransformMat();
		m_MV = viewMatrix * m_parentMat * TranslationMatrix * BillboardMatrix * ZRotationMatrix * ScaleMatrix;
	}
}

void CParticleInstance::BuildVBOAndVAO()
{
	SCommonVertex leftTop;
	leftTop.m_pos.x = -0.5f;
	leftTop.m_pos.y = 0.5f;
	leftTop.m_UV.x = 0;
	leftTop.m_UV.y = 0;
	leftTop.m_color = Color4F::WHITE;
	m_vVertex.push_back(leftTop);

	SCommonVertex rightTop;
	rightTop.m_pos.x = 0.5f;
	rightTop.m_pos.y = 0.5f;
	rightTop.m_UV.x = 1;
	rightTop.m_UV.y = 0;
	rightTop.m_color = Color4F::WHITE;
	m_vVertex.push_back(rightTop);

	SCommonVertex rightBottom;
	rightBottom.m_pos.x = 0.5f;
	rightBottom.m_pos.y = -0.5f;
	rightBottom.m_UV.x = 1;
	rightBottom.m_UV.y = 1;
	rightBottom.m_color = Color4F::WHITE;
	m_vVertex.push_back(rightBottom);

	SCommonVertex leftBottom;
	leftBottom.m_pos.x = -0.5f;
	leftBottom.m_pos.y = -0.5f;
	leftBottom.m_UV.x = 0;
	leftBottom.m_UV.y = 1;
	leftBottom.m_color = Color4F::WHITE;
	m_vVertex.push_back(leftBottom);

	m_vVertexIndex.push_back(0);
	m_vVertexIndex.push_back(1);
	m_vVertexIndex.push_back(3);

	m_vVertexIndex.push_back(1);
	m_vVertexIndex.push_back(2);
	m_vVertexIndex.push_back(3);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vVertex.size() * sizeof(SCommonVertex), &m_vVertex.front(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_index_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vVertexIndex.size() * sizeof(unsigned short), &m_vVertexIndex.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_vao);

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

extern CMesh* g_planePolygonMesh;

void CParticleInstance::Render()
{
	glUseProgram(m_theProgram);

	glBindVertexArray(m_vao);

	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);*/
	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(false);

	glEnable(GL_BLEND);
	if ( m_pEmitter->m_eBlendMode == CEmitter::EBlendMode_ADD )
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else if ( m_pEmitter->m_eBlendMode == CEmitter::EBlendMode_ALPHA_BLEND )
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	SetVertexColor();

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_UV));

	GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");	
	if ( modelViewMatrixUnif >= 0 )
	{
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, m_MV.m);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetCurProjectionMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	GLint colorUnif = glGetUniformLocation(m_theProgram, "u_color");
	if ( colorUnif >= 0 )
	{
		glUniform4f( colorUnif, m_pEmitter->m_ShaderColor.r, m_pEmitter->m_ShaderColor.g, m_pEmitter->m_ShaderColor.b, m_pEmitter->m_ShaderColor.a );
	}

	if ( m_pEmitter->m_iTexture >= 0 )
	{
		glActiveTexture(GL_TEXTURE0 + m_colorTexUnit);
		glBindTexture(GL_TEXTURE_2D, m_pEmitter->m_iTexture);
		glBindSampler(m_colorTexUnit, m_Sampler);
	}

	GLint UVAnimUnif = glGetUniformLocation(m_theProgram, "u_UVAnim");
	if ( UVAnimUnif >= 0 )
	{
		if ( m_pEmitter->m_texAnimInfo.z > 0 && m_pEmitter->m_texAnimInfo.w > 0 )
		{
			m_pEmitter->m_texAnimInfo.x = m_iCurTexSheetFrame / (int)m_pEmitter->m_texAnimInfo.w;
			m_pEmitter->m_texAnimInfo.y = m_iCurTexSheetFrame % (int)m_pEmitter->m_texAnimInfo.z;
		}

		glUniform4f( UVAnimUnif, m_pEmitter->m_texAnimInfo.x, m_pEmitter->m_texAnimInfo.y, m_pEmitter->m_texAnimInfo.z, m_pEmitter->m_texAnimInfo.w );
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_vbo);
	glDrawElements(GL_TRIANGLES, m_vVertexIndex.size(), GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindSampler(m_colorTexUnit, 0);
	glUseProgram(0);

	glDepthMask(true);
}

void CParticleInstance::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
}

void CParticleInstance::Reset()
{
	m_pEmitter->m_EmiterShape.GeneratePositionAndDirection(m_position, m_moveDir);

	m_parentMat = m_pEmitter->m_pParticleSystem->m_transform.GetTransformMat() * m_pEmitter->m_transform.GetTransformMat();
	if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Free )
	{
		m_position = m_parentMat * Vec4(m_position, 1.0f);
		m_moveDir = m_parentMat * Vec4(m_moveDir, 0.0f);
	}
	m_moveDir.Normalize();

	m_fElapsedRatio = m_pEmitter->m_fCurDuration / m_pEmitter->m_fTotalDuration;
	m_fCurSpeed = m_pEmitter->m_fParticleStartSpeed.GetValue(m_fElapsedRatio);
	m_fCurLifeTime = m_pEmitter->m_fParticleLifeTime.GetValue(m_fElapsedRatio);
	m_fStartSize = m_pEmitter->m_fParticleStartSize.GetValue(m_fElapsedRatio, 1.0f);
	m_startVertexColor = m_pEmitter->m_particleStartColor.GetValue(m_fElapsedRatio, Color3B::WHITE);
	m_fStartVertexAlpha = m_pEmitter->m_fParticleStartAlpha.GetValue(m_fElapsedRatio, 255.0f);
	m_fCurZRotation = m_pEmitter->m_fParticleStartZRotation.GetValue(m_fElapsedRatio);

	m_SizeOverLifeTime.RandomPickIndex();
	m_colorOverLifeTime.RandomPickIndex();
	m_AlphaOverLifeTime.RandomPickIndex();
	m_ZRotationOverLifeTime.RandomPickIndex();
}

void CParticleInstance::Init( CEmitter* pParent )
{
	m_pEmitter = pParent;
	m_SizeOverLifeTime = m_pEmitter->m_sizeOverLifeTime;
	m_colorOverLifeTime = m_pEmitter->m_colorOverLifeTime;
	m_AlphaOverLifeTime = m_pEmitter->m_AlphaOverLifeTime;
	m_ZRotationOverLifeTime = m_pEmitter->m_ZRotationOverLifeTime;
	m_TexSheetFrameOverLifeTime = m_pEmitter->m_TexSheetFrameOverLifeTime;

	BuildVBOAndVAO();
	SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("Particle") );
	InitUniform();
	Reset();
}

void CParticleInstance::InitUniform()
{
	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_colorTexture");
	if ( colorTextureUnif >= 0 )
	{
		glUniform1i(colorTextureUnif, m_colorTexUnit);
	}
}

void CParticleInstance::SetVertexColor()
{
	for (auto& rVertex : m_vVertex)
	{
		rVertex.m_color = m_VertexColor;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vVertex.size() * sizeof(SCommonVertex), &m_vVertex.front(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CEmitterShape::GeneratePositionAndDirection( Vec3& outPos, Vec3& outDir )
{
	if ( m_eShapeType == EShape_None )
	{
		outPos = Vec3(0, 0, 0);
	}
	else if ( m_eShapeType == EShape_Box )
	{
		outPos = Vec3( RANDOM_MINUS1_1(), RANDOM_MINUS1_1(), RANDOM_MINUS1_1() ) * m_extent;
	}
	else
	{
		if ( m_eEmitFromType == EEmitFrom_Base )
		{
			if ( m_eShapeType == EShape_Cone )
			{
				float fRandomRadius = RANDOM_MINUS1_1() * m_fRadius;
				float fRandomAngle = RANDOM_0_1() * DEG_TO_RAD(360.0f);

				outPos = Vec3( cosf(fRandomAngle), sinf(fRandomAngle), 0 ) * fRandomRadius;
			}
		}
		else if ( m_eEmitFromType == EEmitFrom_Base_Shell )
		{
			if ( m_eShapeType == EShape_Cone )
			{
				float fRandomAngle = RANDOM_0_1() * DEG_TO_RAD(360.0f);
				outPos = Vec3( cosf(fRandomAngle), sinf(fRandomAngle), 0 ) * m_fRadius;
			}
		}
	}

	outDir = Vec3(0, 0, 1);
	if ( m_eShapeType == EShape_Cone )
	{
		Mat4 rotateX = Mat4::CreateRotationMat(RANDOM_MINUS1_1() * m_fAngle, 0, 0);
		Mat4 rotateZ = Mat4::CreateRotationMat(0, 0, RANDOM_MINUS1_1() * m_fAngle);
		outDir = rotateZ * rotateX * Vec4(outDir, 0.0f);
	}
	else
	{
		if ( m_bRandomDirection )
		{
			outDir = Vec3(RANDOM_MINUS1_1(), RANDOM_MINUS1_1(), RANDOM_MINUS1_1());
		}
	}

	outDir.Normalize();
}
