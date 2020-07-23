#pragma once
#include"global.h"
#include"Debugger.h"
#include"capstone.h"
#include"common.h"
//常规函数

BreakPoint* AddBreakPoint(HANDLE hProcess, LPVOID Addr, int type);
void DisamMem(DWORD Addr, HANDLE hProcess);
void console(DebugProject* NowProject);

//异常触发的动作

void OnProcessCreated(const CREATE_PROCESS_DEBUG_INFO* pInfo, HANDLE hProcess);
void OnThreadCreated(const CREATE_THREAD_DEBUG_INFO* pInfo, HANDLE hThread);
void OnException(const EXCEPTION_DEBUG_INFO* pInfo, DWORD& DebugSign, DebugProject* NowProject);
void OnProcessExited(const EXIT_PROCESS_DEBUG_INFO* pInfo);
void OnThreadExited(const EXIT_THREAD_DEBUG_INFO* pInfo);
void OnOutputDebugString(const OUTPUT_DEBUG_STRING_INFO* pInfo, HANDLE hProcess, HANDLE hThread);
void OnRipEvent(const RIP_INFO* pInfo);
void OnDllLoaded(const LOAD_DLL_DEBUG_INFO* pInfo);
void OnDllUnloaded(const UNLOAD_DLL_DEBUG_INFO* pInfo);