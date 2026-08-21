
#ifndef VKFFT_BACKEND_APP_MANAGEMENT_H
#define VKFFT_BACKEND_APP_MANAGEMENT_H

#include "backend/backend_structs.h"
#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"

////////////////////////////////////////////
//vkFFT/vkFFT_AppManagement/vkFFT_RunApp.h//
////////////////////////////////////////////

static inline VkFFTResult VkFFTSync(VkFFTApplication* app) {
    vkCmdPipelineBarrier(app->configuration.commandBuffer[0], VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 1, app->configuration.memory_barrier, 0, 0, 0, 0);
	return VKFFT_SUCCESS;
};

static inline void VkFFTAppend_backendSetCommandBuffer(VkFFTApplication* app, VkFFTLaunchParams* launchParams){
    app->configuration.commandBuffer = launchParams->commandBuffer;
    VkMemoryBarrier memory_barrier = {
            VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            0,
            VK_ACCESS_SHADER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
    };
    app->configuration.memory_barrier = &memory_barrier;
};


static inline void VkFFTAppend_backendBindPipelineAndDescriptorSets(VkFFTApplication* app, VkFFTAxis* axis) {
	vkCmdBindPipeline(app->configuration.commandBuffer[0], VK_PIPELINE_BIND_POINT_COMPUTE, axis->pipeline);
	if (!app->configuration.usePushDescriptors) vkCmdBindDescriptorSets(app->configuration.commandBuffer[0], VK_PIPELINE_BIND_POINT_COMPUTE, axis->pipelineLayout, 0, 1, &axis->descriptorSet, 0, 0);
};

///////////////////////////////////////////////////
//vkFFT/vkFFT_AppManagement/vkFFT_InitializeApp.h//
///////////////////////////////////////////////////

static inline VkFFTResult setConfigurationVkFFT_backendSetDeviceParams(VkFFTApplication* app, VkFFTConfiguration inputLaunchConfiguration){
	if (!inputLaunchConfiguration.isCompilerInitialized) {
		if (!app->configuration.isCompilerInitialized) {
			int resGlslangInitialize = glslang_initialize_process();
			if (!resGlslangInitialize) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
			app->configuration.isCompilerInitialized = 1;
		}
	}
	if (inputLaunchConfiguration.physicalDevice == 0) {
		
		return VKFFT_ERROR_INVALID_PHYSICAL_DEVICE;
	}
	app->configuration.physicalDevice = (VkPhysicalDevice*)calloc(1,sizeof(VkPhysicalDevice));
	if (!app->configuration.physicalDevice) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.physicalDevice[0] = inputLaunchConfiguration.physicalDevice[0];
	}

	if (inputLaunchConfiguration.device == 0) {
		
		return VKFFT_ERROR_INVALID_DEVICE;
	}
	app->configuration.device = (VkDevice*)calloc(1, sizeof(VkDevice));
	if (!app->configuration.device) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.device[0] = inputLaunchConfiguration.device[0];
	}

	if (inputLaunchConfiguration.queue == 0) {
		
		return VKFFT_ERROR_INVALID_QUEUE;
	}
	app->configuration.queue = (VkQueue*)calloc(1, sizeof(VkQueue));
	if (!app->configuration.queue) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.queue[0] = inputLaunchConfiguration.queue[0];
	}

	if (inputLaunchConfiguration.commandPool == 0) {
		
		return VKFFT_ERROR_INVALID_COMMAND_POOL;
	}
	app->configuration.commandPool = (VkCommandPool*)calloc(1, sizeof(VkCommandPool));
	if (!app->configuration.commandPool) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.commandPool[0] = inputLaunchConfiguration.commandPool[0];
	}

	if (inputLaunchConfiguration.fence == 0) {
		
		return VKFFT_ERROR_INVALID_FENCE;
	}
	app->configuration.fence = (VkFence*)calloc(1, sizeof(VkFence));
	if (!app->configuration.fence) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.fence[0] = inputLaunchConfiguration.fence[0];
	}

	app->configuration.usePushDescriptors = inputLaunchConfiguration.usePushDescriptors;

	VkPhysicalDeviceProperties physicalDeviceProperties = { 0 };
	vkGetPhysicalDeviceProperties(app->configuration.physicalDevice[0], &physicalDeviceProperties);
	app->configuration.maxThreadsNum = physicalDeviceProperties.limits.maxComputeWorkGroupInvocations;
	if (physicalDeviceProperties.vendorID == 0x8086) app->configuration.maxThreadsNum = 256; //Intel fix
	app->configuration.maxComputeWorkGroupCount[0] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[0];
	app->configuration.maxComputeWorkGroupCount[1] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[1];
	app->configuration.maxComputeWorkGroupCount[2] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[2];
	app->configuration.maxComputeWorkGroupSize[0] = physicalDeviceProperties.limits.maxComputeWorkGroupSize[0];
	app->configuration.maxComputeWorkGroupSize[1] = physicalDeviceProperties.limits.maxComputeWorkGroupSize[1];
	app->configuration.maxComputeWorkGroupSize[2] = physicalDeviceProperties.limits.maxComputeWorkGroupSize[2];
	//if ((physicalDeviceProperties.vendorID == 0x8086) && (!app->configuration.doublePrecision) && (!app->configuration.doublePrecisionFloatMemory)) app->configuration.halfThreads = 1;
	app->configuration.sharedMemorySize = physicalDeviceProperties.limits.maxComputeSharedMemorySize;
	app->configuration.vendorID = physicalDeviceProperties.vendorID;
	if (inputLaunchConfiguration.pipelineCache != 0) {
		app->configuration.pipelineCache = (VkPipelineCache*)calloc(1,sizeof(VkPipelineCache));
		if (!app->configuration.pipelineCache) {
			
			return VKFFT_ERROR_MALLOC_FAILED;
		}
		else {
			app->configuration.pipelineCache[0] = inputLaunchConfiguration.pipelineCache[0];
		}
	}
	app->configuration.useRaderUintLUT = 1;
	app->configuration.registerBoostNonPow2 = 0;
	app->configuration.registerBoost4Step = 1;
		
	switch (physicalDeviceProperties.vendorID) {
	case 0x10DE://NVIDIA
		app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 64 : 32;//the coalesced memory is equal to 32 bytes between L2 and VRAM.
		app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
		app->configuration.warpSize = 32;
		app->configuration.registerBoost = 4;
		app->configuration.reorderFourStep = 2;
		app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision) ? 4194305 : 4194305;
		break;
	case 0x8086://INTEL
		app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 128 : 64;
		app->configuration.useLUT = 1;
		app->configuration.warpSize = 32;
		app->configuration.registerBoost = (physicalDeviceProperties.limits.maxComputeSharedMemorySize >= 65536) ? 1 : 2;
		app->configuration.reorderFourStep = 1;
		app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 262144 : 524288;
		break;
	case 0x1002://AMD
		app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 64 : 32;
		app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
		app->configuration.warpSize = 64;
		app->configuration.registerBoost = (physicalDeviceProperties.limits.maxComputeSharedMemorySize >= 65536) ? 2 : 4;
		app->configuration.reorderFourStep = 3;
		app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 262144 : 524288;
		app->configuration.optimizePow2StridesTempBuffer = 2;
		break;
	default:
		app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 128 : 64;
		app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
		app->configuration.warpSize = 32;
		app->configuration.registerBoost = 1;
		app->configuration.reorderFourStep = 1;
		app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 262144 : 524288;
		break;
	}
	return VKFFT_SUCCESS;
};


static inline VkFFTResult findMemoryType123(VkFFTApplication* app, pfUINT memoryTypeBits, pfUINT memorySize, VkMemoryPropertyFlags properties, uint32_t* memoryTypeIndex) {
	VkPhysicalDeviceMemoryProperties memoryProperties = { 0 };

	vkGetPhysicalDeviceMemoryProperties(app->configuration.physicalDevice[0], &memoryProperties);

	for (pfUINT i = 0; i < memoryProperties.memoryTypeCount; ++i) {
		if ((memoryTypeBits & ((pfUINT)1 << i)) && ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) && (memoryProperties.memoryHeaps[memoryProperties.memoryTypes[i].heapIndex].size >= memorySize))
		{
			memoryTypeIndex[0] = (uint32_t)i;
			return VKFFT_SUCCESS;
		}
	}
	return VKFFT_ERROR_FAILED_TO_FIND_MEMORY;
}


static inline VkFFTResult allocateBufferVulkan123(VkFFTApplication* app, VkBuffer* buffer, VkDeviceMemory* deviceMemory, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkDeviceSize size) {
	VkFFTResult resFFT = VKFFT_SUCCESS;
	VkResult res = VK_SUCCESS;
	uint32_t queueFamilyIndices;
	VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 1;
	bufferCreateInfo.pQueueFamilyIndices = &queueFamilyIndices;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usageFlags;
	res = vkCreateBuffer(app->configuration.device[0], &bufferCreateInfo, 0, buffer);
	if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_BUFFER;
	VkMemoryRequirements memoryRequirements = { 0 };
	vkGetBufferMemoryRequirements(app->configuration.device[0], buffer[0], &memoryRequirements);
	VkMemoryAllocateInfo memoryAllocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	resFFT = findMemoryType123(app, memoryRequirements.memoryTypeBits, memoryRequirements.size, propertyFlags, &memoryAllocateInfo.memoryTypeIndex);
	if (resFFT != VKFFT_SUCCESS) return resFFT;
	res = vkAllocateMemory(app->configuration.device[0], &memoryAllocateInfo, 0, deviceMemory);
	if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_ALLOCATE_MEMORY;
	res = vkBindBufferMemory(app->configuration.device[0], buffer[0], deviceMemory[0], 0);
	if (res != VK_SUCCESS) return VKFFT_ERROR_FAILED_TO_BIND_BUFFER_MEMORY;
	return resFFT;
}


static inline VkFFTResult backendVkFFTallocateBuffer(VkFFTApplication* app) {
	return allocateBufferVulkan123(app, app->configuration.tempBuffer, &app->configuration.tempBufferDeviceMemory[0], VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, app->configuration.tempBufferSize[0]);
};


///////////////////////////////////////////////
//vkFFT/vkFFT_AppManagement/vkFFT_DeleteApp.h//
///////////////////////////////////////////////



static inline VkFFTResult deleteVkFFT_backendDestroyBuffer(VkDevice device, backendVkFFTBuffer buffer, VkDeviceMemory* deviceMemory){
	vkDestroyBuffer(device, buffer, 0);
	
	if (*deviceMemory != 0) {
		vkFreeMemory(device, *deviceMemory, 0);
		*deviceMemory = 0;
	}	
	return VKFFT_SUCCESS;
};


static inline void deleteVkFFT_backendFreeAPI(VkFFTApplication* app){
	if (app->configuration.isCompilerInitialized) {
		glslang_finalize_process();
		app->configuration.isCompilerInitialized = 0;
	}
	if (app->configuration.physicalDevice) {
		free(app->configuration.physicalDevice);
		app->configuration.physicalDevice = 0;
	}
	if (app->configuration.device) {
		free(app->configuration.device);
		app->configuration.device = 0;
	}
	if (app->configuration.queue) {
		free(app->configuration.queue);
		app->configuration.queue = 0;
	}
	if (app->configuration.commandPool) {
		free(app->configuration.commandPool);
		app->configuration.commandPool = 0;
	}
	if (app->configuration.fence) {
		free(app->configuration.fence);
		app->configuration.fence = 0;
	}
	if (app->configuration.pipelineCache != 0) {
		free(app->configuration.pipelineCache);
		app->configuration.pipelineCache = 0;
	}
	if (app->configuration.stagingBuffer != 0) {
		free(app->configuration.stagingBuffer);
		app->configuration.stagingBuffer = 0;
	}
	if (app->configuration.stagingBufferDeviceMemory != 0) {
		free(app->configuration.stagingBufferDeviceMemory);
		app->configuration.stagingBufferDeviceMemory = 0;
	}
}
	

#endif //VKFFT_BACKEND_APP_MANAGEMENT_H
