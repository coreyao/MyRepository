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
		: m_fCurLifeTime(0.0f)
		, m_fCurSize(1.0f)
		, m_pEmitter(nullptr)
		, m_theProgram(-1)
		, m_colorTexUnit(0)
	{
		SVertex leftTop;
		leftTop.m_pos.x = -0.5f;
		leftTop.m_pos.y = 0.5f;
		leftTop.m_UV.x = 0;
		leftTop.m_UV.y = 0;
		leftTop.m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
		m_vVertex.push_back(leftTop);

		SVertex rightTop;
		rightTop.m_pos.x = 0.5f;
		rightTop.m_pos.y = 0.5f;
		rightTop.m_UV.x = 1;
		rightTop.m_UV.y = 0;
		rightTop.m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
		m_vVertex.push_back(rightTop);

		SVertex rightBottom;
		rightBottom.m_pos.x = 0.5f;
		rightBottom.m_pos.y = -0.5f;
		rightBottom.m_UV.x = 1;
		rightBottom.m_UV.y = 1;
		rightBottom.m_color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
		m_vVertex.push_back(rightBottom);

		SVertex leftBottom;
		leftBottom.m_pos.x = -0.5f;
		leftBottom.m_pos.y = -0.5f;
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

	void Init(CEmitter* pParent);
	void Reset();
	void Update(float dt);
	void Render();
	void BuildVBOAndVAO();
	void InitUniform();
	void SetGLProgram(GLuint theProgram);

private:
	float m_fCurSpeed;
	float m_fCurLifeTime;
	float m_fCurSize;
	Color4F m_curColor;

	Vec3 m_position;
	Vec3 m_moveDir;
	Mat4 m_MV;
	Mat4 m_parentMat;

	CEmitter* m_pEmitter;

	std::vector<SVertex> m_vVertex;
	std::vector<unsigned short> m_vVertexIndex;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_index_vbo;
	GLuint m_theProgram;

	int m_colorTexUnit;
	GLuint m_Sampler;
};

class CEmitter
{
	friend class GLParticleSystem;
	friend class CParticleInstance;

public:
	enum EEmitMode
	{
		EEmitMode_Relative,
		EEmitMode_Free,
	};

	CEmitter()
		: m_fEmissionRate(10.0f)
		, m_fCurEmissionTime(0)
		, m_fTotalDuration(5.0f)
		, m_fCurDuration(0.0f)
		, m_iMaxParticles(1000)
		, m_iTexture(-1)
		, m_pParticleSystem(nullptr)
		, m_emitMode(EEmitMode_Relative)
	{
	}

	void AddParticle();
	void RecycleParticle(CParticleInstance* pParticle);
	void Update(float dt);
	void Render();
	void SetTexture(const std::string& sTexFileName);

	void InitParticleLifeTime(float fLifeTime);
	void InitParticleLifeTime(float fLifeTimeLH, float fLifeTimeRH);
	//void InitParticleLifeTime( const std::vector< SKeyNode<float> >& vKeyNode );

	void InitParticleStartSpeed(float fStartSpeed);
	void InitParticleStartSpeed(float fStartSpeedLH, float fStartSpeedRH);

	void InitParticleStartZRotation(float fStartZRotation);
	void InitParticleStartZRotation(float fStartZRotationLH, float fStartZRotationRH);

	void InitParticleStartSize(float fStartSize);
	void InitParticleStartSize(float fStartSizeLH, float fStartSizeRH);

	void InitParticleStartColor(Color4F color);
	void InitParticleStartColor(Color4F colorLH, Color4F colorRH);

	void SetEmitMode(EEmitMode mode);
	STransform& GetTransformData();

private:
	int m_iMaxParticles;

	STransform m_transform;
	EEmitMode m_emitMode;

	float m_fTotalDuration;
	float m_fCurDuration;

	float m_fEmissionRate;
	float m_fCurEmissionTime;

	CProperty<float> m_fParticleStartSpeed;
	CProperty<float> m_fParticleStartZRotation;
	CProperty<float> m_fParticleStartSize;
	CProperty<float> m_fParticleLifeTime;
	CProperty<Color4F> m_particleStartColor;

	GLParticleSystem* m_pParticleSystem;
	std::vector<CParticleInstance*> m_vActiveParticle;
	std::vector<CParticleInstance*> m_vInactiveParticle;

	GLuint m_iTexture;
};

class GLParticleSystem
{
	friend class CParticleInstance;
public:
	GLParticleSystem()
	{
	}

	void AddEmitter(CEmitter* pNewEmitter);
	STransform& GetTransformData();

	void Update(float dt);
	void Render();

private:
	STransform m_transform;
	std::vector<CEmitter*> m_vEmitter;
};