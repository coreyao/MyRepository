#ifndef CORE_UTILITY_XSTRINGTEMPLATE_H__INCLUDED
#define CORE_UTILITY_XSTRINGTEMPLATE_H__INCLUDED

namespace Core
{
namespace Utility
{

template <typename InputIterator>
XString::XString(InputIterator begin, InputIterator end)
: BasicString(begin, end)
{
}

/*template <typename InputIterator>
XString& XString::Append(InputIterator first, InputIterator last)
{
    mContent.append(first, last);
    return *this;
}

template <typename InputIterator>
void XString::Insert(XIterator p, InputIterator first, InputIterator last)
{
    mContent.insert(p, first, last);
}

template <typename InputIterator>
XString& XString::Replace(XIterator i1, XIterator i2, InputIterator j1, InputIterator j2)
{
    mContent.replace(i1, i2, j1, j2);
    return *this;
}*/

}
}

#endif