#ifndef CORE_UTILITY_BASICTYPES_H__INCLUDED
#define CORE_UTILITY_BASICTYPES_H__INCLUDED

#define BEGIN_NAMESPACE_APPLICATION namespace Application{
#define END_NAMESPACE_APPLICATION }

#define BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR namespace Application{namespace SystemMonitor{
#define END_NAMESPACE_APPLICATION_SYSTEMMONITOR }}

namespace Application
{
namespace SystemMonitor
{

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
    TypeName(const TypeName&);                  \
    void operator=(const TypeName&)

#define SAFE_DELETE(thePointer)                 \
do                                              \
{                                               \
    if (thePointer != NULL)                     \
    {                                           \
        delete thePointer;                      \
        thePointer = NULL;                      \
    }                                           \
} while (0)                                     

#define SAFE_DELETE_ARRAY(thePointer)           \
do                                              \
{                                               \
    if (thePointer != NULL)                     \
    {                                           \
        delete [] thePointer;                   \
        thePointer = NULL;                      \
    }                                           \
} while (0)        

}
}

#endif