#pragma once

#include <queue>
#include <functional>

class CRenderer
{
public:
	typedef std::function<void(void)> DrawCallBack;

	void AddDrawCallBack(DrawCallBack cb);
	void ClearDrawCallBack();
	void Draw();

	static CRenderer* GetInstance();

private:
	std::vector<DrawCallBack> m_renderingQueue;
	CRenderer(){};
	CRenderer(const CRenderer&){}
	void operator=(const CRenderer&){}

	static CRenderer* s_instance;

};