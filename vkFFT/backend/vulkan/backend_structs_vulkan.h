
#ifndef VKFFT_BACKEND_STRUCTS_H
#define VKFFT_BACKEND_STRUCTS_H

#include "vulkan/vulkan.h"
#include "glslang/Include/glslang_c_interface.h"

#define VKFFT_BACKEND_FFT_CONFIGURATION \
	VkPhysicalDevice* physicalDevice; /*pointer to Vulkan physical device, obtained from vkEnumeratePhysicalDevices*/\
	VkDevice* device; /*pointer to Vulkan device, created with vkCreateDevice*/\
	VkQueue* queue; /*pointer to Vulkan queue, created with vkGetDeviceQueue*/\
	VkCommandPool* commandPool; /*pointer to Vulkan command pool, created with vkCreateCommandPool*/\
	VkFence* fence; /*pointer to Vulkan fence, created with vkCreateFence*/\
	pfUINT isCompilerInitialized; /*specify if glslang compiler has been intialized before (0 - off, 1 - on). Default 0*/\
	pfUINT usePushDescriptors; /*specify if push descriptors have been enabled on the device - then all descriptor sets updates at VkFFTAppend call will be issued with push descriptors. (0 - off, 1 - on). Default 0*/\
	\
	/*optional:*/\
	VkPipelineCache* pipelineCache; /*pointer to Vulkan pipeline cache*/\
	VkBuffer* stagingBuffer; /*pointer to the user defined staging buffer (used internally for LUT data transfers)*/\
	VkDeviceMemory* stagingBufferMemory; /*pointer to the user defined staging buffer memory, associated with the stagingBuffer (used internally for LUT data transfers)*/\
	\
	VkDeviceMemory tempBufferDeviceMemory; /*Filled at app creation*/\
	VkCommandBuffer* commandBuffer; /*Filled at app execution*/\
	VkMemoryBarrier* memory_barrier; /*Filled at app creation*/

typedef const VkBuffer backendVkFFTConstBuffer;
typedef VkBuffer backendVkFFTBuffer;

typedef VkResult backendVkFFTResult;
#define VKFFT_BACKEND_SUCCESS VK_SUCCESS

#define VKFFT_BACKEND_LAUNCH_PARAMS \
	VkCommandBuffer* commandBuffer; //commandBuffer to which FFT is appended

#define VKFFT_BACKEND_SPEC_CONST_LAYOUT

#define VKFFT_BACKEND_PUSH_CONST_LAYOUT

#define VKFFT_BACKEND_AXIS \
	const VkBuffer* inputBuffer;\
	const VkBuffer* outputBuffer;\
	const VkBuffer* kernel;\
	VkDescriptorPool descriptorPool;\
	VkDescriptorSetLayout descriptorSetLayout;\
	VkDescriptorSet descriptorSet;\
	VkPipelineLayout pipelineLayout;\
	VkPipeline pipeline;\
	VkDeviceMemory bufferLUTDeviceMemory;\
	VkBuffer bufferLUT;\
	VkDeviceMemory bufferRaderUintLUTDeviceMemory;\
	VkBuffer bufferRaderUintLUT;\
	VkDeviceMemory* bufferBluesteinDeviceMemory;\
	VkDeviceMemory* bufferBluesteinFFTDeviceMemory;\
	VkBuffer* bufferBluestein;\
	VkBuffer* bufferBluesteinFFT;


#define VKFFT_BACKEND_APPLICATION \
	VkDeviceMemory bufferRaderUintLUTDeviceMemory[VKFFT_MAX_FFT_DIMENSIONS][4];\
	VkBuffer bufferRaderUintLUT[VKFFT_MAX_FFT_DIMENSIONS][4];\
	VkDeviceMemory bufferBluesteinDeviceMemory[VKFFT_MAX_FFT_DIMENSIONS];\
	VkDeviceMemory bufferBluesteinFFTDeviceMemory[VKFFT_MAX_FFT_DIMENSIONS];\
	VkDeviceMemory bufferBluesteinIFFTDeviceMemory[VKFFT_MAX_FFT_DIMENSIONS];\
	VkBuffer bufferBluestein[VKFFT_MAX_FFT_DIMENSIONS];\
	VkBuffer bufferBluesteinFFT[VKFFT_MAX_FFT_DIMENSIONS];\
	VkBuffer bufferBluesteinIFFT[VKFFT_MAX_FFT_DIMENSIONS];

#endif //VKFFT_BACKEND_STRUCTS_H
