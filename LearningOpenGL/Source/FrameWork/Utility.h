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
class CCtrlBase
{
public:
	virtual T GetValue( float fRatio );
};

template <typename T>
class CLinerCtrl : public CCtrlBase<T>
{
public:
	struct SKeyNode
	{
		T m_value;
		float m_fTimePercent;
	};

	CLinerCtrl();

	void AddKeyNode(const SKeyNode& rKeyNode);
	virtual T GetValue( float fRatio ) override;

protected:
	bool Less(const SKeyNode& lh, const SKeyNode& rh);
	std::vector<SKeyNode> m_vNodeItem;
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
			SKeyNode* pStartKeyNode = &m_vNodeItem[i - 1];
			SKeyNode* pEndKeyNode = &m_vNodeItem[i];
			if ( fRatio >= pStartKeyNode->m_fTimePercent && fRatio <= pEndKeyNode->m_fTimePercent )
			{
				return pStartKeyNode->m_value + ( pEndKeyNode->m_value - pStartKeyNode->m_value ) * fRatio;
			}
		}
	}

	return T();
}

template <typename T>
bool CLinerCtrl<T>::Less( const SKeyNode& lh, const SKeyNode& rh )
{
	return lh.m_fTimePercent < rh.m_fTimePercent;
}

template <typename T>
CLinerCtrl<T>::CLinerCtrl()
	: CCtrlBase<T>()
{
}

template <typename T>
void CLinerCtrl<T>::AddKeyNode( const SKeyNode& rKeyNode )
{
	m_vNodeItem.push_back(rKeyNode);
	std::sort( m_vNodeItem.begin(), m_vNodeItem.end(), Less );
}

template <typename T>
class CCurveCtrl : public CLinerCtrl<T>
{
public:
	virtual T GetValue( float fRatio ) override;
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
	virtual T GetValue( float fRatio ) override;

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

template < typename T >
class CRandomCtrl : public CCtrlBase<T>
{
public:
	void SetValue(const T& rStartValue, const T& rEndValue);
	virtual T GetValue( float fRatio ) override;

private:
	T m_startValue;
	T m_endValue;
};

template < typename T >
T CRandomCtrl<T>::GetValue( float fRatio )
{
	return RANDOM_0_1() * ( m_endValue - m_startValue );
}

template < typename T >
void CRandomCtrl<T>::SetValue( const T& rStartValue, const T& rEndValue )
{
	m_startValue = rStartValue;
	m_endValue = rEndValue;
}

template < typename T >
class CProperty
{
public:
	enum EPropertyType
	{
		EPropertyType_Constant,
		EPropertyType_Liner,
		EPropertyType_Curve,
		EPropertyType_RandomBetweenLiner,
		EPropertyType_RandomBetweenContant,
		EPropertyType_RandomBetweenCurve,
	};

	void SetPropertyType( EPropertyType eType );
	T GetValue( float fRatio );
	void AddCtrl( CCtrlBase<T>* pCtrl );

private:
	std::vector< CCtrlBase<T>* > m_vCtrl;
	EPropertyType m_eType;
};

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
}

template < typename T >
void CProperty<T>::SetPropertyType( EPropertyType eType )
{
	m_eType = eType;
}
