
#ifndef VKFFT_BACKEND_STRUCTS_H
#define VKFFT_BACKEND_STRUCTS_H

#include <nvrtc.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <cuComplex.h>

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

typedef void* const backendVkFFTConstBuffer;
typedef void* backendVkFFTBuffer;

#define VKFFT_BACKEND_LAUNCH_PARAMS 

#define VKFFT_BACKEND_SPEC_CONST_LAYOUT

#define VKFFT_BACKEND_PUSH_CONST_LAYOUT

#define VKFFT_BACKEND_AXIS \
	void* const* inputBuffer;\
	void* const* outputBuffer;\
	void* const* kernel;\
	CUmodule VkFFTModule;\
	CUfunction VkFFTKernel;\
	void* bufferLUT;\
	void* bufferRaderUintLUT;\
	CUdeviceptr consts_addr;\
	void** bufferBluestein;\
	void** bufferBluesteinFFT

#define VKFFT_BACKEND_APPLICATION \
	void* bufferRaderUintLUT[VKFFT_MAX_FFT_DIMENSIONS][4];\
	void* bufferBluestein[VKFFT_MAX_FFT_DIMENSIONS];\
	void* bufferBluesteinFFT[VKFFT_MAX_FFT_DIMENSIONS];\
	void* bufferBluesteinIFFT[VKFFT_MAX_FFT_DIMENSIONS]






#endif //VKFFT_BACKEND_STRUCTS_H