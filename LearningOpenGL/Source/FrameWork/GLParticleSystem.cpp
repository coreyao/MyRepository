#include "GLParticleSystem.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"

void GLParticleSystem::AddEmitter( CEmitter* pNewEmitter )
{
	pNewEmitter->m_pParent = this;
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

	for (auto& pParticle : m_vActiveParticle)
	{
		pParticle->Update(dt);
		if ( pParticle->m_age >= pParticle->m_lifeTime )
		{
			RecycleParticle(pParticle);
		}
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
	}
	else
	{
		pParticle = new CParticleInstance;
		pParticle->BuildVBOAndVAO();
		pParticle->SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("NormalMesh") );
	}

	pParticle->m_pParent = this;
	m_vActiveParticle.push_back(pParticle);
}

void CEmitter::RecycleParticle(CParticleInstance* pParticle)
{
	pParticle->m_age = 0;

	auto it = std::find(m_vActiveParticle.begin(), m_vActiveParticle.end(), pParticle);
	if ( it != m_vActiveParticle.end() )
	{
		m_vActiveParticle.erase(it);
		m_vInactiveParticle.push_back(pParticle);
	}
}

void CParticleInstance::Update( float dt )
{
	m_age += dt;

	Mat4 rotationMat = m_pParent->m_pParent->m_transform.GetRotationMat() 
		* m_pParent->m_transform.GetRotationMat()
		* m_transform.GetRotationMat();
	Vec3 dir(0, 0, 1);
	dir = rotationMat * dir;

	m_transform.m_pos += dir * m_fSpeed * dt;
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
}

void CParticleInstance::Render()
{
	glUseProgram(m_theProgram);

	glBindVertexArray(m_vao);

	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*) offsetof(CParticleInstance::SVertex, m_UV));
	glBindVertexArray(0);

	Mat4 viewMatrix = CDirector::GetInstance()->GetCurCamera()->GetViewMat();
	Mat4 TranslationMatrix = Mat4::CreateFromTranslation(m_transform.m_pos.x, m_transform.m_pos.y, m_transform.m_pos.z);

	GLuint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
	if ( modelViewMatrixUnif > 0 )
	{
		Mat4 ModelViewMatrix;
		ModelViewMatrix = viewMatrix * TranslationMatrix;
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, ModelViewMatrix.m);
	}

	GLuint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif > 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetCurCamera()->GetProjMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_vbo);
	glDrawElements(GL_TRIANGLES, m_vVertexIndex.size(), GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void CParticleInstance::SetGLProgram( GLuint theProgram )
{
	m_theProgram = theProgram;
}
