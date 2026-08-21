
#ifndef VKFFT_BACKEND_STRUCTS_H
#define VKFFT_BACKEND_STRUCTS_H

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "Foundation/Foundation.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "Metal/Metal.hpp"

//typedef MTL::Buffer* const backendVkFFTConstBuffer;
typedef MTL::Buffer* backendVkFFTBuffer;
//typedef int backendVkFFTResult; //Not used on Metal
//#define VKFFT_BACKEND_SUCCESS 0 //Not used on Metal
//typedef int backendVkFFTDevice;


#define VKFFT_BACKEND_FFT_CONFIGURATION \
	MTL::Device* device;\
	MTL::CommandQueue* queue;\
	\
	/*optional:*/\
	MTL::CommandBuffer* commandBuffer;/*Filled at app execution*/\
	MTL::ComputeCommandEncoder* commandEncoder /*Filled at app execution*/


#define VKFFT_BACKEND_LAUNCH_PARAMS \
	MTL::CommandBuffer* commandBuffer;/*commandBuffer to which FFT is appended*/\
	MTL::ComputeCommandEncoder* commandEncoder //encoder associated with commandBuffer


#define VKFFT_BACKEND_SPEC_CONST_LAYOUT


#define VKFFT_BACKEND_PUSH_CONST_LAYOUT \
	MTL::Buffer* dataUintBuffer;


#define VKFFT_BACKEND_AXIS \
	MTL::Library* library;\
	MTL::ComputePipelineState* pipeline;\


#define VKFFT_BACKEND_APPLICATION


#define VKFFT_BACKEND_DEVICE_PTR [0]
#define VKFFT_BACKEND_GET_BUFFER_RESOURCES_IDX(buf, idx) buf idx


#include "backend/backend_structs_generic.h"

	
#endif //VKFFT_BACKEND_STRUCTS_H