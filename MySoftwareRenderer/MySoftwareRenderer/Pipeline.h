#pragma once

#include "DataTypes.h"

class CPipeline
{
public:
	void Run();
	void AddFace(const SFaceRuntime& rFace );

	static CPipeline* GetInstance();
private:
	static CPipeline* s_pInstance;
	list<SFaceRuntime> m_vRenderList;
};