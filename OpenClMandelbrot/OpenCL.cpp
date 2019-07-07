#include "OpenCL.h"

OpenCL::OpenCL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char *filename) :
filename(filename),
SCREEN_WIDTH(SCREEN_WIDTH),
SCREEN_HEIGHT(SCREEN_HEIGHT)
{
	output = new int[SCREEN_WIDTH * SCREEN_HEIGHT];
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
	/*Getting platforms and choose an available one.*/
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

	/*Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device.*/
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

	/*Create context.*/
	context = clCreateContext(NULL, 1, devices, NULL, NULL, NULL);

	/*Step 4: Creating command queue associate with the context.*/
	commandQueue = clCreateCommandQueueWithProperties(context, devices[0], 0, NULL);

	/*Create program object */
	std::string sourceStr;
	openFile(filename, sourceStr);
	const char *source = sourceStr.c_str();
	size_t sourceSize[] = { strlen(source) };
	program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

	/*Build program. */
	clBuildProgram(program, 1, devices, NULL, NULL, NULL);

	/*Create kernel object */
	kernel = clCreateKernel(program, "helloworld", NULL);
}

int* OpenCL::run(int nZoom, double x, double y)
{
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(int), NULL, NULL);

	/*Set Kernel arguments.*/
	clSetKernelArg(kernel, 0, sizeof(int), &SCREEN_WIDTH);
	clSetKernelArg(kernel, 1, sizeof(int), &SCREEN_HEIGHT);
	clSetKernelArg(kernel, 2, sizeof(int), &nZoom);
	clSetKernelArg(kernel, 3, sizeof(double), &x);
	clSetKernelArg(kernel, 4, sizeof(double), &y);
	clSetKernelArg(kernel, 5, sizeof(cl_mem), &outputBuffer);

	/*Running the kernel.*/
	size_t global_work_size[1] = { SCREEN_WIDTH * SCREEN_HEIGHT };
	clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

	/*Read the cout put back to host memory.*/
	clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(int), output, 0, NULL, NULL);

	clReleaseMemObject(outputBuffer);//Release buffer.

	return output;
}
