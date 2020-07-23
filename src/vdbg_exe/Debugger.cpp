#include "Debugger.h"

BOOL GetContext(HANDLE hThread, DWORD& dwWant, const char* reg, BOOL isPrint)
{
    CONTEXT lpContext;
    lpContext.ContextFlags = CONTEXT_FULL;//获取所有寄存器的值

    if (NULL == GetThreadContext(hThread, &lpContext))
    {
        printf("GetThreadContext Error [%d]\n", GetLastError());
        return FALSE;
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
    return TRUE;
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