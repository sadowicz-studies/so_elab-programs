#define PIPE_NAME "\\\\.\\pipe\\TestPipe"
#define INPUT_SIZE 1048577540
#define BUFFER_SIZE 10240

#include <stdio.h>
#include <Windows.h>

#include "..\\CommonFunctions\\common_functions.h"

int main(int argc, char* argv[])
{
	WaitNamedPipeA(PIPE_NAME, NMPWAIT_USE_DEFAULT_WAIT);

	HANDLE pipeHandle = CreateFileA(PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(pipeHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to create / open file.\n");
		return EXIT_FAILURE;
	}

	char buffer[BUFFER_SIZE] = { 0 };

	if(!ReadFromFile(pipeHandle, buffer, BUFFER_SIZE, INPUT_SIZE))
		return EXIT_FAILURE;

	CloseHandle(pipeHandle);

   return EXIT_SUCCESS;
}