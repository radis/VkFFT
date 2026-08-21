
#ifndef VKFFT_BACKEND_STRUCTS_H
#define VKFFT_BACKEND_STRUCTS_H

#include <ze_api.h>

//typedef void* const backendVkFFTConstBuffer;
typedef void* backendVkFFTBuffer;
//typedef ze_device_handle_t backendVkFFTDevice;


#define VKFFT_BACKEND_FFT_CONFIGURATION \
	ze_device_handle_t* device;\
	ze_context_handle_t* context;\
	ze_command_queue_handle_t* commandQueue;\
	uint32_t commandQueueID;\
	\
	/*optional:*/\
	ze_command_list_handle_t* commandList /*Filled at app execution*/


#define VKFFT_BACKEND_LAUNCH_PARAMS \
	ze_command_list_handle_t* commandList //commandList to which FFT is appended


#define VKFFT_BACKEND_SPEC_CONST_LAYOUT


#define VKFFT_BACKEND_PUSH_CONST_LAYOUT


#define VKFFT_BACKEND_AXIS \
	ze_module_handle_t VkFFTModule;\
	ze_kernel_handle_t VkFFTKernel;\


#define VKFFT_BACKEND_APPLICATION


#define VKFFT_BACKEND_DEVICE_PTR app->configuration.context
#define VKFFT_BACKEND_GET_BUFFER_RESOURCES_IDX(buf, idx) buf idx


#include "backend/backend_structs_generic.h"

	
#endif //VKFFT_BACKEND_STRUCTS_H