#pragma once

#include "DataTypes.h"
#include "RasterizationStage.h"
#include "GeometryStage.h"
#include "ApplicationStage.h"

class CPipeline
{
public:
	void Draw();
	void AddFace(SFaceRuntime* rFace);

	static CPipeline* GetInstance();
private:
	static CPipeline* s_pInstance;
	list<SFaceRuntime*> m_vRenderList;
};