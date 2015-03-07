#ifndef CORE_UTILITY_XSTRING_H__INCLUDED
#define CORE_UTILITY_XSTRING_H__INCLUDED

#include <vector>

#include "BasicDefines.h"
#include "UtilityTypes.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class XString : public BasicString
{
public:
	typedef BasicString::iterator XIterator;
	typedef BasicString::const_iterator XConstIterator;
	typedef BasicString::reverse_iterator XReverseIterator;
	typedef BasicString::const_reverse_iterator XConstReverseIterator;

	XString();
	XString(const XString& other);
	XString(const BasicString& other);
	XString(time_t seconds);
	XString(const XString& str, size_t pos, size_t n);
	XString(const XChar* s, size_t n);
	XString(const XChar* s);
	XString(size_t n, XChar c);
	template <typename InputIterator>
	XString(InputIterator begin, InputIterator end);
	~XString();

	XString& operator=(const XString& other);
	XString& operator=(const BasicString& other);
	XString& operator=(const XChar* s);
	XString& operator=(XChar c);

	XString&		Format(const XChar* format, ...);

	int             ConvertToInt32();
	unsigned int    ConvertToUInt32();
	__int64         ConvertToUInt64();

	std::string     ToMultiCharString();
	std::wstring    ToWideCharString();

	void			ToLower();
	void			ToUpper();

	std::vector<XString> SplitString();

	static void		WideCStringToMultiCString(char* multiCString, size_t multiCStringLen, const wchar_t* wideCString);
	static void		MultiCStringToWideCString(wchar_t* wideCString, size_t wideCStringLen, const char* multiCString);

	static size_t   GetStringLength(const XChar* str);

#ifndef ABANDON_BOOST_SERIALIZATION
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int /* file_version */)
	{
		ar & boost::serialization::base_object<BasicString>(*this);
	}
#endif

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#include "XStringTemplate.h"

#endif

