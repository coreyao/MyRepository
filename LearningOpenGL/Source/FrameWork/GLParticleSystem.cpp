#include "GLParticleSystem.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include "Image/PNGReader.h"
#include "GLMesh.h"

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

void CEmitter::Update( float dt )
{
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
		if ( pParticle->m_fCurLifeTime < 0 )
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

void CParticleInstance::Update( float dt )
{
	m_fCurLifeTime -= dt;

	//m_position += m_moveDir * m_fCurSpeed * dt;
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

	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);*/

	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_UV));

	Mat4 viewMatrix = CDirector::GetInstance()->GetCurCamera()->GetViewMat();
	Mat4 TranslationMatrix = Mat4::CreateFromTranslation(m_position.x,m_position.y, m_position.z);
	Mat4 BillboardMatrix = Mat4::IDENTITY;

	//Vec3 forward = CDirector::GetInstance()->GetCurCamera()->GetEyePos() - m_pEmitter->m_pParticleSystem->m_transform.GetTransformMat() * m_pEmitter->m_transform.GetTransformMat() * TranslationMatrix * Vec3(0, 0, 0);
	//forward.normalize();
	//Vec3 up(0, 1, 0);
	//Vec3 right = up.Cross(forward);
	//right.normalize();
	//up = forward.Cross(right);
	//up.normalize();
	//BillboardMatrix.SetForward(forward.x, forward.y, forward.z);
	//BillboardMatrix.SetRight(right.x, right.y, right.z);
	//BillboardMatrix.SetUp(up.x, up.y, up.z);

	BillboardMatrix = Mat4::CreateFromRotationY(-g_planeMesh->m_rotation.z);

	GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");	
	if ( modelViewMatrixUnif >= 0 )
	{
		Mat4 ModelViewMatrix;
		ModelViewMatrix = viewMatrix * m_pEmitter->m_pParticleSystem->m_transform.GetTransformMat() * m_pEmitter->m_transform.GetTransformMat() * TranslationMatrix * BillboardMatrix;
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, ModelViewMatrix.m);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetCurCamera()->GetProjMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
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
}

void CParticleInstance::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
}

void CParticleInstance::Reset()
{
	//m_position = Vec3(RANDOM_MINUS1_1(), 0, 0);
	m_fCurSpeed = m_pEmitter->m_fParticleStartSpeed;
	m_fCurLifeTime = m_pEmitter->m_fParticleLifeTime;
	m_fCurSize = m_pEmitter->m_fParticleStartSize;
	m_moveDir = Vec3(0, 1, 0);
	m_moveDir.normalize();
}

void CParticleInstance::Init( CEmitter* pParent )
{
	m_pEmitter = pParent;

	BuildVBOAndVAO();
	SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("Particle") );
	InitUniform();
	Reset();
}

void CParticleInstance::InitUniform()
{
	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "colorTexture");
	if ( colorTextureUnif >= 0 )
	{
		glUniform1i(colorTextureUnif, m_colorTexUnit);
	}
}
