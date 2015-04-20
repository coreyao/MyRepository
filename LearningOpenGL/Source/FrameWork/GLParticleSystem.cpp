#include "GLParticleSystem.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include "Image/PNGReader.h"
#include "GLMesh.h"

#include <utility>

void GLParticleSystem::AddEmitter( CEmitter* pNewEmitter )
{
	pNewEmitter->m_pParticleSystem = this;
	m_vEmitter.push_back(pNewEmitter);
}

void GLParticleSystem::Update( float dt )
{
	for ( auto& pEmitter : m_vEmitter )
	{
		pEmitter->Update(dt);
	}
}

void GLParticleSystem::Render()
{
	for ( auto& pEmitter : m_vEmitter )
	{
		pEmitter->Render();
	}
}

STransform& GLParticleSystem::GetTransformData()
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
	return m_emiterShape;
}

void CEmitter::SetEmitRate( float fEmitRate )
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

void CParticleInstance::Update( float dt )
{
	m_fCurLifeTime -= dt;
	float fTotalLifeTime = m_pEmitter->m_fParticleLifeTime.GetValue(m_fElapsedRatio);
	m_fCurLifeTime = max(m_fCurLifeTime, 0.0f);

	float fLifeTimeRatio = ( fTotalLifeTime - m_fCurLifeTime ) / fTotalLifeTime;
	m_fCurSize = m_SizeOverLifeTime.GetValue(fLifeTimeRatio, 1.0f);
	m_curColor = m_colorOverLifeTime.GetValue(fLifeTimeRatio, Color3B::WHITE);
	m_fCurAlpha = m_AlphaOverLifeTime.GetValue(fLifeTimeRatio, 255);
	m_fCurZRotation = m_ZRotationOverLifeTime.GetValue(fLifeTimeRatio, 0.0f);

	m_position += m_moveDir * m_fCurSpeed * dt;

	Mat4 viewMatrix = CDirector::GetInstance()->GetCurCamera()->GetViewMat();
	Mat4 TranslationMatrix = Mat4::CreateFromTranslation(m_position.x,m_position.y, m_position.z);
	Mat4 ScaleMatrix = Mat4::CreateFromScale(m_fCurSize, m_fCurSize, m_fCurSize);
	Mat4 ZRotationMatrix = Mat4::CreateFromRotationZ(m_fCurZRotation);

	Mat4 BillboardMatrix = Mat4::IDENTITY;
	Vec3 forward;
	if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Free )
		forward = CDirector::GetInstance()->GetCurCamera()->GetLookAtDir() * (-1);
	else if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Relative )
		forward = m_parentMat.Inverse() * CDirector::GetInstance()->GetCurCamera()->GetLookAtDir() * (-1);
	forward.normalize();
	Vec3 up(0, 1, 0);
	Vec3 right = forward.Cross(up);
	right.normalize();
	up = forward.Cross(right);
	up.normalize();
	BillboardMatrix.SetRight(right.x, right.y, right.z);
	BillboardMatrix.SetForward(forward.x, forward.y, forward.z);
	BillboardMatrix.SetUp(up.x, up.y, up.z);

	if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Free )
	{
		m_MV = viewMatrix * TranslationMatrix * ScaleMatrix * BillboardMatrix * ZRotationMatrix;
	}
	else if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Relative )
	{
		m_parentMat = m_pEmitter->m_pParticleSystem->m_transform.GetTransformMat() * m_pEmitter->m_transform.GetTransformMat();
		m_MV = viewMatrix * m_parentMat * TranslationMatrix * ScaleMatrix * BillboardMatrix * ZRotationMatrix;
	}
}

void CParticleInstance::BuildVBOAndVAO()
{
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vVertex.size() * sizeof(CParticleInstance::SVertex), &m_vVertex.front(), GL_STATIC_DRAW);
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

extern COGLMesh* g_planeMesh;

void CParticleInstance::Render()
{
	glUseProgram(m_theProgram);

	glBindVertexArray(m_vao);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	//glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(false);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_UV));

	GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");	
	if ( modelViewMatrixUnif >= 0 )
	{
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, m_MV.m);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetCurCamera()->GetProjMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	GLint colorUnif = glGetUniformLocation(m_theProgram, "u_color");
	if ( colorUnif >= 0 )
	{
		Color4F color(m_curColor.r / 255.0f, m_curColor.g / 255.0f, m_curColor.b / 255.0f, m_fCurAlpha / 255.0f);
		glUniform4f( colorUnif, color.r, color.g, color.b, color.a );
	}

	if ( m_pEmitter->m_iTexture >= 0 )
	{
		glActiveTexture(GL_TEXTURE0 + m_colorTexUnit);
		glBindTexture(GL_TEXTURE_2D, m_pEmitter->m_iTexture);
		glBindSampler(m_colorTexUnit, m_Sampler);
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
	m_pEmitter->m_emiterShape.GeneratePositionAndDirection(m_position, m_moveDir);

	m_parentMat = m_pEmitter->m_pParticleSystem->m_transform.GetTransformMat() * m_pEmitter->m_transform.GetTransformMat();
	if ( m_pEmitter->m_emitMode == CEmitter::EEmitMode_Free )
	{
		m_position = m_parentMat.TransformPoint(m_position);
		m_moveDir = m_parentMat.TransformVector(m_moveDir);
	}
	m_moveDir.normalize();

	m_fElapsedRatio = m_pEmitter->m_fCurDuration / m_pEmitter->m_fTotalDuration;
	m_fCurSpeed = m_pEmitter->m_fParticleStartSpeed.GetValue(m_fElapsedRatio);
	m_fCurLifeTime = m_pEmitter->m_fParticleLifeTime.GetValue(m_fElapsedRatio);
	m_fCurSize = m_pEmitter->m_fParticleStartSize.GetValue(m_fElapsedRatio, 1.0f);
	m_curColor = m_pEmitter->m_particleStartColor.GetValue(m_fElapsedRatio, Color3B::WHITE);
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

void CEmitterShape::GeneratePositionAndDirection( Vec3& outPos, Vec3& outDir )
{
	if ( m_eEmitFromType == EEmitFrom_Base )
	{
		if ( m_eShapeType == EShape_Cone )
		{
			float fRandomRadius = RANDOM_MINUS1_1() * m_fRadius;
			float fRandomAngle = RANDOM_0_1() * DEGREES_TO_RADIANS(360.0f);

			outPos = Vec3( cosf(fRandomAngle), sinf(fRandomAngle), 0 ) * fRandomRadius;
		}
	}
	else if ( m_eEmitFromType == EEmitFrom_Base_Shell )
	{
		if ( m_eShapeType == EShape_Cone )
		{
			float fRandomAngle = RANDOM_0_1() * DEGREES_TO_RADIANS(360.0f);
			outPos = Vec3( cosf(fRandomAngle), sinf(fRandomAngle), 0 ) * m_fRadius;
		}
	}

	outDir = Vec3(0, 0, 1);
	if ( m_eShapeType == EShape_Cone )
	{
		Mat4 rotateX = Mat4::CreateFromRotationX(RANDOM_MINUS1_1() * m_fAngle);
		Mat4 rotateZ = Mat4::CreateFromRotationZ(RANDOM_MINUS1_1() * m_fAngle);
		outDir = rotateZ * rotateX * outDir;
	}
	else
	{
		if ( m_bRandomDirection )
		{
			outDir = Vec3(RANDOM_MINUS1_1(), RANDOM_MINUS1_1(), RANDOM_MINUS1_1());
		}
	}

	outDir.normalize();
}
