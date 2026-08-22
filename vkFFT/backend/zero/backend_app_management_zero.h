
#ifndef VKFFT_BACKEND_APP_MANAGEMENT_H
#define VKFFT_BACKEND_APP_MANAGEMENT_H

#include "backend/backend_structs.h"
#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"

//vkFFT/vkFFT_AppManagement/vkFFT_RunApp.h

static inline VkFFTResult VkFFTSync_backend(backendVkFFTConfiguration* config) {
	ze_result_t res = ZE_RESULT_SUCCESS;
    res = zeCommandListAppendBarrier(config->commandList[0], nullptr, 0, nullptr);
    if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_SUBMIT_BARRIER;
	return VKFFT_SUCCESS;
};

static inline void VkFFTAppend_backendSetCommandBuffer(backendVkFFTConfiguration* config, backendVkFFTLaunchParams* launchParams){
    config->commandList = launchParams->commandList;
};

static inline void VkFFTAppend_backendBindPipelineAndDescriptorSets(backendVkFFTConfiguration* config, backendVkFFTAxis* axis) {};

//vkFFT/vkFFT_AppManagement/vkFFT_InitializeApp.h

static inline VkFFTResult setConfigurationVkFFT_backendSetDeviceParams(VkFFTApplication* app, VkFFTConfiguration inputLaunchConfiguration){
	ze_result_t res = ZE_RESULT_SUCCESS;
	if (inputLaunchConfiguration.device == 0) {
		
		return VKFFT_ERROR_INVALID_DEVICE;
	}
	app->configuration.device = (ze_device_handle_t*)calloc(1, sizeof(ze_device_handle_t));
	if (!app->configuration.device) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.device[0] = inputLaunchConfiguration.device[0];
	}

	if (inputLaunchConfiguration.context == 0) {
		
		return VKFFT_ERROR_INVALID_CONTEXT;
	}
	app->configuration.context = (ze_context_handle_t*)calloc(1, sizeof(ze_context_handle_t));
	if (!app->configuration.context) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.context[0] = inputLaunchConfiguration.context[0];
	}

	if (inputLaunchConfiguration.commandQueue == 0) {
		
		return VKFFT_ERROR_INVALID_QUEUE;
	}
	app->configuration.commandQueue = (ze_command_queue_handle_t*)calloc(1, sizeof(ze_command_queue_handle_t));
	if (!app->configuration.commandQueue) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.commandQueue[0] = inputLaunchConfiguration.commandQueue[0];
	}

	app->configuration.commandQueueID = inputLaunchConfiguration.commandQueueID;
	ze_device_properties_t device_properties;
	ze_device_compute_properties_t compute_properties;
	res = zeDeviceGetProperties(app->configuration.device[0], &device_properties);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	res = zeDeviceGetComputeProperties(app->configuration.device[0], &compute_properties);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	uint32_t vendorID = device_properties.vendorId;
	app->configuration.maxThreadsNum = compute_properties.maxTotalGroupSize;
	app->configuration.maxComputeWorkGroupSize[0] = compute_properties.maxGroupSizeX;
	app->configuration.maxComputeWorkGroupSize[1] = compute_properties.maxGroupSizeY;
	app->configuration.maxComputeWorkGroupSize[2] = compute_properties.maxGroupSizeZ;

	app->configuration.maxComputeWorkGroupCount[0] = compute_properties.maxGroupCountX;
	app->configuration.maxComputeWorkGroupCount[1] = compute_properties.maxGroupCountY;
	app->configuration.maxComputeWorkGroupCount[2] = compute_properties.maxGroupCountZ;
	//if ((vendorID == 0x8086) && (!app->configuration.doublePrecision) && (!app->configuration.doublePrecisionFloatMemory)) app->configuration.halfThreads = 1;
	app->configuration.sharedMemorySize = compute_properties.maxSharedLocalMemory;

	app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 128 : 64;
	app->configuration.useLUT = 1;
	app->configuration.warpSize = device_properties.physicalEUSimdWidth;
	app->configuration.registerBoostNonPow2 = 0;
	app->configuration.registerBoost = (app->configuration.sharedMemorySize >= 65536) ? 1 : 2;
	app->configuration.registerBoost4Step = 1;
	app->configuration.reorderFourStep = 1;
	app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 262144 : 524288;
	app->configuration.vendorID = 0x8086;
	app->configuration.useRaderUintLUT = 1;
	
	return VKFFT_SUCCESS;
};

static inline VkFFTResult backendVkFFTallocateBuffer(VkFFTApplication* app) {
	ze_device_mem_alloc_desc_t device_desc = VKFFT_ZERO_INIT;
	device_desc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
	ze_result_t res = zeMemAllocDevice(app->configuration.context[0], &device_desc, app->configuration.tempBufferSize[0], sizeof(float), app->configuration.device[0], app->configuration.tempBuffer);
	if (res != ZE_RESULT_SUCCESS) {
		deleteVkFFT(app);
		return VKFFT_ERROR_FAILED_TO_ALLOCATE;
	}
	return VKFFT_SUCCESS;
};

static inline VkFFTResult deleteVkFFT_backendDestroyBuffer(ze_context_handle_t context, backendVkFFTBuffer* bufferPtr){
	ze_result_t res = ZE_RESULT_SUCCESS;
	res = zeMemFree(context, *bufferPtr);
	if (res == ZE_RESULT_SUCCESS) return VKFFT_SUCCESS;
	return VKFFT_ERROR_FAILED_TO_DESTROY_BUFFER;
};

static inline void deleteVkFFT_backendFreeAPI(backendVkFFTConfiguration* config){
	if (config->device) {
		free(config->device);
		config->device = 0;
	}
	if (config->context) {
		free(config->context);
		config->context = 0;
	}
	if (config->commandQueue) {
		free(config->commandQueue);
		config->commandQueue = 0;
	}	
}

#endif //VKFFT_BACKEND_APP_MANAGEMENT_H