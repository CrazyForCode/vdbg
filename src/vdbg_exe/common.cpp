#include"common.h"
#include"Debugger.h"
void DivideArg(char* command, char(*argList)[20])
{
    char* pre, * next;
    int cnt;

    pre = command;
    next = command;
    cnt = 0;
    while (*next != '\0')
    {
        while (*next != '\t' && *next != ' ' && *next != '\0')
        {
            next++;
        }
        switch (cnt)
        {
        case 0:strncpy_s(*(argList + 0), pre, (DWORD)next - (DWORD)pre); break;
        case 1:strncpy_s(*(argList + 1), pre, (DWORD)next - (DWORD)pre); break;
        case 2:strncpy_s(*(argList + 2), pre, (DWORD)next - (DWORD)pre); break;
        case 3:strncpy_s(*(argList + 3), pre, (DWORD)next - (DWORD)pre); break;
        }
        cnt++;
        if (*next == '\0')
            break;
        pre = next + 1;
        next = pre;
    }
}

BOOL AsciiToWchar(WCHAR* Dest,char* Src)
{
    swprintf_s(Dest, strlen(Src)+1 , L"%hs", Src);
    //int requireLen = MultiByteToWideChar(CP_ACP,0,(LPCSTR)Src,strlen(Src),NULL,0);
    //memset(Dest, 0, sizeof(Dest));
    //MultiByteToWideChar(CP_ACP,0,(LPCSTR)Src,strlen(Src), Dest,requireLen);
    wprintf(TEXT("%s\n"), Dest);
    
    return TRUE;
}
void TerminalDebugger(DebugProject* NowProject)
{

    CloseHandle(NowProject->hProcess);
    CloseHandle(NowProject->hThread);
    while (NowProject->BreakPointList != NULL)
    {
        BreakPoint* tmp = NowProject->BreakPointList;
        NowProject->BreakPointList = NowProject->BreakPointList->nextBp;
        free(tmp);
    }
    free(NowProject);
    exit(0);
}