#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CParticleInstance
{
private:
	friend class CEmitter;

	struct SVertex
	{
		Vec3 m_pos;
		Color4F m_color;
		Vec2 m_UV;
	};

public:
	CParticleInstance()
		: m_fSpeed(5.0f)
		, m_fAcceleration(0.0f)
		, m_lifeTime(3.0f)
		, m_age(0.0f)
		, m_pParent(nullptr)
		, m_theProgram(0)
	{
		SVertex leftTop;
		leftTop.m_pos.x = -1.0f;
		leftTop.m_pos.y = 1.0f;
		leftTop.m_UV.x = 0;
		leftTop.m_UV.y = 0;
		leftTop.m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
		m_vVertex.push_back(leftTop);

		SVertex rightTop;
		rightTop.m_pos.x = 1.0f;
		rightTop.m_pos.y = 1.0f;
		rightTop.m_UV.x = 1;
		rightTop.m_UV.y = 0;
		rightTop.m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
		m_vVertex.push_back(rightTop);

		SVertex rightBottom;
		rightBottom.m_pos.x = 1.0f;
		rightBottom.m_pos.y = -1.0f;
		rightBottom.m_UV.x = 1;
		rightBottom.m_UV.y = 1;
		rightBottom.m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
		m_vVertex.push_back(rightBottom);

		SVertex leftBottom;
		leftBottom.m_pos.x = -1.0f;
		leftBottom.m_pos.y = -1.0f;
		leftBottom.m_UV.x = 0;
		leftBottom.m_UV.y = 1;
		leftBottom.m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
		m_vVertex.push_back(leftBottom);

		m_vVertexIndex.push_back(0);
		m_vVertexIndex.push_back(1);
		m_vVertexIndex.push_back(3);

		m_vVertexIndex.push_back(1);
		m_vVertexIndex.push_back(2);
		m_vVertexIndex.push_back(3);
	}

	void Reset();
	void Update(float dt);
	void Render();
	void BuildVBOAndVAO();
	void SetGLProgram(GLuint theProgram);

	STransform m_transform;

private:
	float m_fSpeed;
	float m_fAcceleration;
	float m_lifeTime;
	float m_age;

	CEmitter* m_pParent;
	std::vector<SVertex> m_vVertex;
	std::vector<unsigned short> m_vVertexIndex;

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_index_vbo;
	GLuint m_theProgram;
};

class CEmitter
{
	friend class GLParticleSystem;
	friend class CParticleInstance;

public:
	CEmitter()
		: m_fEmissionRate(1.0f)
		, m_fCurEmissionTime(0)
		, m_iMaxParticles(500)
		, m_pParent(nullptr)
	{
	}

	void AddParticle();
	void RecycleParticle(CParticleInstance* pParticle);
	void Update(float dt);
	void Render();

	STransform m_transform;
private:
	float m_fEmissionRate;
	float m_fCurEmissionTime;

	int m_iMaxParticles;

	GLParticleSystem* m_pParent;
	std::vector<CParticleInstance*> m_vActiveParticle;
	std::vector<CParticleInstance*> m_vInactiveParticle;

};

class GLParticleSystem
{
	friend class CParticleInstance;
public:
	GLParticleSystem()
	{
	}

	void AddEmitter(CEmitter* pNewEmitter);

	void Update(float dt);
	void Render();

private:
	STransform m_transform;

	std::vector<CEmitter*> m_vEmitter;
};