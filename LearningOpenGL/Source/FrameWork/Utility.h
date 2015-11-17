#pragma once

#include <winsock.h>
#include <functional>
#include <cassert>
#include <ctime>
#include <sstream>
#include <map>
#include "DataTypes.h"

#ifndef MAX_EXPORTER_MODE
#include "OpenGL/GLFrameWork.h"
#endif

#ifndef SHADER_FILE_DIR
#define SHADER_FILE_DIR std::string("../Resource/Shaders/")
#endif

#ifndef IMAGE_FILE_DIR
#define IMAGE_FILE_DIR std::string("../Resource/Images/")
#endif

#ifndef MESH_FILE_DIR
#define MESH_FILE_DIR std::string("../Resource/Mesh/")
#endif

#ifndef FONT_FILE_DIR
#define FONT_FILE_DIR std::string("../Resource/Font/")
#endif

#ifdef  _WIN64
#define offsetof(s,m)   (size_t)( (ptrdiff_t)&reinterpret_cast<const volatile char&>((((s *)0)->m)) )
#else
#define offsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PI 3.141592f
#define DEG_TO_RAD(x) ((PI) * ((x) / (180.0f))) 
#define RAD_TO_DEG(x) ((180) * ((x) / (PI)) )
#define EPSILON 0.00001f
#define NEARLY_EQUAL(x,y) (fabs((x) - (y)) <= EPSILON)

#define RANDOM_0_1() ((float)rand()/RAND_MAX)
#define RANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)

enum EVertexOrder
{
	EVertexOrder_ClockWise,
	EVertexOrder_Counter_ClockWise,
};

#ifndef MAX_EXPORTER_MODE
class CRenderState
{
public:
	CRenderState();

	bool m_bEnableCullFace;
	bool m_bCullBackFace;
	EVertexOrder m_eVertexOrder;

	bool m_bEnableDepthTest;
	bool m_bEnableDepthWrite;
	GLuint m_iDepthFunc;

	GLenum m_iBlendSrc;
	GLenum m_iBlendDst;

	bool m_bDrawWireFrame;
	bool m_bEnableLight;

	float m_fGamma;
	bool m_bEnableGammaCorrection;
};

class CMaterial
{
public:
	CMaterial();

	GLuint GetBaseColorTex();
	GLuint GetNormalMapTex();
	float GetShininess();

	void SetBaseColorTexture(const std::string& sFileName);
	void SetNormalMapTexture(const std::string& sFileName);
	void SetShininess(float fShininess);

private:
	GLuint m_baseColorTex;
	float m_fShininess;

	GLuint m_normalMapTex;
};

class CObject
{
public:
	virtual void Update(float dt);
	virtual void Render();

	STransform m_transform;
	CRenderState m_renderState;
	GLuint m_theProgram;
};
#endif

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};

extern int gettimeofday(struct timeval *, struct timezone *);
extern char* GB2312ToUTF8(const char* gb2312);

template <typename T>
struct SKeyNode
{
	SKeyNode() : m_value(T()), m_fTimePercent(0.0f) 
	{
	}

	SKeyNode( float fTimePercent, T value )
	{
		m_fTimePercent = fTimePercent;
		m_value = value;
	}

	float m_fTimePercent;
	T m_value;
};

template <typename T>
class CCtrlBase
{
public:
	virtual T GetValue( float fRatio = 0.0f ) = 0;
};

template <typename T>
class CLinerCtrl : public CCtrlBase<T>
{
public:
	CLinerCtrl();

	void AddKeyNode(const SKeyNode<T>& rKeyNode);
	virtual T GetValue( float fRatio = 0.0f ) override;

protected:
	bool Less(const SKeyNode<T>& lh, const SKeyNode<T>& rh);
	std::vector<SKeyNode<T>> m_vNodeItem;
};

template <typename T>
T CLinerCtrl<T>::GetValue( float fRatio )
{
	if ( m_vNodeItem.size() <= 1)
		return T();

	for (int i = 0; i < m_vNodeItem.size(); ++i)
	{
		if ( i > 0 )
		{
			SKeyNode<T>* pStartKeyNode = &m_vNodeItem[i - 1];
			SKeyNode<T>* pEndKeyNode = &m_vNodeItem[i];
			if ( fRatio >= pStartKeyNode->m_fTimePercent && fRatio <= pEndKeyNode->m_fTimePercent )
			{
				return pStartKeyNode->m_value + ( pEndKeyNode->m_value - pStartKeyNode->m_value ) * 
					( (fRatio - pStartKeyNode->m_fTimePercent) / ( pEndKeyNode->m_fTimePercent - pStartKeyNode->m_fTimePercent ));
			}
		}
	}

	return T();
}

template <typename T>
bool CLinerCtrl<T>::Less( const SKeyNode<T>& lh, const SKeyNode<T>& rh )
{
	return lh.m_fTimePercent < rh.m_fTimePercent;
}

template <typename T>
CLinerCtrl<T>::CLinerCtrl()
	: CCtrlBase<T>()
{
}

template <typename T>
void CLinerCtrl<T>::AddKeyNode( const SKeyNode<T>& rKeyNode )
{
	m_vNodeItem.push_back(rKeyNode);
	std::sort( m_vNodeItem.begin(), m_vNodeItem.end(), std::bind(&CLinerCtrl<T>::Less, this, std::placeholders::_1, std::placeholders::_2) );
}

template <typename T>
class CCurveCtrl : public CCtrlBase<T>
{
public:
	virtual T GetValue( float fRatio = 0.0f ) override;
	void AddNode(T node);

private:
	std::vector<T> m_vNodeItem;
};

template <typename T>
void CCurveCtrl<T>::AddNode( T node )
{
	m_vNodeItem.push_back(node);
}

template <typename T>
T CCurveCtrl<T>::GetValue( float fRatio )
{
	if ( m_vNodeItem.size() < 4)
		return T();

	T p0 = m_vNodeItem[0];
	T p1 = m_vNodeItem[1];
	T p2 = m_vNodeItem[2];
	T p3 = m_vNodeItem[3];

	return p0 * ( 1 - fRatio ) * ( 1 - fRatio ) * ( 1 - fRatio ) +
		p1 * fRatio * ( 1 - fRatio ) * ( 1 - fRatio ) * 3 +
		p2 * fRatio * fRatio * ( 1 - fRatio ) * 3 +
		p3 * fRatio * fRatio * fRatio;
}

template <typename T>
class CConstantCtrl : public CCtrlBase<T>
{
public:
	CConstantCtrl();

	void SetValue( const T& rValue );
	virtual T GetValue( float fRatio = 0.0f ) override;

private:
	T m_value;
};

template <typename T>
CConstantCtrl<T>::CConstantCtrl()
	: m_value(T())
{
}

template <typename T>
T CConstantCtrl<T>::GetValue( float fRatio )
{
	return m_value;
}

template <typename T>
void CConstantCtrl<T>::SetValue( const T& rValue )
{
	m_value = rValue;
}

enum EPropertyType
{
	EPropertyType_Constant,
	EPropertyType_Liner,
	EPropertyType_Curve,
	EPropertyType_RandomBetweenLiner,
	EPropertyType_RandomBetweenConstant,
	EPropertyType_RandomBetweenCurve,
};

template < typename T >
class CProperty
{
public:
	EPropertyType GetPropertyType();

	void AddCtrl( CCtrlBase<T>* pCtrl );
	T GetValue( float fRatio = 0.0f, T defaultValue = T() );
	void RandomPickIndex();

	template < typename U >
	void Init( EPropertyType eType, ... )
	{
		m_eType = eType;
		if ( m_eType == EPropertyType_Constant )
		{
			va_list params;
			va_start(params, eType);

			T lValue = va_arg(params, U);
			auto pCtrl = new CConstantCtrl<T>;
			pCtrl->SetValue( lValue );
			AddCtrl(pCtrl);

			va_end(params);
		}
		else if ( m_eType == EPropertyType_Liner )
		{
			va_list params;
			va_start(params, eType);

			auto pCtrl = new CLinerCtrl<T>;
			AddCtrl(pCtrl);

			int iKeyNodeCount = va_arg(params, int);
			for (int i = 0; i < iKeyNodeCount; ++i)
			{
				SKeyNode<T> key = va_arg(params, SKeyNode<T>);
				pCtrl->AddKeyNode(key);
			}

			va_end(params);
		}
		else if ( m_eType == EPropertyType_Curve )
		{
			va_list params;
			va_start(params, eType);

			auto pCtrl = new CCurveCtrl<T>;
			AddCtrl(pCtrl);

			int iKeyNodeCount = va_arg(params, int);
			for (int i = 0; i < iKeyNodeCount; ++i)
			{
				T key = va_arg(params, U);
				pCtrl->AddNode(key);
			}

			va_end(params);
		}
		else if ( m_eType == EPropertyType_RandomBetweenConstant )
		{
			va_list params;
			va_start(params, eType);

			int iNodeCount = va_arg(params, int);
			for (int i = 0; i < iNodeCount; ++i)
			{
				T value = va_arg(params, U);

				auto pCtrl = new CConstantCtrl<T>;
				pCtrl->SetValue(value);
				AddCtrl(pCtrl);
			}

			va_end(params);
		}
		else if ( m_eType == EPropertyType_RandomBetweenLiner )
		{
			va_list params;
			va_start(params, eType);

			int iLinerCount = va_arg(params, int);
			for (int i = 0; i < iLinerCount; ++i)
			{
				auto pCtrl = new CLinerCtrl<T>;
				AddCtrl(pCtrl);

				int iKeyNodeCount = va_arg(params, int);
				for (int i = 0; i < iKeyNodeCount; ++i)
				{
					SKeyNode<T> key = va_arg(params, SKeyNode<T>);
					pCtrl->AddKeyNode(key);
				}
			}

			va_end(params);
		}
		else if ( m_eType == EPropertyType_RandomBetweenCurve )
		{
			va_list params;
			va_start(params, eType);

			int iLinerCount = va_arg(params, int);
			for (int i = 0; i < iLinerCount; ++i)
			{
				auto pCtrl = new CCurveCtrl<T>;
				AddCtrl(pCtrl);

				int iKeyNodeCount = va_arg(params, int);
				for (int i = 0; i < iKeyNodeCount; ++i)
				{
					T key = va_arg(params, U);
					pCtrl->AddNode(key);
				}
			}

			va_end(params);
		}
	}

	CProperty()
		: m_eType(EPropertyType_Constant)
		, m_iRandomPickIdx(0)
	{
	}

protected:
	std::vector< CCtrlBase<T>* > m_vCtrl;
	EPropertyType m_eType;
	int m_iRandomPickIdx;
};

template < typename T >
EPropertyType CProperty<T>::GetPropertyType()
{
	return m_eType;
}

template < typename T >
void CProperty<T>::AddCtrl( CCtrlBase<T>* pCtrl )
{
	m_vCtrl.push_back(pCtrl);
}

template < typename T >
T CProperty<T>::GetValue( float fRatio, T defaultValue /*= T()*/ )
{
	if ( m_vCtrl.empty() )
	{
		return defaultValue;
	}

	if ( m_eType == EPropertyType_Constant || m_eType == EPropertyType_Liner || m_eType == EPropertyType_Curve )
	{
		return m_vCtrl[0]->GetValue( fRatio );
	}
	else if ( m_eType == EPropertyType_RandomBetweenLiner || m_eType == EPropertyType_RandomBetweenConstant || m_eType == EPropertyType_RandomBetweenCurve )
	{
		return m_vCtrl[m_iRandomPickIdx]->GetValue( fRatio );
	}

	return defaultValue;
}

template < typename T >
void CProperty<T>::RandomPickIndex()
{
	if ( m_vCtrl.size() > 0 )
	{
		m_iRandomPickIdx = rand() % m_vCtrl.size();
	}
	else
	{
		m_iRandomPickIdx = 0;
	}
}
