#ifndef CORE_UTILITY_BASICTYPES_H__INCLUDED
#define CORE_UTILITY_BASICTYPES_H__INCLUDED

#define BEGIN_NAMESPACE(X) namespace X{
#define END_NAMESPACE(X) }

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

enum E_LOGMODE
{
	LOGMODE_CONSOLE         = 0x00000001,
	LOGMODE_FILE            = 0x00000002,
	LOGMODE_DEBUGWINDOW     = 0x00000004,

	LOGMODE_FORCETO32BITS   = 0xffffffff
};

enum E_MessageLevel
{
	MESSAGE_LEVEL_NO_MESSAGE		= 0,
	MESSAGE_LEVEL_ERROR_MESSAGE		= 1,
	MESSAGE_LEVEL_ALL_MESSAGE		= 2,

	MESSAGE_LEVEL_FORCETO32BITS = 0Xffffffff
};

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

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif