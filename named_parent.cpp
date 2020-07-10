#define PIPE_NAME "\\\\.\\pipe\\TestPipe"
#define INPUT_SIZE 1048577540
#define BUFFER_SIZE 10240

#include <stdio.h>
#include <Windows.h>

#include "..\\CommonFunctions\\common_functions.h"

LARGE_INTEGER frequency;
LARGE_INTEGER start;
LARGE_INTEGER stop;

int main(int argc, char* argv[])
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start); // implementation timer start

	if(argc != 3)
	{
		fprintf(stderr, "USAGE: program_name <input_file_path> <client_process_file_path>\n");
		return EXIT_FAILURE;
	}

	HANDLE pipeHandle = CreateNamedPipeA(PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, BUFFER_SIZE, BUFFER_SIZE, INFINITE, NULL);
	if(pipeHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to create named pipe.\n");
		return EXIT_FAILURE;
	}

	PROCESS_INFORMATION* childProcInfo = CreateChildProc(argv[2], NULL, NULL, NULL);
	if(!childProcInfo)
		return EXIT_FAILURE;


	if(!ConnectNamedPipe(pipeHandle, NULL))
	{
		fprintf(stderr, "ERROR: Unable to connect to named pipe.\n");
		return EXIT_FAILURE;
	}

	HANDLE InputFileHandle = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if(InputFileHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to create / open file.\n");
		printf("%d\n", GetLastError());
		return EXIT_FAILURE;
	}

	char buffer[BUFFER_SIZE] = { 0 };

	//QueryPerformanceFrequency(&frequency);
	//QueryPerformanceCounter(&start); // transfer timer start

	if(!WriteFromFileToFile(InputFileHandle, pipeHandle, buffer, BUFFER_SIZE, INPUT_SIZE))
		return EXIT_FAILURE;

	WaitForSingleObject(childProcInfo->hProcess, INFINITE);

	//QueryPerformanceCounter(&stop); // transfer timer stop

	if(!DisconnectNamedPipe(pipeHandle))
	{
		fprintf(stderr, "ERROR: Unable to disconnect to named pipe.\n");
		return EXIT_FAILURE;
	}

	CloseHandle(pipeHandle);
	CloseHandle(InputFileHandle);

	CloseHandle(childProcInfo->hThread);
	CloseHandle(childProcInfo->hProcess);

	free(childProcInfo);

	QueryPerformanceCounter(&stop); // implementation timer stop

	double comTime = (double)(stop.QuadPart - start.QuadPart) / frequency.QuadPart * 1000;

	printf("Time:\t%.6f", comTime);

    return EXIT_SUCCESS;
}