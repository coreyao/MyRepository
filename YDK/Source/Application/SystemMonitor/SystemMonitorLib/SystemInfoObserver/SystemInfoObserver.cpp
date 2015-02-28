#include "Precomp/Precomp.h"
#include "SystemInfoObserver/SystemInfoObserver.h"
#include "XString/XString.h"

#include "LogSystem.h"

#define _WIN32_DCOM

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

using Core::Utility::XString;
using Core::XChar;

SystemInfoObserver* SystemInfoObserver::mInstance = NULL;

SystemInfoObserver* SystemInfoObserver::GetInstance()
{
    if (mInstance == NULL)
    {
        mInstance = new SystemInfoObserver;
    }

    return mInstance;
}

SystemInfoObserver::SystemInfoObserver()
{
    Init();
}

SystemInfoObserver::~SystemInfoObserver()
{
    if (mSrv != NULL)
    {
        mSrv->Release();
    }

    if (mLoc != NULL)
    {
        mLoc->Release();   
    }

    CoUninitialize();
}

void SystemInfoObserver::Init()
{
    HRESULT hres;
    // Initialize COM.
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        XString message;
        message.Format(TEXT("Failed to initialize COM library. Error code = 0x%X\n"), hres);
        Core::LOG_ERROR(message.c_str());
        assert(false);
    }

    // Initialize 
    hres = CoInitializeSecurity(
        NULL,     
        -1,      // COM negotiates service                  
        NULL,    // Authentication services
        NULL,    // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
        RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
        NULL,             // Authentication info 
        EOAC_NONE,        // Additional capabilities
        NULL              // Reserved
        );

    if (FAILED(hres))
    {
        XString message;
        message.Format(TEXT("Failed to initialize security. Error code = 0x%X\n"), hres);
        Core::LOG_ERROR(message.c_str());
        CoUninitialize();
        assert(false);
    }

    // Obtain the initial locator to Windows Management
    // on a particular host computer.
    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &mLoc);

    if (FAILED(hres))
    {
        XString message;
        message.Format(TEXT("Failed to create IWbemLocator object. Error code = 0x%X\n"), hres);
        Core::LOG_ERROR(message.c_str());
        CoUninitialize();
        assert(false);
    }

    // Connect to the root\cimv2 namespace with the
    // current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = mLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // WMI namespace
        NULL,                    // User name
        NULL,                    // User password
        0,                       // Locale
        NULL,                    // Security flags                 
        0,                       // Authority       
        0,                       // Context object
        &mSrv                    // IWbemServices proxy
        );                              

    if (FAILED(hres))
    {
        XString message;
        message.Format(TEXT("Could not connect. Error code = 0x%X\n"), hres);
        Core::LOG_ERROR(message.c_str());
        mLoc->Release();     
        CoUninitialize();
        assert(false);
    }

    Core::LOG(TEXT("Connected to ROOT\\CIMV2 WMI namespace"));

    // Set the IWbemServices proxy so that impersonation
    // of the user (client) occurs.
    hres = CoSetProxyBlanket(
        mSrv,                         // the proxy to set
        RPC_C_AUTHN_WINNT,            // authentication service
        RPC_C_AUTHZ_NONE,             // authorization service
        NULL,                         // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,       // authentication level
        RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
        NULL,                         // client identity 
        EOAC_NONE                     // proxy capabilities     
        );

    if (FAILED(hres))
    {
        XString message;
        message.Format(TEXT("Could not set proxy blanket. Error code = 0x%X\n"), hres);
        Core::LOG_ERROR(message.c_str());
        mSrv->Release();
        mLoc->Release();     
        CoUninitialize();
        assert(false);
    }
}

VARIANTS SystemInfoObserver::GetInfo(const XString& className, const XString& attributeName)
{
    assert(!className.empty() && !attributeName.empty());
    // Use the IWbemServices pointer to make requests of WMI. 
    // Make requests here:
    // For example, query for all the running processes
    HRESULT hres;
    VARIANTS result;
    IEnumWbemClassObject* pEnumerator = NULL;
    XString queryString;
    queryString.Format(TEXT("SELECT %s FROM %s"), attributeName.c_str(), className.c_str());
    hres = mSrv->ExecQuery(
        bstr_t("WQL"), 
        bstr_t(queryString.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        XString message;
        message.Format(TEXT("Query for processes failed. Error code = 0x%X\n"), hres);
        Core::LOG_ERROR(message.c_str());
        mSrv->Release();
        mLoc->Release();     
        CoUninitialize();
        assert(false);
    }
    else
    { 
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        while (pEnumerator)
        {
            hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            if(0 == uReturn)
            {
                break;
            }

            VARIANT vtProp;

            // Get the value of the Name property
            hres = pclsObj->Get(attributeName.c_str(), 0, &vtProp, 0, 0);
            result.push_back(vtProp);
            pclsObj->Release();
        }
    }

    return result;
}

VARIANTS_ARRAY SystemInfoObserver::GetInfoArray(const XString& className, const std::vector<XString>& attributeNames)
{
    assert(!className.empty());
    
    // Use the IWbemServices pointer to make requests of WMI. 
    // Make requests here:
    HRESULT hres;
    VARIANTS_ARRAY result;
    IEnumWbemClassObject* pEnumerator = NULL;
    XString queryString;
    XString tempAttributes;
    for (size_t i = 0; i < attributeNames.size(); ++i)
    {
        tempAttributes += attributeNames[i];
        if (i == attributeNames.size() - 1)
        {
            break;
        }
        tempAttributes += TEXT(", ");
    }
    queryString.Format(TEXT("SELECT %s FROM %s"), tempAttributes.c_str(), className.c_str());
    hres = mSrv->ExecQuery(
        bstr_t("WQL"), 
        bstr_t(queryString.c_str()),
        WBEM_FLAG_BIDIRECTIONAL | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        XString message;
        message.Format(TEXT("Query for processes failed. Error code = 0x%X\n"), hres);
        Core::LOG_ERROR(message.c_str());
        mSrv->Release();
        mLoc->Release();     
        CoUninitialize();
        assert(false);
    }
    else
    { 
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        std::vector<XString>::const_iterator iter = attributeNames.begin();
        std::vector<XString>::const_iterator endIter = attributeNames.end();
        for (;iter != endIter; ++iter)
        {
            VARIANTS currentResult;
            while (pEnumerator)
            {
                hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

                if(0 == uReturn)
                {
                    break;
                }
                
                VARIANT vtProp;
                hres = pclsObj->Get(iter->c_str(), 0, &vtProp, 0, 0);
                currentResult.push_back(vtProp);
                pclsObj->Release();
            }

            result.push_back(currentResult);
            hres = pEnumerator->Reset();
        }
    }

    pEnumerator->Release();

    return result;
}

std::vector< std::vector<XChar*> > SystemInfoObserver::GetAddressValueArrayWithClassAndAttibuteArray(const XString& className, const std::vector<XString>& attributeNames)
{
    std::vector< std::vector<XChar*> > result;
    VARIANTS_ARRAY variantsArray = GetInfoArray(className, attributeNames);
    for (size_t i = 0; i < variantsArray.size(); ++i)
    {
        std::vector<XChar*> tempAddressArray;
        for (size_t j = 0; j < variantsArray[i].size(); ++j)
        {
            if (variantsArray[i][j].vt != VT_NULL)
            {
                XChar* temp = (variantsArray[i][j]).bstrVal;
                tempAddressArray.push_back(temp);
                VariantClear(&((variantsArray)[i][j]));
            }
            else
            {
                tempAddressArray.push_back(NULL);
            }
        }
        result.push_back(tempAddressArray);
    }

    return result;
}

STRINGS_ARRAY SystemInfoObserver::GetStringValueArrayWithClassAndAttibuteArray(const XString& className, const std::vector<XString>& attributeNames)
{
    STRINGS_ARRAY result;
    VARIANTS_ARRAY variantsArray = GetInfoArray(className, attributeNames);
    for (size_t i = 0; i < variantsArray.size(); ++i)
    {
        STRINGS tempStringArray;
        for (size_t j = 0; j < variantsArray[i].size(); ++j)
        {
            if (variantsArray[i][j].vt != VT_NULL)
            {
                XChar* temp = (variantsArray[i][j]).bstrVal;
                tempStringArray.push_back(temp);
                VariantClear(&((variantsArray)[i][j]));
            }
            else
            {
                tempStringArray.push_back(TEXT(""));
            }
        }
        result.push_back(tempStringArray);
    }

    return result;
}

STRINGS SystemInfoObserver::GetStringValueWithClassAndAttribute(const XString& className, const XString& attributeName)
{
    STRINGS result;
    VARIANTS variants = GetInfo(className, attributeName);
    for (size_t i = 0; i < variants.size(); ++i)
    {
        if (variants[i].vt != VT_NULL)
        {
            XChar* temp = (variants[i]).bstrVal;
            result.push_back(temp);
        }
        else
        {
            result.push_back(TEXT(""));
        }
        VariantClear(&((variants)[i]));
    }

    return result;
}

USHORTS SystemInfoObserver::GetUShortValueWithClassAndAttribute(const XString& className, const XString& attributeName)
{
    USHORTS result;
    VARIANTS variants = GetInfo(className, attributeName);
    for (size_t i = 0; i < variants.size(); ++i)
    {
        result.push_back(((variants)[i]).uiVal);
        VariantClear(&((variants)[i]));
    }

    return result;
}

ULONGS SystemInfoObserver::GetULongValueWithClassAndAttribute(const XString& className, const XString& attributeName)
{
    ULONGS result;
    VARIANTS variants = GetInfo(className, attributeName);
    for (size_t i = 0; i < variants.size(); ++i)
    {
        result.push_back(((variants)[i]).ulVal);
        VariantClear(&(variants)[i]);
    }

    return result;
}


END_NAMESPACE_APPLICATION_SYSTEMMONITOR
