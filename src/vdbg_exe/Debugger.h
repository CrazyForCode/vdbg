#pragma once
#include"global.h"

//ÿ���ϵ����Ϣ�������������д洢
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
//����һ�����򣬸ó���ĵ�ǰ��Ϣ
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