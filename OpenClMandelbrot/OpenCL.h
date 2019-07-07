#pragma once

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
	OpenCL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char *filename);
	~OpenCL();
	int initialise();
	int* run(int nZoom, double x, double y);
	
private:
	const char *filename;
	int openFile(const char *filename, std::string& s);

	const int SCREEN_WIDTH;
	const int SCREEN_HEIGHT;
	int* output;

	cl_uint numPlatforms;
	cl_platform_id platform = NULL;
	cl_uint numDevices = 0;
	cl_device_id* devices;

	cl_context context;
	cl_command_queue commandQueue;
	cl_program program;
	cl_kernel kernel;
};

