#include "process.h"
#include "capstone.h"
#include"global.h"
#include"common.h"

#define DEBUGPATH "C:\\Users\\Vioet\\Desktop\\Debugger\\Output\\example\\WaitForDebug.exe"

//程序启动时的必备工作
BOOL init(int argc,char *argv[], STARTUPINFO& si, PROCESS_INFORMATION& pi)
{
    WCHAR wszFileName[512];
#ifdef DEBUGPATH
    if (argc < 2)
    {
        printf("Wrong Parameter\n");
        exit(0);
    }
    AsciiToWchar(wszFileName, argv[1]);
    if (NULL == CreateProcess(wszFileName, NULL, NULL, NULL, FALSE, DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
#else
    if (NULL == CreateProcess(TEXT(DEBUGPATH), NULL, NULL, NULL, FALSE, DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
#endif    
    {
        printf("CreateProcess Error[%d]\n", GetLastError());
        return FALSE;
    }
    return TRUE;
}
int main(int argc, char* argv[])
{
    DEBUG_EVENT debugEvent;
    BOOL waitEvent = TRUE;
    DWORD  DebugSign = DBG_CONTINUE;
    STARTUPINFO si = { 0 }; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };
    DebugProject* NowProject = (DebugProject*)malloc(sizeof(DebugProject));

    init(argc,argv,si,pi);

    NowProject->hProcess = pi.hProcess;
    NowProject->hThread = pi.hThread;
    NowProject->BreakPointList = NULL;

    console(NowProject);
    NowProject->isRunning = TRUE;
    while (1)
    {
        while (waitEvent == TRUE && WaitForDebugEvent(&debugEvent, INFINITE))
        {
            DebugSign = DBG_CONTINUE;
            switch (debugEvent.dwDebugEventCode) {

            case CREATE_PROCESS_DEBUG_EVENT:
                OnProcessCreated(&debugEvent.u.CreateProcessInfo, NowProject->hProcess);
                break;

            case CREATE_THREAD_DEBUG_EVENT:
                OnThreadCreated(&debugEvent.u.CreateThread, NowProject->hThread);
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
                OnOutputDebugString(&debugEvent.u.DebugString, NowProject->hProcess, NowProject->hThread);
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
    }
    TerminalDebugger(NowProject);
}
