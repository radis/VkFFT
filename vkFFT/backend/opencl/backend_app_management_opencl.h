
#ifndef VKFFT_BACKEND_APP_MANAGEMENT_H
#define VKFFT_BACKEND_APP_MANAGEMENT_H

#include "backend/backend_structs.h"
#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"

//vkFFT/vkFFT_AppManagement/vkFFT_RunApp.h

static inline VkFFTResult VkFFTSync(VkFFTApplication* app) {
	return VKFFT_SUCCESS;
};

static inline void VkFFTAppend_backendSetCommandBuffer(VkFFTApplication* app){
    app->configuration.commandQueue = launchParams->commandQueue;
};

static inline void VkFFTAppend_backendBindPipelineAndDescriptorSets(VkFFTApplication* app, VkFFTAxis* axis) {};

//vkFFT/vkFFT_AppManagement/vkFFT_InitializeApp.h

static inline VkFFTResult setConfigurationVkFFT_backendSetDeviceParams(VkFFTApplication* app, VkFFTConfiguration inputLaunchConfiguration){
	cl_int res = 0;
	if (inputLaunchConfiguration.device == 0) {
		
		return VKFFT_ERROR_INVALID_DEVICE;
	}
	app->configuration.device = (cl_device_id*)calloc(1, sizeof(cl_device_id));
	if (!app->configuration.device) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.device[0] = inputLaunchConfiguration.device[0];
	}

	if (inputLaunchConfiguration.context == 0) {
		
		return VKFFT_ERROR_INVALID_CONTEXT;
	}
	app->configuration.context = (cl_context*)calloc(1, sizeof(cl_context));
	if (!app->configuration.context) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.context[0] = inputLaunchConfiguration.context[0];
	}

	cl_uint vendorID;
	size_t value_int64;
	cl_uint value_cl_uint;
	res = clGetDeviceInfo(app->configuration.device[0], CL_DEVICE_VENDOR_ID, sizeof(cl_int), &vendorID, 0);
	if (res != 0) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	res = clGetDeviceInfo(app->configuration.device[0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &value_int64, 0);
	if (res != 0) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxThreadsNum = value_int64;

	res = clGetDeviceInfo(app->configuration.device[0], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &value_cl_uint, 0);
	if (res != 0) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	size_t* dims = (size_t*)malloc(sizeof(size_t) * value_cl_uint);
	if (dims) {
		res = clGetDeviceInfo(app->configuration.device[0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * value_cl_uint, dims, 0);
		if (res != 0) {
			
			return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
		}
		app->configuration.maxComputeWorkGroupSize[0] = dims[0];
		app->configuration.maxComputeWorkGroupSize[1] = dims[1];
		app->configuration.maxComputeWorkGroupSize[2] = dims[2];
		free(dims);
		dims = 0;
	}
	else {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	app->configuration.maxComputeWorkGroupCount[0] = UINT64_MAX;
	app->configuration.maxComputeWorkGroupCount[1] = UINT64_MAX;
	app->configuration.maxComputeWorkGroupCount[2] = UINT64_MAX;
	//if ((vendorID == 0x8086) && (!app->configuration.doublePrecision) && (!app->configuration.doublePrecisionFloatMemory)) app->configuration.halfThreads = 1;
	cl_ulong sharedMemorySize;
	res = clGetDeviceInfo(app->configuration.device[0], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &sharedMemorySize, 0);
	if (res != 0) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.sharedMemorySize = sharedMemorySize;
	app->configuration.vendorID = vendorID;
	app->configuration.useRaderUintLUT = 1;
	switch (vendorID) {
	case 0x10DE://NVIDIA
		app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 64 : 32;//the coalesced memory is equal to 32 bytes between L2 and VRAM.
		app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
		app->configuration.warpSize = 32;
		app->configuration.registerBoostNonPow2 = 0;
		app->configuration.registerBoost = 4;
		app->configuration.registerBoost4Step = 1;
		app->configuration.reorderFourStep = 2;
		app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 4194305 : 4194305;
		app->configuration.sharedMemorySize -= 0x10;//reserved by system
		break;
	case 0x8086://INTEL
		app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 128 : 64;
		app->configuration.useLUT = 1;
		app->configuration.warpSize = 32;
		app->configuration.registerBoostNonPow2 = 0;
		app->configuration.registerBoost = (sharedMemorySize >= 65536) ? 1 : 2;
		app->configuration.registerBoost4Step = 1;
		app->configuration.reorderFourStep = 1;
		app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 262144 : 524288;
		break;
	case 0x1002://AMD
		app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 64 : 32;
		app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
		app->configuration.warpSize = 64;
		app->configuration.registerBoostNonPow2 = 0;
		app->configuration.registerBoost = (sharedMemorySize >= 65536) ? 2 : 4;
		app->configuration.registerBoost4Step = 1;
		app->configuration.reorderFourStep = 3;
		app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 262144 : 524288;
		app->configuration.optimizePow2StridesTempBuffer = 2;
		break;
	default:
		app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 128 : 64;
		app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
		app->configuration.warpSize = 32;
		app->configuration.registerBoostNonPow2 = 0;
		app->configuration.registerBoost = 1;
		app->configuration.registerBoost4Step = 1;
		app->configuration.reorderFourStep = 1;
		app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 262144 : 524288;
		break;
	}
	
	return VKFFT_SUCCESS;
};

static inline VkFFTResult backendVkFFTallocateBuffer(VkFFTApplication* app) {
	
	cl_int res = CL_SUCCESS;
	app->configuration.tempBuffer[0] = clCreateBuffer(app->configuration.context[0], CL_MEM_READ_WRITE, app->configuration.tempBufferSize[0], 0, &res);
	if (res != CL_SUCCESS) {
		deleteVkFFT(app);
		return VKFFT_ERROR_FAILED_TO_ALLOCATE;
	}
	return VKFFT_SUCCESS;
};


static inline VkFFTResult deleteVkFFT_backendDestroyBuffer(int device, backendVkFFTBuffer buffer){
	cl_int res = 0;
	res = clReleaseMemObject(buffer);
	if (res == 0) return VKFFT_SUCCESS;
	return VKFFT_ERROR_FAILED_TO_DESTROY_BUFFER;
};

static inline void deleteVkFFT_backendFreeAPI(VkFFTApplication* app){
	if (app->configuration.device) {
		free(app->configuration.device);
		app->configuration.device = 0;
	}
	if (app->configuration.context) {
		free(app->configuration.context);
		app->configuration.context = 0;
	}	
}

#endif //VKFFT_BACKEND_APP_MANAGEMENT_H