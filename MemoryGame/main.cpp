#include <windows.h>
#include <stdlib.h>
#include <utility>
#include <string>
#include "memory_app.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int showCommand)
{
    int N = 3;  // Default grid size
    if (cmdLine && wcslen(cmdLine) > 0) {
        N =  _wtoi(cmdLine);
    }
    memo_app app{ instance,N };
    return app.run(showCommand);
}