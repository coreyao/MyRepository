#include "Utility.h"
#include "DataTypes.h"

void Helper::LerpVertex(CVertexRuntime* pVertex1, CVertexRuntime* pVertex2, float t, CVertexRuntime* pVertexOut)
{
	for (int eVar = EVertexAttributeVar_Position; eVar < EVertexAttributeVar_Max; ++eVar)
	{
		pVertexOut->m_vVertexAttributeVar[eVar]
			= Helper::Lerp(pVertex1->m_vVertexAttributeVar[eVar], pVertex2->m_vVertexAttributeVar[eVar], t);
	}

	for (int i = 0; i < conMaxCustomVar; ++i)
	{
		pVertexOut->m_vCustomVariable[i] = Helper::Lerp(pVertex1->m_vCustomVariable[i], pVertex2->m_vCustomVariable[i], t);
	}
}
