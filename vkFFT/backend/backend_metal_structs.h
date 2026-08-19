
#ifndef VKFFT_BACKEND_H
#define VKFFT_BACKEND_H

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "Foundation/Foundation.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "Metal/Metal.hpp"

#define VKFFT_BACKEND_FFT_CONFIGURATION \
	MTL::Device* device;\
	MTL::CommandQueue* queue;\
	\
	/*optional:*/\
	MTL::CommandBuffer* commandBuffer;/*Filled at app execution*/\
	MTL::ComputeCommandEncoder* commandEncoder /*Filled at app execution*/

typedef MTL::Buffer* const backendVkFFTConstBuffer;
typedef MTL::Buffer* backendVkFFTBuffer;

#define VKFFT_BACKEND_LAUNCH_PARAMS \
	MTL::CommandBuffer* commandBuffer;/*commandBuffer to which FFT is appended*/\
	MTL::ComputeCommandEncoder* commandEncoder //encoder associated with commandBuffer

#define VKFFT_BACKEND_SPEC_CONST_LAYOUT

#define VKFFT_BACKEND_PUSH_CONST_LAYOUT \
	MTL::Buffer* dataUintBuffer;


#define VKFFT_BACKEND_AXIS \
	MTL::Buffer* const* inputBuffer;\
	MTL::Buffer* const* outputBuffer;\
	MTL::Buffer* const* kernel;\
	MTL::Library* library;\
	MTL::ComputePipelineState* pipeline;\
	MTL::Buffer* bufferLUT;\
	MTL::Buffer* bufferRaderUintLUT;\
	MTL::Buffer** bufferBluestein;\
	MTL::Buffer** bufferBluesteinFFT


#define VKFFT_BACKEND_APPLICATION \
	MTL::Buffer* bufferRaderUintLUT[VKFFT_MAX_FFT_DIMENSIONS][4];\
	MTL::Buffer* bufferBluestein[VKFFT_MAX_FFT_DIMENSIONS];\
	MTL::Buffer* bufferBluesteinFFT[VKFFT_MAX_FFT_DIMENSIONS];\
	MTL::Buffer* bufferBluesteinIFFT[VKFFT_MAX_FFT_DIMENSIONS]






#endif //VKFFT_BACKEND_H