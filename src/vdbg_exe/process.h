#pragma once
#include<Windows.h>
#include"capstone.h"
//常规函数
class BreakPoint {
public:
    LPVOID lpAddress;
    BYTE   bContent;

    BreakPoint* nextBp;
    BreakPoint()
    {
        lpAddress = NULL;
        bContent = 0;
        nextBp = NULL;
    }
};
class DebugProject {
public:
    HANDLE hProcess;
    HANDLE hThread;
    BreakPoint* BreakPointList;

    DebugProject()
    {
        hProcess = NULL;
        hThread = NULL;
        BreakPointList = NULL;
    }
};

void GetContext(HANDLE hThread, DWORD& dwWant, const char* reg, BOOL isPrint)
{
    CONTEXT lpContext;
    lpContext.ContextFlags = CONTEXT_FULL;//获取所有寄存器的值
    if (NULL == GetThreadContext(hThread, &lpContext))
    {
        printf("GetThreadContext Error [%d]\n", GetLastError());
        return;
    }
    if (isPrint)
    {
        std::cout << "eax:" << lpContext.Eax << std::endl
            << "ebx:" << lpContext.Ebx << std::hex << std::endl
            << "ecx:" << lpContext.Ecx << std::hex << std::endl
            << "edx:" << lpContext.Edx << std::hex << std::endl
            << "eip:" << lpContext.Eip << std::hex << std::endl
            << "ebp:" << lpContext.Ebp << std::hex << std::endl
            << "esp:" << lpContext.Esp << std::hex << std::endl;
    }
    if (reg != NULL)
    {
        if (!strcmp(reg, "eax"))
            dwWant = lpContext.Eax;
        else if (!strcmp(reg, "ebx"))
            dwWant = lpContext.Ebx;
        else if (!strcmp(reg, "ecx"))
            dwWant = lpContext.Ecx;
        else if (!strcmp(reg, "edx"))
            dwWant = lpContext.Edx;
        else if (!strcmp(reg, "eip"))
            dwWant = lpContext.Eip;
        else if (!strcmp(reg, "ebp"))
            dwWant = lpContext.Ebp;
        else if (!strcmp(reg, "esp"))
            dwWant = lpContext.Esp;
    }
}
void showData(BYTE* Data, int num, int type, DWORD Addr)
{
    DWORD address = Addr;
    switch (type)
    {
    case 1:
        printf("address\t\t0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f\n");
        for (int i = 0; i < num / 16; i++)
        {
            printf("%8p\t", Addr);
            for (int j = 0; j < 16; j++)
            {
                printf("%02x  ", Data[i + j]);
            }
            printf("\n");
            Addr += 16;
        }
        break;
    case 2:
        printf("address\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\ta\tb\tc\td\te\tf\n");
        for (int i = 0; i < num / 16; i++)
        {
            printf("%016p\t", Addr);
            for (int j = 0; j < 4; j++)
            {
                printf("%02x\t", Data[i * j]);
            }
            Addr += 16;
        }
    }
}
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
            printf("%08x\t", insn[j].address);
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
            CloseHandle(hProcess);
            CloseHandle(hThread);
            exit(0);
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

//异常触发的动作

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
