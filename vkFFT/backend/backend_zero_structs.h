
#ifndef VKFFT_BACKEND_STRUCTS_H
#define VKFFT_BACKEND_STRUCTS_H

#include <ze_api.h>

#define VKFFT_BACKEND_FFT_CONFIGURATION \
	ze_device_handle_t* device;\
	ze_context_handle_t* context;\
	ze_command_queue_handle_t* commandQueue;\
	uint32_t commandQueueID;\
	\
	/*optional:*/\
	ze_command_list_handle_t* commandList /*Filled at app execution*/


typedef void* const backendVkFFTConstBuffer;
typedef void* backendVkFFTBuffer;

#define VKFFT_BACKEND_LAUNCH_PARAMS \
	ze_command_list_handle_t* commandList //commandList to which FFT is appended

#define VKFFT_BACKEND_SPEC_CONST_LAYOUT

#define VKFFT_BACKEND_PUSH_CONST_LAYOUT

#define VKFFT_BACKEND_AXIS \
	void* const* inputBuffer;\
	void* const* outputBuffer;\
	void* const* kernel;\
	ze_module_handle_t VkFFTModule;\
	ze_kernel_handle_t VkFFTKernel;\
	void* bufferLUT;\
	void* bufferRaderUintLUT;\
	void** bufferBluestein;\
	void** bufferBluesteinFFT

#define VKFFT_BACKEND_APPLICATION \
	void* bufferRaderUintLUT[VKFFT_MAX_FFT_DIMENSIONS][4];\
	void* bufferBluestein[VKFFT_MAX_FFT_DIMENSIONS];\
	void* bufferBluesteinFFT[VKFFT_MAX_FFT_DIMENSIONS];\
	void* bufferBluesteinIFFT[VKFFT_MAX_FFT_DIMENSIONS]







#endif //VKFFT_BACKEND_STRUCTS_H