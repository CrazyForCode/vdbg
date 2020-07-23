#pragma once
#include "global.h"
#include"Debugger.h"
void DivideArg(char* command, char(*argList)[20]);
BOOL AsciiToWchar(WCHAR *Dest,char *Src);
void TerminalDebugger(DebugProject* NowProject);