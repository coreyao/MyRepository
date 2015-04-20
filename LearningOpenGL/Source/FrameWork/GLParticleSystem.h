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
		, m_fCurZRotation(0.0f)
		, m_fCurSpeed(0.0f)
		, m_pEmitter(nullptr)
		, m_theProgram(-1)
		, m_colorTexUnit(0)
		, m_fCurAlpha(255)
	{
		SVertex leftTop;
		leftTop.m_pos.x = -0.5f;
		leftTop.m_pos.y = 0.5f;
		leftTop.m_UV.x = 0;
		leftTop.m_UV.y = 0;
		leftTop.m_color = Color4F::WHITE;
		m_vVertex.push_back(leftTop);

		SVertex rightTop;
		rightTop.m_pos.x = 0.5f;
		rightTop.m_pos.y = 0.5f;
		rightTop.m_UV.x = 1;
		rightTop.m_UV.y = 0;
		rightTop.m_color = Color4F::WHITE;
		m_vVertex.push_back(rightTop);

		SVertex rightBottom;
		rightBottom.m_pos.x = 0.5f;
		rightBottom.m_pos.y = -0.5f;
		rightBottom.m_UV.x = 1;
		rightBottom.m_UV.y = 1;
		rightBottom.m_color = Color4F::WHITE;
		m_vVertex.push_back(rightBottom);

		SVertex leftBottom;
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
	}

	void Init(CEmitter* pParent);
	void Reset();
	void Update(float dt);
	void Render();
	void BuildVBOAndVAO();
	void InitUniform();
	void SetGLProgram(GLuint theProgram);

private:
	float m_fElapsedRatio;
	float m_fCurSpeed;
	float m_fCurLifeTime;
	float m_fCurZRotation;
	float m_fCurSize;
	Color3B m_curColor;
	int m_fCurAlpha;

	CProperty<float> m_SizeOverLifeTime;
	CProperty<Color3B> m_colorOverLifeTime;
	CProperty<int> m_AlphaOverLifeTime;
	CProperty<float> m_ZRotationOverLifeTime;

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

class CEmitterShape
{
public:
	enum EShape
	{
		EShape_Cone,
		EShape_Box,
	};

	enum EEmitFrom
	{
		EEmitFrom_Base,
		EEmitFrom_Base_Shell,
		EEmitFrom_Volume,
		EEmitFrom_Volume_Shell,
	};

	CEmitterShape()
		: m_eShapeType(EShape_Cone)
		, m_eEmitFromType(EEmitFrom_Base)
		, m_fAngle(60.0f)
		, m_fRadius(50.0f)
		, m_bRandomDirection(false)
	{
	}

	EShape GetShape(){return m_eShapeType;}
	void SetShape(EShape eType){m_eShapeType = eType;}

	EEmitFrom GetEmitFrom(){return m_eEmitFromType;}
	void SetEmitFrom(EEmitFrom eEmitFromType) { m_eEmitFromType = eEmitFromType; }

	float GetAngle() { return m_fAngle; }
	void SetAngle(float fAngle) { m_fAngle = fAngle; }

	float GetRadius() { return m_fRadius; }
	void SetRadius(float fRadius) { m_fRadius = fRadius; }

	bool IsRandomDirection() { return m_bRandomDirection; }
	void SetRandomDirection(bool bRandom) { m_bRandomDirection = bRandom; }

	void GeneratePositionAndDirection(Vec3& outPos, Vec3& outDir);

private:
	EShape m_eShapeType;
	EEmitFrom m_eEmitFromType;

	float m_fAngle;
	float m_fRadius;

	bool m_bRandomDirection;
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
		: m_fEmissionRate(1.0f)
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
	void SetEmitMode(EEmitMode mode);
	void SetEmissionRate(float fEmitRate);
	void SetTotalDuration(float fTotalDuration);
	void SetMaxParticles(int iMaxParticles);
	STransform& GetTransformData();
	CEmitterShape& GetEmitterShapeRef();

	CProperty<float>& GetParticleStartSpeedRef() { return m_fParticleStartSpeed; }
	CProperty<float>& GetParticleStartZRotationRef() { return m_fParticleStartZRotation; }
	CProperty<float>& GetParticleStartSizeRef() { return m_fParticleStartSize; }
	CProperty<float>& GetParticleStartLifeTimeRef() { return m_fParticleLifeTime; }
	CProperty<Color3B>& GetParticleStartColorRef()  { return m_particleStartColor; }
	CProperty<float>& GetParticleStartAlphaRef() { return m_fParticleStartAlpha; }

	CProperty<float>& GetParticleSizeOverLifeTimeRef() { return m_sizeOverLifeTime; }
	CProperty<Color3B>& GetParticleColorOverLifeTimeRef() { return m_colorOverLifeTime; }
	CProperty<int>& GetParticleAlphaOverLifeTimeRef() { return m_AlphaOverLifeTime; }
	CProperty<float>& GetParticleZRotationOverLifeTimeRef() { return m_ZRotationOverLifeTime; }

private:
	int m_iMaxParticles;

	STransform m_transform;
	EEmitMode m_emitMode;

	CProperty<float> m_fParticleStartSpeed;
	CProperty<float> m_fParticleStartZRotation;
	CProperty<float> m_fParticleStartSize;
	CProperty<float> m_fParticleLifeTime;
	CProperty<Color3B> m_particleStartColor;
	CProperty<float> m_fParticleStartAlpha;

	CProperty<float> m_sizeOverLifeTime;
	CProperty<Color3B> m_colorOverLifeTime;
	CProperty<int> m_AlphaOverLifeTime;
	CProperty<float> m_ZRotationOverLifeTime;

	float m_fTotalDuration;
	float m_fCurDuration;

	float m_fEmissionRate;
	float m_fCurEmissionTime;

	CEmitterShape m_emiterShape;

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