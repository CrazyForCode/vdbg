#pragma once
#include"global.h"

//每个断点的信息，用链表来进行存储
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
//调试一个程序，该程序的当前信息
class DebugProject {
public:
	HANDLE hProcess;
	HANDLE hThread;
    BreakPoint* BreakPointList;
    BOOL isRunning;

    DebugProject()
    {
        hProcess = NULL;
        hThread = NULL;
        BreakPointList = NULL;
        isRunning = FALSE;
    }
};


BOOL GetContext(HANDLE hThread, DWORD& dwWant, const char* reg, BOOL isPrint);
void showData(BYTE* Data, int num, int type, DWORD Addr);