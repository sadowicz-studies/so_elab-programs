#include "common_functions.h"

//named pipe version

PROCESS_INFORMATION* CreateChildProc(LPCSTR programName, LPSTR commandLine, HANDLE childStdIn, HANDLE childStdOut)
{
	STARTUPINFOA sUpInfo;
	memset(&sUpInfo, 0, sizeof(STARTUPINFOA));

	sUpInfo.cb = sizeof(STARTUPINFOA);
	
	PROCESS_INFORMATION* resProcInfo = (PROCESS_INFORMATION*)calloc(1, sizeof(PROCESS_INFORMATION));
	if(!resProcInfo)
	{
		fprintf(stderr, "ERROR: Unable to allocate memory for PROCESS_INFORMATION structure. ( CreateChildProc )\n");
		return NULL;
	}
	memset(resProcInfo, 0, sizeof(PROCESS_INFORMATION));

	if(!CreateProcessA(programName, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &sUpInfo, resProcInfo))
	{
		fprintf(stderr, "ERROR: Unable to create process. ( CreateChildProc )\n");
		return NULL;
	}
	
	return resProcInfo;
}

BOOL ReadFromFile(HANDLE pipeReadHandle, char* buffer, size_t bufferSize, DWORD toRead)
{
	DWORD totalRead = 0;
	DWORD read;

	while(totalRead < toRead)
	{
		if(!ReadFile(pipeReadHandle, buffer, bufferSize, &read, NULL))
		{
			fprintf(stderr, "ERROR: Unable to read from pipe. ( ReadFromPipeOrFile )\n");
			return FALSE;
		}

		totalRead += read;
		ZeroMemory(buffer, bufferSize);
	}

	return TRUE;
}

BOOL WriteFromFileToFile(HANDLE pipeReadHandle, HANDLE pipeWriteHandle, char* buffer, size_t bufferSize, DWORD toWrite)
{
	DWORD read;
	DWORD totalWritten = 0;
	DWORD written;

	while(totalWritten < toWrite)
	{
		if(!ReadFile(pipeReadHandle, buffer, bufferSize, &read, NULL))
		{
			fprintf(stderr, "ERROR: Unable to read from pipe. ( WriteFromPipeToPipe )\n");
			return FALSE;
		}

		if(!WriteFile(pipeWriteHandle, buffer, read, &written, NULL))
		{
			fprintf(stderr, "ERROR: Unable to write to pipe. ( WriteFromPipeToPipe )\n");
			return FALSE;
		}

		totalWritten += written;
		ZeroMemory(buffer, bufferSize);
	}

	return TRUE;
}