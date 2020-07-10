#define INPUT_SIZE 1048577540
#define BUFFER_SIZE 10240

#include <stdio.h>
#include <Windows.h>
#include <time.h>

#include "..\CommonFunctions\common_functions.h"

HANDLE childInHandle = NULL;
HANDLE parentHandle = NULL;

LARGE_INTEGER frequency;
LARGE_INTEGER start;
LARGE_INTEGER stop;

int main(int argc, char* argv[])
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start); // implementation timer start

	if(argc != 3)
	{
		printf("USAGE: program_name <input_file_path> <child_process_file_path>\n");
		return EXIT_FAILURE;
	}

	if(!CreateOneWayPipe(&childInHandle, &parentHandle, FALSE))
		return EXIT_FAILURE;

	PROCESS_INFORMATION* childProcInfo = CreateChildProc(argv[2], NULL, childInHandle, NULL);
	if(!childProcInfo)
		return EXIT_FAILURE;

	HANDLE InputFileHandle = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if(InputFileHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to create / open file.\n");
		return EXIT_FAILURE;
	}

	char buffer[BUFFER_SIZE] = { 0 };

	//QueryPerformanceFrequency(&frequency);
	//QueryPerformanceCounter(&start); //transfer timer start


	if(!WriteFromFileToFile(InputFileHandle, parentHandle, buffer, BUFFER_SIZE, INPUT_SIZE))
		return EXIT_FAILURE;

	WaitForSingleObject(childProcInfo->hProcess, INFINITE);
	
	//QueryPerformanceCounter(&stop); //transfer timer stop

	CloseHandle(childInHandle);
	CloseHandle(parentHandle);
	CloseHandle(childProcInfo->hThread);
	CloseHandle(childProcInfo->hProcess);

	free(childProcInfo);

	QueryPerformanceCounter(&stop); //implementation timer stop
	double comTime = (double)(stop.QuadPart - start.QuadPart) / frequency.QuadPart * 1000;

	printf("Time:\t%.6f", comTime);

	return EXIT_SUCCESS;
}