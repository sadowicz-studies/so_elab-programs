#define PIPE_NAME "\\\\.\\pipe\\TestPipe"
#define INPUT_SIZE 1048577540
#define BUFFER_SIZE 102400
#define THREADS 2

#include <stdio.h>
#include <Windows.h>

#include "..\\CommonFunctions\\common_functions.h"

DWORD WINAPI ServerThread(void* input);
DWORD WINAPI ClientThread(void* input);

HANDLE threadHandle[THREADS] = { NULL };
HANDLE InputFileHandle;

LARGE_INTEGER frequency;
LARGE_INTEGER start;
LARGE_INTEGER stop;

int main(int argc, char* argv[])
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);//implementation timer start

	if(argc != 2)
	{
		fprintf(stderr, "USAGE: program_name <input_file_path>\n");
		return EXIT_FAILURE;
	}

	InputFileHandle = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if(InputFileHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to create / open file.\n");
		printf("%d\n", GetLastError());
		return EXIT_FAILURE;
	}

	threadHandle[0] = CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
	if(!threadHandle)
	{
		fprintf(stderr, "ERROR: Unable to create thread\n");
		return EXIT_FAILURE;
	}

	threadHandle[1] = CreateThread(NULL, 0, ClientThread, NULL, 0, NULL);
	if(!threadHandle)
	{
		fprintf(stderr, "ERROR: Unable to create thread\n");
		return EXIT_FAILURE;
	}

	WaitForSingleObject(threadHandle[0], INFINITE);

	CloseHandle(InputFileHandle);
	CloseHandle(threadHandle[0]);

	QueryPerformanceCounter(&stop); //implementation timer stop

	double comTime = (double)(stop.QuadPart - start.QuadPart) / frequency.QuadPart * 1000;

	printf("Time:\t%.6f", comTime);

	return EXIT_SUCCESS;
}

DWORD WINAPI ServerThread(void* input)
{

	HANDLE pipeHandle = CreateNamedPipeA(PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, BUFFER_SIZE, BUFFER_SIZE, INFINITE, NULL);
	if(pipeHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to create named pipe.\n");
		return EXIT_FAILURE;
	}

	if(!ConnectNamedPipe(pipeHandle, NULL))
	{
		fprintf(stderr, "ERROR: Unable to connect to named pipe.\n");
		return EXIT_FAILURE;
	}

	char buffer[BUFFER_SIZE] = { 0 };

	//QueryPerformanceCounter(&start);//transfer timer start

	WriteFromFileToFile(InputFileHandle, pipeHandle, buffer, BUFFER_SIZE, INPUT_SIZE);

	WaitForSingleObject(threadHandle[1], INFINITE);

	//QueryPerformanceCounter(&stop); //transfer timer stop

	if(!DisconnectNamedPipe(pipeHandle))
	{
		fprintf(stderr, "ERROR: Unable to disconnect to named pipe.\n");
		return EXIT_FAILURE;
	}

	CloseHandle(pipeHandle);

	return EXIT_SUCCESS;
}

DWORD WINAPI ClientThread(void* input)
{
	WaitNamedPipeA(PIPE_NAME, NMPWAIT_USE_DEFAULT_WAIT);

	HANDLE pipeHandle = CreateFileA(PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(pipeHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to create / open file.\n");
		return EXIT_FAILURE;
	}

	char outBuffer[BUFFER_SIZE] = { 0 };

	if(!ReadFromFile(pipeHandle, outBuffer, BUFFER_SIZE, INPUT_SIZE))
		return EXIT_FAILURE;

	CloseHandle(pipeHandle);

	return EXIT_SUCCESS;
}