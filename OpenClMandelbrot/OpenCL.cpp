#include "OpenCL.h"

OpenCL::OpenCL(int SCREEN_WIDTH, const char *filename) :
filename(filename),
SCREEN_WIDTH(SCREEN_WIDTH)
{
	output = new double[SCREEN_WIDTH * SCREEN_WIDTH];
}

OpenCL::~OpenCL()
{
	clReleaseKernel(kernel); //Release kernel.
	clReleaseProgram(program); //Release the program object.
	clReleaseContext(context); //Release context.
	clReleaseCommandQueue(commandQueue); //Release  Command queue.

	if (output != NULL)
	{
		free(output);
		output = NULL;
	}

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

	/*Step 8: Create kernel object */
	kernel = clCreateKernel(program, "helloworld", NULL);
}

double* OpenCL::run(int nZoom, double x, double y)
{
	//double* output = (double*) malloc(SCREEN_WIDTH * SCREEN_WIDTH);

	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, SCREEN_WIDTH * SCREEN_WIDTH * sizeof(double), NULL, NULL);

	/*Step 9: Sets Kernel arguments.*/
	clSetKernelArg(kernel, 0, sizeof(int), &SCREEN_WIDTH);
	clSetKernelArg(kernel, 1, sizeof(int), &nZoom);
	clSetKernelArg(kernel, 2, sizeof(double), &x);
	clSetKernelArg(kernel, 3, sizeof(double), &y);
	clSetKernelArg(kernel, 4, sizeof(cl_mem), &outputBuffer);

	/*Step 10: Running the kernel.*/
	size_t global_work_size[1] = { SCREEN_WIDTH * SCREEN_WIDTH };
	clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

	/*Step 11: Read the cout put back to host memory.*/
	clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, SCREEN_WIDTH * SCREEN_WIDTH * sizeof(double), output, 0, NULL, NULL);

	clReleaseMemObject(outputBuffer);

	return output;
}
