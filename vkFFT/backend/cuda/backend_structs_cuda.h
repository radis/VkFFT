
#ifndef VKFFT_BACKEND_STRUCTS_H
#define VKFFT_BACKEND_STRUCTS_H

#include <nvrtc.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <cuComplex.h>

//typedef void* const backendVkFFTConstBuffer;
typedef void* backendVkFFTBuffer;
typedef int backendVkFFTDevice;


#define VKFFT_BACKEND_FFT_CONFIGURATION \
	CUdevice* device; /*pointer to CUDA device, obtained from cuDeviceGet*/\
	/*CUcontext* context;/* /*pointer to CUDA context, obtained from cuDeviceGet*/\
	cudaStream_t* stream; /*pointer to streams (can be more than 1), where to execute the kernels*/\
	pfUINT num_streams; /*try to submit CUDA kernels in multiple streams for asynchronous execution. Default 0, set to >=1 if you pass values in the stream pointer.*/\
	\
	/*optional:*/\
	cudaEvent_t* stream_event; /*Filled at app creation*/\
	pfUINT streamCounter; /*Filled at app creation*/\
	pfUINT streamID /*Filled at app creation*/


#define VKFFT_BACKEND_LAUNCH_PARAMS 


#define VKFFT_BACKEND_SPEC_CONST_LAYOUT


#define VKFFT_BACKEND_PUSH_CONST_LAYOUT


#define VKFFT_BACKEND_AXIS \
	CUmodule VkFFTModule;\
	CUfunction VkFFTKernel;\
	CUdeviceptr consts_addr;


#define VKFFT_BACKEND_APPLICATION


#define VKFFT_BACKEND_DEVICE_PTR [0]
#define VKFFT_BACKEND_GET_BUFFER_RESOURCES_IDX(buf, idx) buf idx


#include "backend/backend_structs_generic.h"


#endif //VKFFT_BACKEND_STRUCTS_H