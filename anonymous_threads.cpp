#define INPUT_SIZE 1048577540
#define BUFFER_SIZE 102400
#define THREADS 2

#include <stdio.h>
#include <Windows.h>
#include <time.h>

#include "..\CommonFunctions\\common_functions.h"

DWORD WINAPI WritePipeThread(void* input);
DWORD WINAPI ReadPipeThread(void* input);

HANDLE pipeReadHandle;
HANDLE pipeWriteHandle;

HANDLE InputFileHandle;

LARGE_INTEGER frequency;
LARGE_INTEGER start;
LARGE_INTEGER stop;

int main(int argc, char* argv[])
{
	QueryPerformanceFrequency(&frequency); // timer frequency
	QueryPerformanceCounter(&start); //implementation timer start

	if(argc != 2)
	{
		printf("USAGE: program_name <input_file_path>\n");
		return EXIT_FAILURE;
	}

	if(!CreateOneWayPipe(&pipeReadHandle, &pipeWriteHandle, FALSE))
		return EXIT_FAILURE;

	InputFileHandle = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if(InputFileHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to create / open file.\n");
		return EXIT_FAILURE;
	}

	HANDLE threadHandles[THREADS];

	threadHandles[0] = CreateThread(NULL, 0, WritePipeThread, NULL, 0, NULL);
	if(!threadHandles[0])
	{
		fprintf(stderr, "ERROR: Unable to create thread.\n");
		return EXIT_FAILURE;
	}

	threadHandles[1] = CreateThread(NULL, 0, ReadPipeThread, NULL, 0, NULL);
	if(!threadHandles[1])
	{
		fprintf(stderr, "ERROR: Unable to create thread.\n");
		return EXIT_FAILURE;
	}

	WaitForMultipleObjects(THREADS, threadHandles, TRUE, INFINITE);

	//QueryPerformanceCounter(&stop); //transfer timer stop
	
	CloseHandle(threadHandles[0]);
	CloseHandle(threadHandles[1]);
	CloseHandle(InputFileHandle);
	CloseHandle(pipeReadHandle);
	CloseHandle(pipeWriteHandle);

	QueryPerformanceCounter(&stop); //implementation timer stop

	double comTime = (double)(stop.QuadPart - start.QuadPart) / frequency.QuadPart * 1000;

	printf("Time:\t%.6f", comTime);

	return EXIT_SUCCESS;
}

DWORD WINAPI WritePipeThread(void* input)
{
	char buffer[BUFFER_SIZE] = { 0 };

	//QueryPerformanceCounter(&start); //transfer timer start

	if(!WriteFromFileToFile(InputFileHandle, pipeWriteHandle, buffer, BUFFER_SIZE, INPUT_SIZE))
			return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

DWORD WINAPI ReadPipeThread(void* input)
{
	char outBuffer[BUFFER_SIZE] = { 0 };

	if(!ReadFromFile(pipeReadHandle, outBuffer, BUFFER_SIZE, INPUT_SIZE))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}