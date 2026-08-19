
#ifndef VKFFT_BACKEND_H
#define VKFFT_BACKEND_H

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define VKFFT_BACKEND_FFT_CONFIGURATION \
	cl_platform_id* platform; /*not required*/ \
	cl_device_id* device;\
	cl_context* context;\
	\
	/*optional:*/\
	cl_command_queue* commandQueue


typedef const cl_mem backendVkFFTConstBuffer;
typedef cl_mem backendVkFFTBuffer;

#define VKFFT_BACKEND_LAUNCH_PARAMS \
	cl_command_queue* commandQueue //commandBuffer to which FFT is appended
	
#define VKFFT_BACKEND_SPEC_CONST_LAYOUT

#define VKFFT_BACKEND_PUSH_CONST_LAYOUT

#define VKFFT_BACKEND_AXIS \
	const cl_mem* inputBuffer;\
	const cl_mem* outputBuffer;\
	const cl_mem* kernel;\
	cl_program  VkFFTProgram;\
	cl_kernel VkFFTKernel;\
	cl_mem bufferLUT;\
	cl_mem bufferRaderUintLUT;\
	cl_mem* bufferBluestein;\
	cl_mem* bufferBluesteinFFT

#define VKFFT_BACKEND_APPLICATION \
	cl_mem bufferRaderUintLUT[VKFFT_MAX_FFT_DIMENSIONS][4];\
	cl_mem bufferBluestein[VKFFT_MAX_FFT_DIMENSIONS];\
	cl_mem bufferBluesteinFFT[VKFFT_MAX_FFT_DIMENSIONS];\
	cl_mem bufferBluesteinIFFT[VKFFT_MAX_FFT_DIMENSIONS]	






#endif //VKFFT_BACKEND_H