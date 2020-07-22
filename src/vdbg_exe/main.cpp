#include <iostream>
#include <Windows.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "process.h"
#include "capstone.h"

#define MAX_LENGTH 100
//#define DEBUGPATH "C:\\Users\\Vioet\\Desktop\\Debugger\\Output\\example\\WaitForDebug.exe"

int main(int argc, char* argv[])
{
    /*if (argc < 2)
    {
        printf("wrong parameter\n");
        exit(0);
    }*/
    char fileName[512] = "C:\\Users\\Vioet\\Desktop\\Debugger\\vdbg\\Debug\\WaitForDebug.exe";
    DEBUG_EVENT debugEvent;
    STARTUPINFO si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };
    BOOL waitEvent = TRUE;
    HANDLE hThread;
    HANDLE hProcess;
    DWORD  DebugSign = DBG_CONTINUE;
    DebugProject* NowProject = (DebugProject*)malloc(sizeof(DebugProject));
    //memset(fileName, 0, sizeof(fileName));
    //strcpy_s(fileName, argv[1]);
    int requireLen = MultiByteToWideChar(
        CP_ACP,
        MB_PRECOMPOSED,
        (LPCSTR)fileName,
        strlen(fileName),
        NULL,
        0);

    TCHAR* pWideStr = (TCHAR*)malloc(requireLen * sizeof(TCHAR));
    memset(pWideStr, 0, requireLen * sizeof(TCHAR));
    MultiByteToWideChar(
        CP_ACP,
        MB_PRECOMPOSED,
        (LPCSTR)fileName,
        strlen(fileName),
        pWideStr,
        requireLen);
    *(pWideStr + requireLen) = 0;
    wprintf(TEXT("%s\n"), pWideStr);

#ifdef DEBUGPATH
    if (NULL == CreateProcess(TEXT(DEBUGPATH), NULL, NULL, NULL, FALSE, DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
#else
    if (NULL == CreateProcess(pWideStr, NULL, NULL, NULL, FALSE, DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
#endif    
    {
        printf("CreateProcess Error[%d]\n", GetLastError());
        return -1;
    }
    NowProject->hProcess = pi.hProcess;
    NowProject->hThread = pi.hThread;
    NowProject->BreakPointList = NULL;

    hProcess = pi.hProcess;
    hThread = pi.hThread;

    while (waitEvent == TRUE && WaitForDebugEvent(&debugEvent, INFINITE))
    {
        DebugSign = DBG_CONTINUE;
        switch (debugEvent.dwDebugEventCode) {

        case CREATE_PROCESS_DEBUG_EVENT:
            OnProcessCreated(&debugEvent.u.CreateProcessInfo, hProcess);
            break;

        case CREATE_THREAD_DEBUG_EVENT:
            OnThreadCreated(&debugEvent.u.CreateThread, hThread);
            break;

        case EXCEPTION_DEBUG_EVENT:
            OnException(&debugEvent.u.Exception, DebugSign, NowProject);
            break;

        case EXIT_PROCESS_DEBUG_EVENT:
            OnProcessExited(&debugEvent.u.ExitProcess);
            waitEvent = FALSE;
            break;

        case EXIT_THREAD_DEBUG_EVENT:
            OnThreadExited(&debugEvent.u.ExitThread);
            break;

        case LOAD_DLL_DEBUG_EVENT:
            OnDllLoaded(&debugEvent.u.LoadDll);
            break;

        case UNLOAD_DLL_DEBUG_EVENT:
            OnDllUnloaded(&debugEvent.u.UnloadDll);
            break;

        case OUTPUT_DEBUG_STRING_EVENT:
            OnOutputDebugString(&debugEvent.u.DebugString, hProcess, hThread);
            break;

        case RIP_EVENT:
            OnRipEvent(&debugEvent.u.RipInfo);
            break;

        default:
            std::wcout << TEXT("Unknown debug event.") << std::endl;
            break;
        }
        if (waitEvent == TRUE) {
            ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DebugSign);
        }
        else {
            break;
        }
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

}
