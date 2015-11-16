#pragma once

#include <vector>

class CObject;
class CBaseScene
{
public:
	virtual void OnEnter();
	virtual void OnExit();

	virtual void Update(float dt);
	virtual void Draw();

protected:
	std::vector<CObject*> m_vObject;
};