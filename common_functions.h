#pragma once

//named pipe version

#include <stdio.h>
#include <Windows.h>

PROCESS_INFORMATION* CreateChildProc(LPCSTR programName, LPSTR commandLine, HANDLE childStdIn, HANDLE childStdOut);

BOOL ReadFromFile(HANDLE pipeReadHandle, char* buffer, size_t bufferSize, DWORD toRead);
BOOL WriteFromFileToFile(HANDLE pipeReadHandle, HANDLE pipeWriteHandle, char* buffer, size_t bufferSize, DWORD toWrite);
