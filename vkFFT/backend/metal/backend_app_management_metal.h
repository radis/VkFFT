
#ifndef VKFFT_BACKEND_APP_MANAGEMENT_H
#define VKFFT_BACKEND_APP_MANAGEMENT_H

#include "backend/backend_structs.h"
#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"

//vkFFT/vkFFT_AppManagement/vkFFT_RunApp.h

static inline VkFFTResult VkFFTSync_backend(backendVkFFTConfiguration* config) {
	return VKFFT_SUCCESS;
};

static inline void VkFFTAppend_backendSetCommandBuffer(backendVkFFTConfiguration* config, backendVkFFTLaunchParams* launchParams){
    config->commandBuffer = launchParams->commandBuffer;
    config->commandEncoder = launchParams->commandEncoder;
};

static inline void VkFFTAppend_backendBindPipelineAndDescriptorSets(backendVkFFTConfiguration* config, backendVkFFTAxis* axis) {};

//vkFFT/vkFFT_AppManagement/vkFFT_InitializeApp.h

static inline VkFFTResult setConfigurationVkFFT_backendSetDeviceParams(VkFFTApplication* app, VkFFTConfiguration inputLaunchConfiguration){
	if (inputLaunchConfiguration.device == 0) {
		
		return VKFFT_ERROR_INVALID_DEVICE;
	}
    app->configuration.device = inputLaunchConfiguration.device;

	if (inputLaunchConfiguration.queue == 0) {
		
		return VKFFT_ERROR_INVALID_QUEUE;
	}
    app->configuration.queue = inputLaunchConfiguration.queue;

	const char dummy_kernel[50] = "kernel void VkFFT_dummy (){}";
	const char function_name[20] = "VkFFT_dummy";

	NS::Error* error;
	MTL::CompileOptions* compileOptions = MTL::CompileOptions::alloc();
	NS::String* str_code = NS::String::string(dummy_kernel, NS::UTF8StringEncoding);
	MTL::Library* dummy_library = app->configuration.device->newLibrary(str_code, compileOptions, &error);
	NS::String* str_name = NS::String::string(function_name, NS::UTF8StringEncoding);
	MTL::Function* function = dummy_library->newFunction(str_name);
	MTL::ComputePipelineState* dummy_state = app->configuration.device->newComputePipelineState(function, &error);

	MTL::Size size = app->configuration.device->maxThreadsPerThreadgroup();
	app->configuration.maxThreadsNum = dummy_state->maxTotalThreadsPerThreadgroup();

	app->configuration.maxComputeWorkGroupSize[0] = size.width;
	app->configuration.maxComputeWorkGroupSize[1] = size.height;
	app->configuration.maxComputeWorkGroupSize[2] = size.depth;

	if (app->configuration.maxThreadsNum > 256) {
		app->configuration.maxThreadsNum = 256;

		app->configuration.maxComputeWorkGroupSize[0] = 256;
		app->configuration.maxComputeWorkGroupSize[1] = 256;
		app->configuration.maxComputeWorkGroupSize[2] = 256;
		//The dummy kernel approach (above) does not work for some DCT-IV kernels (like 256x256x256). They refuse to have more than 256 threads. I will just force OpenCL thread limits for now.
	}

	app->configuration.maxComputeWorkGroupCount[0] = -1;
	app->configuration.maxComputeWorkGroupCount[1] = -1;
	app->configuration.maxComputeWorkGroupCount[2] = -1;

	app->configuration.sharedMemorySizeStatic = app->configuration.device->maxThreadgroupMemoryLength();
	app->configuration.sharedMemorySize = app->configuration.device->maxThreadgroupMemoryLength();

	app->configuration.warpSize = dummy_state->threadExecutionWidth();

	app->configuration.useRaderUintLUT = 1;

	app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 128 : 64;//the coalesced memory is equal to 64 bytes between L2 and VRAM.
	app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
	app->configuration.registerBoostNonPow2 = 0;
	app->configuration.registerBoost = 1;
	app->configuration.registerBoost4Step = 1;
	app->configuration.reorderFourStep = 1;
	app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 262144 : 524288;
	app->configuration.vendorID = 0x1027f00;

	dummy_state->release();
	function->release();
	str_name->release();
	dummy_library->release();
	str_code->release();
	compileOptions->release();
	
	return VKFFT_SUCCESS;
};

static inline VkFFTResult backendVkFFTallocateBuffer(VkFFTApplication* app) {
	app->configuration.tempBuffer[0] = app->configuration.device->newBuffer(app->configuration.tempBufferSize[0], MTL::ResourceStorageModePrivate);
	return VKFFT_SUCCESS;
};


static inline VkFFTResult deleteVkFFT_backendDestroyBuffer(int device, backendVkFFTBuffer* bufferPtr){
	((MTL::Buffer*)(*bufferPtr))->release();
	return VKFFT_SUCCESS;
};

static inline void deleteVkFFT_backendFreeAPI(backendVkFFTConfiguration* config){}

#endif //VKFFT_BACKEND_APP_MANAGEMENT_H