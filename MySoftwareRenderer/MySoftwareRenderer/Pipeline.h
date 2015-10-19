#pragma once

#include "DataTypes.h"
#include "RasterizationStage.h"
#include "GeometryStage.h"
#include "ApplicationStage.h"

class CPipeline
{
public:
	void Draw();
	void AddFace(CFaceRuntime* rFace);

	static CPipeline* GetInstance();
private:
	static CPipeline* s_pInstance;
	list<CFaceRuntime*> m_vRenderList;
};