

#include "OpenCL.h"

int main() {

	OpenCL test("HelloWorld_Kernel.cl");
	if (!test.initialise()) {
		return -1;
	}

	int input = 1;
	int input2 = 2;
	std::cout << "Input:" << std::endl;
	std::cout << input << std::endl;
	std::cout << "Input2:" << std::endl;
	std::cout << input2 << std::endl;

	float* output = test.run(input, input2);
	std::cout << "Output:" << std::endl;
	for (int i = 1; i < 100; i++) {
		std::cout << output[i] << std::endl;
	}

	return SUCCESS;
}