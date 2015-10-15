#include "Utility.h"
#include "DataTypes.h"

void Helper::LerpVertex(SVertexRuntime* pVertex1, SVertexRuntime* pVertex2, float t, SVertexRuntime* pVertexOut)
{
	for (auto& pVertexAttrPair : pVertex1->m_vVertexAttributeVar)
	{
		pVertexOut->m_vVertexAttributeVar[pVertexAttrPair.first]
			= Helper::Lerp(pVertex1->m_vVertexAttributeVar[pVertexAttrPair.first], pVertex2->m_vVertexAttributeVar[pVertexAttrPair.first], t);
	}
	for (unsigned int i = 0; i < pVertex1->m_vCustomVariable.size(); ++i)
	{
		pVertexOut->m_vCustomVariable[i] = Helper::Lerp(pVertex1->m_vCustomVariable[i], pVertex2->m_vCustomVariable[i], t);
	}
}
