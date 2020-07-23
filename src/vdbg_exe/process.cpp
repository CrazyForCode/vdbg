#include "process.h"
#include "common.h"
BreakPoint* AddBreakPoint(HANDLE hProcess, LPVOID Addr, int type)
{
    DWORD dwRet;
    BYTE temp;
    BreakPoint* tempBp = NULL;
    switch (type)
    {
    case 0:
        ReadProcessMemory(hProcess, Addr, &temp, 1, &dwRet);
        WriteProcessMemory(hProcess, Addr, "\xcc", 1, &dwRet);
        tempBp = (BreakPoint*)malloc(sizeof(BreakPoint));
        break;
    case 1:
        break;
    case 2:
        break;
    }
    return tempBp;
}
void DisamMem(DWORD Addr, HANDLE hProcess)
{
    csh handle;
    cs_insn* insn;
    size_t count;

    DWORD dwRet;
    BYTE Data[512];
    memset(Data, 0, sizeof(Data));
    ReadProcessMemory(hProcess, (LPCVOID)Addr, &Data, 32, &dwRet);

    if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle)) {
        printf("ERROR: Failed to initialize engine!\n");
        return;
    }

    count = cs_disasm(handle, (unsigned char*)Data, dwRet, 0x1000, 0, &insn);
    if (count) {
        size_t j;

        for (j = 0; j < count; j++) {
            printf("0x%08x\t", insn[j].address);
            printf("%s\t", insn[j].mnemonic);
            printf("%s\n", insn[j].op_str);
        }

        cs_free(insn, count);
    }
    else
        printf("ERROR: Failed to disassemble given code!\n");

    cs_close(&handle);
}
void console(DebugProject* NowProject)
{
    char command[512];
    char argList[4][20];
    BYTE Data[1024];
    HANDLE hProcess = NowProject->hProcess;
    HANDLE hThread = NowProject->hThread;
    BreakPoint* BreakPointList = NowProject->BreakPointList;

    //需要先运行文件的命令: reg、c、x、u
    //不需要先运行文件的命令: help、c、r、bp

    while (1)
    {
        DWORD dwRet = 0;
        DWORD dwWant;

        memset(command, 0, 512);
        memset(argList, 0, sizeof(argList));

        printf(">");
        std::cin.getline(command, 512);

        DivideArg(command, argList);
        if (!strcmp("q", argList[0]))
        {
            TerminalDebugger(NowProject);
        }
        else if (!strcmp("clear", argList[0]))
        {
            system("cls");
        }
        else if (!strcmp("reg", argList[0]))
        {
            GetContext(hThread, dwWant, NULL, TRUE);
        }
        else if (!strcmp("c", argList[0]))
        {
            break;
        }
        else if (!strcmp("x", argList[0]))
        {
            int num;
            int type = 1;
            DWORD Addr;
            if (!strcmp(argList[3], ""))
                type = 1;
            else
            {
                switch (argList[3][0])
                {
                case 'b':type = 1; break;
                case 'd':type = 2; break;
                case 'g':type = 3; break;
                default: type = 1;
                }
            }
            if (!strcmp(argList[2], ""))
                num = 16;
            else
                num = atoi(argList[2]) * type;
            Addr = strtol(argList[1], NULL, 16);
            //计算数量
            do
            {
                if (num > 512)
                {
                    ReadProcessMemory(hProcess, (LPCVOID)Addr, &Data, 512, &dwRet);
                    showData(Data, 512, type, Addr);
                    num -= 512;
                }
                else
                {
                    ReadProcessMemory(hProcess, (LPCVOID)Addr, &Data, num, &dwRet);
                    showData(Data, num, type, Addr);
                    break;
                }

            } while (1);
        }
        else if (!strcmp(argList[0], "help"))
        {
            printf("支持的命令\n"
                "x addr num type (显示某个地址的值 addr为地址 num为显示的数量 type为数据类型，其中包括：b(字节) d(四个字节) g(八个字节))\n"
                "reg (查看寄存器的值)\n"
                "bp addr(添加断点)\n"
                "u addr(查看该地址代码)\n"
                "c (程序继续运行)\n"
                "q (退出)\n"
                "clear (清屏)\n"
                "help\n");
        }
        else if (!strcmp(argList[0], "bp"))
        {
            if (BreakPointList == NULL)
            {
                BreakPointList = AddBreakPoint(hProcess, (LPVOID)strtol(argList[1], NULL, 16), 0);
            }
            else
            {
                BreakPoint* tmp = BreakPointList;
                while (tmp->nextBp != NULL) tmp = tmp->nextBp;
                tmp = AddBreakPoint(hProcess, (LPVOID)strtol(argList[1], NULL, 16), 0);
            }

        }
        else if (!strcmp("u", argList[0]))
        {
            DisamMem(strtol(argList[1], NULL, 16), hProcess);
        }
        else
        {
            printf("Error Command\n");
        }
    }
}

void OnProcessCreated(const CREATE_PROCESS_DEBUG_INFO* pInfo, HANDLE hProcess)
{
    return;
}
void OnThreadCreated(const CREATE_THREAD_DEBUG_INFO* pInfo, HANDLE hThread)
{
    return;
}
void OnException(const EXCEPTION_DEBUG_INFO* pInfo, DWORD& DebugSign, DebugProject* NowProject)
{
    HANDLE hThread = NowProject->hThread;
    HANDLE hProcess = NowProject->hProcess;

    std::wcout << TEXT("An exception was occured.") << std::endl
        << TEXT("Exception code: ") << pInfo->ExceptionRecord.ExceptionCode << std::dec << std::endl;
    if (pInfo->ExceptionRecord.ExceptionCode == 0x80000003)
    {
        BreakPoint* head = NowProject->BreakPointList;
        DWORD dwEip = NULL;
        DWORD dwRet;
        GetContext(hThread, dwEip, "eip", FALSE);
        while (head != NULL)
        {
            if ((DWORD)head->lpAddress == dwEip)
            {
                WriteProcessMemory(hProcess, head->lpAddress, &head->bContent, 1, &dwRet);
                break;
            }
            head = head->nextBp;
        }
    }
    console(NowProject);
    DebugSign = DBG_EXCEPTION_NOT_HANDLED;
    return;
}
//待修改


void OnProcessExited(const EXIT_PROCESS_DEBUG_INFO* pInfo)
{
    return;
}
void OnThreadExited(const EXIT_THREAD_DEBUG_INFO* pInfo)
{
    return;
}
void OnOutputDebugString(const OUTPUT_DEBUG_STRING_INFO* pInfo, HANDLE hProcess, HANDLE hThread)
{
    BYTE* pBuffer = (BYTE*)malloc(pInfo->nDebugStringLength);
    SIZE_T bytesRead;
    ReadProcessMemory(
        hProcess,
        pInfo->lpDebugStringData,
        pBuffer,
        pInfo->nDebugStringLength,
        &bytesRead);
    int requireLen = MultiByteToWideChar(
        CP_ACP,
        MB_PRECOMPOSED,
        (LPCSTR)pBuffer,
        pInfo->nDebugStringLength,
        NULL,
        0);

    TCHAR* pWideStr = (TCHAR*)malloc(requireLen * sizeof(TCHAR));

    MultiByteToWideChar(
        CP_ACP,
        MB_PRECOMPOSED,
        (LPCSTR)pBuffer,
        pInfo->nDebugStringLength,
        pWideStr,
        requireLen);
    wprintf(TEXT("The Debugged Process Output String: %s\n"), pWideStr);
    free(pWideStr);
    pWideStr = NULL;

    return;
}
void OnRipEvent(const RIP_INFO* pInfo)
{

}
void OnDllLoaded(const LOAD_DLL_DEBUG_INFO* pInfo)
{

}
void OnDllUnloaded(const UNLOAD_DLL_DEBUG_INFO* pInfo) {

}
