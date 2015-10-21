#pragma once

#include "Utility.h"

struct Color3B
{
	Color3B() : r(0), g(0), b(0)
	{
	}

	Color3B(int _r, int _g, int _b)
	{
		r = _r;
		g = _g;
		b = _b;
	}

	Color3B operator+(const Color3B& rh);
	Color3B operator-(const Color3B& rh);
	Color3B operator*(float fScale);
	Color3B operator/(float fScale);
	Color3B operator*=(float fScale);
	Color3B operator*(const Color3B& rh);
	Color3B operator*=(const Color3B& rh);

	int r;
	int g;
	int b;

	static const Color3B WHITE;
	static const Color3B GREEN;
};

struct Color4F
{
	Color4F() : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
	{
	}

	Color4F(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Color4F(Color3B color3B, float _a)
	{
		r = color3B.r / 255.0f;
		g = color3B.g / 255.0f;
		b = color3B.b / 255.0f;
		a = _a;
	}

	Color4F operator*(const Color4F& rh) const;
	Color4F operator-(const Color4F& rh) const;
	Color4F operator/(float fScalar) const;
	Color4F operator*(float fScalar) const;
	void operator+=(const Color4F& rh);
	Color4F operator+(const Color4F& rh) const;

	void operator*=(float fScalar);

	int ToARGB();
	void Set(float _r, float _g, float _b, float _a);

	float r;
	float g;
	float b;
	float a;

	static const Color4F WHITE;
	static const Color4F GREEN;
	static const Color4F BLACK;
};

struct SFaceData
{
	SFaceData()
	: m_VertexIndex1(0)
	, m_VertexIndex2(0)
	, m_VertexIndex3(0)
	{
	}

	int m_VertexIndex1;
	int m_VertexIndex2;
	int m_VertexIndex3;
};

struct SVertexData
{
	Vec3 m_pos;
	Vec3 m_normal;
	Vec3 m_tangent;
	Vec2 m_UV;
	Color4F m_color;
	Vec4 m_boneIndex;
	Vec4 m_blendWeight;
};

class CVariable
{
public:
	union
	{
		float ft;
		struct{ Vec2 v2; };
		struct{ Vec3 v3; };
		struct{ Vec4 v4; };
		struct{ Color4F color; };
	};

	CVariable()
	{
		memset(this, 0, sizeof(CVariable));
	}

	CVariable operator*(float fScalar) const;
	CVariable operator+(const CVariable& rh) const;
	CVariable operator-(const CVariable& rh) const;

	void operator+=(const CVariable& rh);
	void operator*=(float fScalar);
};

enum EVertexAttributeVar
{
	EVertexAttributeVar_Position,
	EVertexAttributeVar_Normal,
	EVertexAttributeVar_tangent,
	EVertexAttributeVar_UV,
	EVertexAttributeVar_Color,
	EVertexAttributeVar_Max,
};

const int conMaxCustomVar = 3;
class CVertexRuntime
{
public:
	Vec4 m_boneIndex;
	Vec4 m_blendWeight;

	CVariable m_vVertexAttributeVar[EVertexAttributeVar_Max];
	CVariable m_vCustomVariable[conMaxCustomVar];

	CVertexRuntime(){}

	CVertexRuntime(const CVertexRuntime& rh);
	void operator=(const CVertexRuntime& rh);
};

struct SFragment
{
	SFragment() : fAlpha(1.0f){}

	CVariable m_vVertexAttributeVar[EVertexAttributeVar_Max];
	CVariable m_vCustomVariable[conMaxCustomVar];
	float fAlpha;
};

class CRenderState;
class CFaceRuntime
{
public:
	CFaceRuntime()
	: m_pRenderState(nullptr)
	, m_fAlpha(1.0f)
	{
	}

	CVertexRuntime m_vertex1;
	CVertexRuntime m_vertex2;
	CVertexRuntime m_vertex3;
	float m_fAlpha;
	CRenderState* m_pRenderState;
};

struct STextureData
{
	std::string m_sFileName;
};

struct SMaterialData
{
	std::string m_MaterialName;
	std::vector<STextureData> m_SubTextureVec;
};

struct SBoneKey
{
	SBoneKey()
	{
		memset(m_sBoneName, 0, sizeof(m_sBoneName));
	}

	char m_sBoneName[256];
	Vec3 m_translation;
	Quaternion m_rotation;
	Vec3 m_scale;
};

struct SBoneFrame
{
	SBoneFrame()
	: m_iIndex(-1)
	, m_fTime(0.0f)
	{
	}

	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	int m_iIndex;
	float m_fTime;
	std::vector<SBoneKey> m_vKey;
};

struct SBoneData
{
	SBoneData()
	: m_iIndex(-1)
	, m_iParentIndex(-1)
	{
	}

	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	int m_iParentIndex;
	int m_iIndex;
	std::string  m_sName;
	Mat4 m_originalBindMat;
	Mat4 m_inverseBindMat;
	std::vector<int> m_vChildIndex;
};

class SSkeletonData
{
public:
	std::vector<SBoneData> m_vBone;
	std::vector<int> m_vSkinBone;
	std::vector<SBoneFrame> m_vFrame;
};

struct SSubMeshData
{
	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	std::string				m_MeshName;
	Mat4					m_MeshMatrix;
	std::vector<SFaceData>		m_vFace;
	std::vector<SVertexData>	m_vVertex;
	SMaterialData			m_cMaterial;
};

struct SMeshData
{
	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	std::vector<SSubMeshData>	m_vSubMesh;
	SSkeletonData				m_skeleton;
};

struct STransform
{
public:
	STransform()
	{
		m_mat = Mat4::IDENTITY;
		m_scale.set(1.0f, 1.0f, 1.0f);
		m_bTransformDirty = true;
		m_bUseQuaternion = false;
	}

	Mat4 GetTransformMat();
	Mat4 GetRotationMat();
	void Reset();

	const Vec3& GetPosition() const;
	void SetPosition(const Vec3& pos);

	const Vec3& GetRotation() const;
	void SetRotation(const Vec3& rot);
	void SetRotation(const Quaternion& rot);

	const Vec3& GetScale() const;
	void SetScale(const Vec3& scale);

	bool IsTransformDirty();
	void SetUseQuaternion(bool bVal);

private:
	Vec3 m_pos;
	Vec3 m_rotation;
	Quaternion m_quat;
	Vec3 m_scale;

	Mat4 m_mat;

	bool m_bTransformDirty;
	bool m_bUseQuaternion;
};

enum EVertexOrder
{
	EVertexOrder_ClockWise,
	EVertexOrder_Counter_ClockWise,
};

class CVertexShaderBase;
class CFragmentShaderBase;
class CMaterial;
class CRenderState
{
public:
	CRenderState()
	: m_bEnableCullFace(true)
	, m_bDrawWireFrame(true)
	, m_eVertexOrder(EVertexOrder_ClockWise)
	, m_pVertexShader(nullptr)
	, m_pFragmentShader(nullptr)
	{
	}

	bool m_bEnableCullFace;
	bool m_bDrawWireFrame;
	EVertexOrder m_eVertexOrder;
	CVertexShaderBase* m_pVertexShader;
	CFragmentShaderBase* m_pFragmentShader;
};

class CRenderObject
{
public:
	CRenderObject() : m_pVertexShader(nullptr), m_pFragmentShader(nullptr) 
	{
	}

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

	CVertexShaderBase* m_pVertexShader;
	CFragmentShaderBase* m_pFragmentShader;
	STransform m_transform;
	CRenderState m_renderState;
};

typedef shared_ptr<CRenderObject> RenderObjPtr;

class CTexture
{
public:
	CTexture()
		: m_iWidth(0)
		, m_iHeight(0)
		, m_pData(nullptr)
	{
	}

	int m_iWidth;
	int m_iHeight;
	unsigned char* m_pData;
};

class CSampler
{
public:
	enum EUVWrapMode
	{
		EUVWrapMode_Repeat,
		EUVWrapMode_Clamp,
	};

	enum ETextureFilter
	{
		ETextureFilter_Nearest,
		ETextureFilter_Liner,
	};

	CSampler()
		: UV_WRAP_S(EUVWrapMode_Clamp)
		, UV_WRAP_T(EUVWrapMode_Clamp)
		, TEXTURE_MAG_FILTER(ETextureFilter_Nearest)
		, TEXTURE_MIN_FILTER(ETextureFilter_Nearest)
	{
	}

	EUVWrapMode UV_WRAP_S;
	EUVWrapMode UV_WRAP_T;

	ETextureFilter TEXTURE_MAG_FILTER;
	ETextureFilter TEXTURE_MIN_FILTER;
};

class CMaterial
{
public:
	CMaterial()
		: m_baseColorTex(0)
		, m_fShininess(64.0f)
	{
	}

	int GetBaseColorTex();
	void SetBaseColorTexture(const std::string& sFileName);
	void SetShininess(float fShininess);
	float GetShininess();

private:
	int m_baseColorTex;
	float m_fShininess;
};
