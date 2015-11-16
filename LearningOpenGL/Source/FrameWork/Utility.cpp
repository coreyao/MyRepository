#include "Utility.h"
#include "Image/ImageManager.h"

int gettimeofday( struct timeval * val, struct timezone * )
{
	if (val)
	{
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency( &liFreq );
		QueryPerformanceCounter( &liTime );
		val->tv_sec     = (long)( liTime.QuadPart / liFreq.QuadPart );
		val->tv_usec    = (long)( liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0 );
	}
	return 0;
}

extern char* GB2312ToUTF8( const char* gb2312 )
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);  
	wchar_t* wstr = new wchar_t[len+1];  
	memset(wstr, 0, len+1);  
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);  
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);  
	char* str = new char[len+1];  
	memset(str, 0, len+1);  
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);  
	if(wstr) delete[] wstr;  
	return str;  
}

void CObject::Update(float dt)
{
}

void CObject::Render()
{
	if (m_renderState.m_bEnableCullFace)
	{
		glEnable(GL_CULL_FACE);

		if (m_renderState.m_bCullBackFace)
			glCullFace(GL_BACK);
		else
			glCullFace(GL_FRONT);

		if (m_renderState.m_eVertexOrder == EVertexOrder_Counter_ClockWise)
			glFrontFace(GL_CCW);
		else
			glFrontFace(GL_CW);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	if (m_renderState.m_bDrawWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_renderState.m_bEnableDepthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if ( m_renderState.m_bEnableDepthWrite )
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);

	glDepthFunc(m_renderState.m_iDepthFunc);

	glEnable(GL_BLEND);
	glBlendFunc(m_renderState.m_iBlendSrc, m_renderState.m_iBlendDst);
}

CRenderState::CRenderState()
: m_bEnableCullFace(true)
, m_bCullBackFace(true)
, m_bDrawWireFrame(false)
, m_bEnableDepthWrite(true)
, m_iDepthFunc(GL_LEQUAL)
, m_bEnableLight(false)
, m_bEnableDepthTest(true)
, m_eVertexOrder(EVertexOrder_ClockWise)
, m_iBlendSrc(GL_SRC_ALPHA)
, m_iBlendDst(GL_ONE_MINUS_SRC_ALPHA)
{
}

CMaterial::CMaterial()
: m_baseColorTex(-1)
, m_normalMapTex(-1)
, m_fShininess(32)
{
}

void CMaterial::SetBaseColorTexture(const std::string& sFileName)
{
	unsigned char* pData = nullptr;
	float fWidth = 0;
	float fHeight = 0;

	CImageManager::GetInstance()->Load(sFileName.c_str(), pData, fWidth, fHeight);
	if (pData)
	{
		glGenTextures(1, &m_baseColorTex);
		glBindTexture(GL_TEXTURE_2D, m_baseColorTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fWidth, fHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);

		glBindTexture(GL_TEXTURE_2D, 0);

		delete[] pData;
	}
}

void CMaterial::SetNormalMapTexture(const std::string& sFileName)
{
	unsigned char* pData = nullptr;
	float fWidth = 0;
	float fHeight = 0;

	CImageManager::GetInstance()->Load(sFileName.c_str(), pData, fWidth, fHeight);
	if (pData)
	{
		glGenTextures(1, &m_normalMapTex);
		glBindTexture(GL_TEXTURE_2D, m_normalMapTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fWidth, fHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);

		glBindTexture(GL_TEXTURE_2D, 0);

		delete[] pData;
	}
}

void CMaterial::SetShininess(float fShininess)
{
	m_fShininess = fShininess;
}

GLuint CMaterial::GetBaseColorTex()
{
	return m_baseColorTex;
}

float CMaterial::GetShininess()
{
	return m_fShininess;
}

GLuint CMaterial::GetNormalMapTex()
{
	return m_normalMapTex;
}
