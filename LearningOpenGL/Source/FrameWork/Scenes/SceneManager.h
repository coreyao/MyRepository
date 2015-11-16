#pragma once

#include "BaseScene.h"

class CSceneManager
{
public:
	static CSceneManager* GetInstance();

	void Next();
	void Previous();
	CBaseScene* GetCurScene();

	virtual void Update(float dt);
	virtual void Draw();

private:
	std::vector<CBaseScene*> m_vScene;
	CSceneManager();
	int m_curSceneIndex;

	static CSceneManager* s_instance;
};