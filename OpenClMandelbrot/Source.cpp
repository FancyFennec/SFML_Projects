

#include "OpenCL.h"

int main() {

	OpenCL test("HelloWorld_Kernel.cl");
	if (!test.initialise()) {
		return -1;
	}

	const char* input = "GdkknVnqkc";
	std::cout << "Input string:" << std::endl;
	std::cout << input << std::endl;

	char* output = test.run(input);
	std::cout << "Output string:" << std::endl;
	std::cout << output << std::endl;

	if (output != NULL)
	{
		free(output);
		output = NULL;
	}

	return SUCCESS;
}