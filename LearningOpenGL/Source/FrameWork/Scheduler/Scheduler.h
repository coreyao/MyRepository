#pragma once

#include <vector>
#include <functional>

class CObject;
class CScheduler
{
public:
	typedef std::function<void(float)> UpdateCallBack;

	void Update(float dt);
	void AddUpdateCallBack(UpdateCallBack cb);
	void ClearUpdateCallBack();

	static CScheduler* GetInstance();

private:
	std::vector<UpdateCallBack> m_updateNormalList;
	CScheduler(){};
	CScheduler(const CScheduler&){}
	void operator=(const CScheduler&){}

	static CScheduler* s_instance;
};