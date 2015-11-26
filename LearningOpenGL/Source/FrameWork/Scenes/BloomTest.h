#pragma once

#include "FrameWork/Scenes/BaseScene.h"

class CBloom;
class CMesh;
class CBloomTest : public CBaseScene
{
public:
	virtual void OnEnter();
	virtual void OnExit();

	virtual void Update(float dt);
	virtual void Draw();

	CBloom* m_pBloom;
	std::vector<CMesh*> m_vLighters;
};