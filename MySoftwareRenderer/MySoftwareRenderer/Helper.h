#ifndef CPPYIN_Helper
#define CPPYIN_Helper

namespace Helper
{
	template<typename T>
	void Swap(T& a, T& b);
}

template<typename T>
void Helper::Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

#endif