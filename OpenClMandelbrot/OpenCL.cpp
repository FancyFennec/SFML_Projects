#include "OpenCL.h"



OpenCL::OpenCL(const char *filename) :
filename(filename)
{
}


OpenCL::~OpenCL()
{
	clReleaseKernel(kernel); //Release kernel.
	clReleaseProgram(program); //Release the program object.
	clReleaseCommandQueue(commandQueue); //Release  Command queue.
	clReleaseContext(context); //Release context.

	if (devices != NULL)
	{
		free(devices);
		devices = NULL;
	}

}

int OpenCL::openFile(const char * filename, std::string & s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if (f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size + 1];
		if (!str)
		{
			f.close();
			return SUCCESS;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return SUCCESS;
	}
	std::cout << "Error: failed to open file\n:" << filename << std::endl;
	return FAILURE;
}

int OpenCL::initialise()
{
	/*Step1: Getting platforms and choose an available one.*/
	if (clGetPlatformIDs(0, NULL, &numPlatforms) != CL_SUCCESS)
	{
		std::cout << "Error: Getting platforms!" << std::endl;
		return FAILURE;
	}

	if (numPlatforms > 0)
	{
		cl_platform_id* platforms =
			(cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
		clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}

	/*Step 2:Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device.*/
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	if (numDevices == 0) //no GPU available.
	{
		std::cout << "No GPU device available." << std::endl;
		std::cout << "Choose CPU as default device." << std::endl;
		clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
		clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
	}
	else
	{
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
		clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	}

	/*Step 3: Create context.*/
	context = clCreateContext(NULL, 1, devices, NULL, NULL, NULL);

	/*Step 4: Creating command queue associate with the context.*/
	commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);

	/*Step 5: Create program object */
	std::string sourceStr;
	openFile(filename, sourceStr);
	const char *source = sourceStr.c_str();
	size_t sourceSize[] = { strlen(source) };
	program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

	/*Step 6: Build program. */
	clBuildProgram(program, 1, devices, NULL, NULL, NULL);
}

char* OpenCL::run(const char* input)
{
	/*Step 7: Initial input,output for the host and create memory objects for the kernel*/
	size_t strlength = strlen(input);
	char* output = (char*)malloc(strlength + 1);

	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		(strlength + 1) * sizeof(char), (void *)input, NULL);
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		(strlength + 1) * sizeof(char), NULL, NULL);

	
	/*Step 8: Create kernel object */
	kernel = clCreateKernel(program, "helloworld", NULL);

	/*Step 9: Sets Kernel arguments.*/
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);

	/*Step 10: Running the kernel.*/
	size_t global_work_size[1] = { strlength };
	clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL,
		global_work_size, NULL, 0, NULL, NULL);

	/*Step 11: Read the cout put back to host memory.*/
	clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0,
		strlength * sizeof(char), output, 0, NULL, NULL);

	output[strlength] = '\0'; //Add the terminal character to the end of output.

	clReleaseMemObject(inputBuffer); //Release mem object.
	clReleaseMemObject(outputBuffer);

	return output;
}
