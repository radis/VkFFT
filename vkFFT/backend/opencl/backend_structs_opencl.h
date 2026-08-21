
#ifndef VKFFT_BACKEND_STRUCTS_H
#define VKFFT_BACKEND_STRUCTS_H

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

//typedef const cl_mem backendVkFFTConstBuffer;
typedef cl_mem backendVkFFTBuffer;
//typedef int backendVkFFTDevice;


#define VKFFT_BACKEND_FFT_CONFIGURATION \
	cl_platform_id* platform; /*not required*/ \
	cl_device_id* device;\
	cl_context* context;\
	\
	/*optional:*/\
	cl_command_queue* commandQueue


#define VKFFT_BACKEND_LAUNCH_PARAMS \
	cl_command_queue* commandQueue //commandBuffer to which FFT is appended
	
	
#define VKFFT_BACKEND_SPEC_CONST_LAYOUT


#define VKFFT_BACKEND_PUSH_CONST_LAYOUT


#define VKFFT_BACKEND_AXIS \
	cl_program  VkFFTProgram;\
	cl_kernel VkFFTKernel;\


#define VKFFT_BACKEND_APPLICATION


#define VKFFT_BACKEND_DEVICE_PTR [0]
#define VKFFT_BACKEND_GET_BUFFER_RESOURCES_IDX(buf, idx) buf idx


#include "backend/backend_structs_generic.h"

	
#endif //VKFFT_BACKEND_STRUCTS_H