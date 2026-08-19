
#ifndef VKFFT_BACKEND_H
#define VKFFT_BACKEND_H

#include <hip/hiprtc.h>
#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>
#include <hip/hip_complex.h>

#define VKFFT_BACKEND_FFT_CONFIGURATION \
	hipDevice_t* device; /*pointer to HIP device, obtained from hipDeviceGet*/\
	/*hipCtx_t* context;*/ /*pointer to HIP context, obtained from hipDeviceGet*/\
	hipStream_t* stream; /*pointer to streams (can be more than 1), where to execute the kernels*/\
	pfUINT num_streams; /*try to submit HIP kernels in multiple streams for asynchronous execution. Default 0, set to >=1 if you pass values in the stream pointer.*/\
	\
	/*optional:*/\
	hipEvent_t* stream_event; /*Filled at app creation*/\
	pfUINT streamCounter; /*Filled at app creation*/\
	pfUINT streamID; /*Filled at app creation*/\
	pfINT  useStrict32BitAddress /* guarantee 32 bit addresses in bytes instead of number of elements. This results in fewer instructions generated. -1: Disable, 0: Infer based on size, 1: enable. Has no effect with useUint64.*/


typedef void* const backendVkFFTConstBuffer;
typedef void* backendVkFFTBuffer;

#define VKFFT_BACKEND_LAUNCH_PARAMS 

#define VKFFT_BACKEND_SPEC_CONST_LAYOUT \
	pfINT  useStrict32BitAddress;

#define VKFFT_BACKEND_PUSH_CONST_LAYOUT


#define VKFFT_BACKEND_AXIS \
	void* const* inputBuffer;\
	void* const* outputBuffer;\
	void* const* kernel;\
	hipModule_t VkFFTModule;\
	hipFunction_t VkFFTKernel;\
	void* bufferLUT;\
	void* bufferRaderUintLUT;\
	hipDeviceptr_t consts_addr;\
	void** bufferBluestein;\
	void** bufferBluesteinFFT

#define VKFFT_BACKEND_APPLICATION \
	void* bufferRaderUintLUT[VKFFT_MAX_FFT_DIMENSIONS][4];\
	void* bufferBluestein[VKFFT_MAX_FFT_DIMENSIONS];\
	void* bufferBluesteinFFT[VKFFT_MAX_FFT_DIMENSIONS];\
	void* bufferBluesteinIFFT[VKFFT_MAX_FFT_DIMENSIONS]	






#endif //VKFFT_BACKEND_H