#include "common_functions.h"

// anonymous pipes version

PROCESS_INFORMATION* CreateChildProc(LPCSTR programName, LPSTR commandLine, HANDLE childStdIn, HANDLE childStdOut)
{
	STARTUPINFOA sUpInfo;
	memset(&sUpInfo, 0, sizeof(STARTUPINFOA));

	sUpInfo.cb = sizeof(STARTUPINFOA);

	if(!childStdIn)
		sUpInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	else
		sUpInfo.hStdInput = childStdIn;

	if(!childStdOut)
		sUpInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	else
		sUpInfo.hStdOutput = childStdOut;

	sUpInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	sUpInfo.dwFlags = STARTF_USESTDHANDLES;

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

		CloseHandle(childStdIn);
		CloseHandle(childStdOut);

	return resProcInfo;
}

BOOL CreateOneWayPipe(PHANDLE readPipeHandle, PHANDLE writePipeHandle, BOOL childWrites)
{
	SECURITY_ATTRIBUTES secAttributes = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

	if(!CreatePipe(readPipeHandle, writePipeHandle, &secAttributes, 0))
	{
		fprintf(stderr, "ERROR: Unable to create anonymus pipe. ( CreateOneWayPipe )\n");
		return FALSE;
	}

	if(childWrites)
	{
		if(!SetHandleInformation(*readPipeHandle, HANDLE_FLAG_INHERIT, 0))
		{
			fprintf(stderr, "ERROR: Unable to set handle information. ( CreateOneWayPipe )\n");
			return FALSE;
		}
	}
	else
	{
		if(!SetHandleInformation(*writePipeHandle, HANDLE_FLAG_INHERIT, 0))
		{
			fprintf(stderr, "ERROR: Unable to set handle information. ( CreateOneWayPipe )\n");
			return FALSE;
		}
	}

	return TRUE;
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