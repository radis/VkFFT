
#ifndef VKFFT_BACKEND_STRUCTS_H
#define VKFFT_BACKEND_STRUCTS_H

#include <hip/hiprtc.h>
#include <hip/hip_runtime.h>
#include <hip/hip_runtime_api.h>
#include <hip/hip_complex.h>

//typedef void* const backendVkFFTConstBuffer;
typedef void* backendVkFFTBuffer;
//typedef int backendVkFFTDevice;


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


#define VKFFT_BACKEND_LAUNCH_PARAMS 


#define VKFFT_BACKEND_SPEC_CONST_LAYOUT \
	pfINT  useStrict32BitAddress;


#define VKFFT_BACKEND_PUSH_CONST_LAYOUT


#define VKFFT_BACKEND_AXIS \
	hipModule_t VkFFTModule;\
	hipFunction_t VkFFTKernel;\
	hipDeviceptr_t consts_addr;


#define VKFFT_BACKEND_APPLICATION


#define VKFFT_BACKEND_DEVICE_PTR [0]
#define VKFFT_BACKEND_GET_BUFFER_RESOURCES_IDX(buf, idx) buf idx


#include "backend/backend_structs_generic.h"

#endif //VKFFT_BACKEND_STRUCTS_H