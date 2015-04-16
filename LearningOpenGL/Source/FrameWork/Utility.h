#pragma once

#include <winsock.h>
#include "DataTypes.h"

#ifndef SHADER_FILE_DIR
#define SHADER_FILE_DIR std::string("../Resource/Shaders/")
#endif

#ifndef IMAGE_FILE_DIR
#define IMAGE_FILE_DIR std::string("../Resource/Images/")
#endif

#ifndef MESH_FILE_DIR
#define MESH_FILE_DIR std::string("../Resource/Mesh/")
#endif

#ifdef  _WIN64
#define offsetof(s,m)   (size_t)( (ptrdiff_t)&reinterpret_cast<const volatile char&>((((s *)0)->m)) )
#else
#define offsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
#endif

#define RANDOM_0_1() ((float)rand()/RAND_MAX)
#define RANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};

extern int gettimeofday(struct timeval *, struct timezone *);

template <typename T>
struct SKeyNode
{
	T m_value;
	float m_fTimePercent;
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
		return;

	for (int i = 0; i < m_vNodeItem.size(); ++i)
	{
		if ( i > 0 )
		{
			SKeyNode<T>* pStartKeyNode = &m_vNodeItem[i - 1];
			SKeyNode<T>* pEndKeyNode = &m_vNodeItem[i];
			if ( fRatio >= pStartKeyNode->m_fTimePercent && fRatio <= pEndKeyNode->m_fTimePercent )
			{
				return pStartKeyNode->m_value + ( pEndKeyNode->m_value - pStartKeyNode->m_value ) * fRatio;
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
	std::sort( m_vNodeItem.begin(), m_vNodeItem.end(), Less );
}

template <typename T>
class CCurveCtrl : public CLinerCtrl<T>
{
public:
	virtual T GetValue( float fRatio = 0.0f ) override;
};

template <typename T>
T CCurveCtrl<T>::GetValue( float fRatio )
{
	if ( m_vNodeItem.size() < 4)
		return;

	T p0 = m_vNodeItem[0].m_value;
	T p1 = m_vNodeItem[1].m_value;
	T p2 = m_vNodeItem[2].m_value;
	T p3 = m_vNodeItem[3].m_value;

	return p0 * ( 1 - fRatio ) * ( 1 - fRatio ) * ( 1 - fRatio ) +
		3 * p1 * fRatio * ( 1 - fRatio ) * ( 1 - fRatio ) +
		3 * p2 * fRatio * fRatio * ( 1 - fRatio ) +
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
	EPropertyType_RandomBetweenContant,
	EPropertyType_RandomBetweenCurve,
};

template < typename T >
class CProperty
{
public:
	void SetPropertyType( EPropertyType eType );
	EPropertyType GetPropertyType();

	void AddCtrl( CCtrlBase<T>* pCtrl );
	T GetValue( float fRatio = 0.0f );
	void SetValue( const T& rValue );

private:
	std::vector< CCtrlBase<T>* > m_vCtrl;
	EPropertyType m_eType;
};

template < typename T >
EPropertyType CProperty<T>::GetPropertyType()
{
	return m_eType;
}

template < typename T >
void CProperty<T>::SetValue( const T& rValue )
{
	if ( m_eType == EPropertyType_Constant )
	{
		CConstantCtrl<T>* pCtrl = dynamic_cast<CConstantCtrl<T>*>(m_vCtrl[0]);
		if ( pCtrl )
			return pCtrl->SetValue( rValue );
	}
}

template < typename T >
void CProperty<T>::AddCtrl( CCtrlBase<T>* pCtrl )
{
	m_vCtrl.push_back(pCtrl);
}

template < typename T >
T CProperty<T>::GetValue( float fRatio )
{
	if ( m_eType == EPropertyType_Constant || m_eType == EPropertyType_Liner || m_eType == EPropertyType_Curve )
	{
		return m_vCtrl[0]->GetValue( fRatio );
	}
	else if ( m_eType == EPropertyType_RandomBetweenLiner || m_eType == EPropertyType_RandomBetweenContant || m_eType == EPropertyType_RandomBetweenCurve )
	{
		int iIndex = rand() % m_vCtrl.size();
		return m_vCtrl[iIndex]->GetValue( fRatio );
	}

	return T();
}

template < typename T >
void CProperty<T>::SetPropertyType( EPropertyType eType )
{
	m_eType = eType;
}
