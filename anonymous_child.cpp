#define INPUT_SIZE 1048577540
#define BUFFER_SIZE 10240

#include <stdio.h>
#include <Windows.h>

#include "..\CommonFunctions\common_functions_a.h"

int main(int argc, char* argv[])
{
	HANDLE stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
	if(stdInHandle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR: Unable to get handle.\n");
		return EXIT_FAILURE;
	}

	char buffer[BUFFER_SIZE] = { 0 };

	if(!ReadFromFile(stdInHandle, buffer, BUFFER_SIZE, INPUT_SIZE))
		return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
