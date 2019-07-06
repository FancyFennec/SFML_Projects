#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define SUCCESS 0
#define FAILURE 1

#include <CL/cl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

class OpenCL
{
public:
	OpenCL(const char *filename);
	~OpenCL();
	int initialise();
	float*  run(int i, int j);
	
private:
	const char *filename;
	int openFile(const char *filename, std::string& s);

	cl_uint numPlatforms;
	cl_platform_id platform = NULL;
	cl_uint numDevices = 0;
	cl_device_id* devices;

	cl_context context;
	cl_command_queue commandQueue;
	cl_program program;
	cl_kernel kernel;
};

