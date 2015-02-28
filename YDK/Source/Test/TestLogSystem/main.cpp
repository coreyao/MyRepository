#include <windows.h>
#include <cassert>
#include "LogSystem.h"

void FunctionA()
{
    Core::LOG(TEXT("CALL FUNCTIONA"));
}

void FunctionB()
{
    Core::LOG(TEXT("CALL FUNCTIONB"));
    Core::LOG_ERROR(TEXT("error"));
    //assert(false);
}

void FunctionC()
{
    Core::LOG(TEXT("CALL FUNCTIONC"));
}

int main()
{
    Core::LOG(TEXT("--------------Begin testing LogSystem--------------"));
    FunctionA();
    FunctionB();
    FunctionC();
    Core::LOG(TEXT("--------------End   testing LogSystem--------------"));
    system("pause");
    return 0;
}