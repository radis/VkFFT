
#ifndef VKFFT_BACKEND_APP_MANAGEMENT_H
#define VKFFT_BACKEND_APP_MANAGEMENT_H

#include "backend/backend_app_management.h"
#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"

//vkFFT/vkFFT_AppManagement/vkFFT_RunApp.h

static inline VkFFTResult VkFFTSync(backendVkFFTConfiguration* config) {
    if (config->num_streams > 1) {
        cudaError_t res = cudaSuccess;
        for (pfUINT s = 0; s < config->num_streams; s++) {
            res = cudaEventSynchronize(config->stream_event[s]);
            if (res != cudaSuccess) return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
        }
        config->streamCounter = 0;
    }
	return VKFFT_SUCCESS;
};

static inline void VkFFTAppend_backendSetCommandBuffer(backendVkFFTConfiguration* config, backendVkFFTLaunchParams* launchParams){
    config->streamCounter = 0;
};

static inline void VkFFTAppend_backendBindPipelineAndDescriptorSets(backendVkFFTConfiguration* config, backendVkFFTAxis* axis) {};

//vkFFT/vkFFT_AppManagement/vkFFT_InitializeApp.h

static inline VkFFTResult setConfigurationVkFFT_backendSetDeviceParams(VkFFTApplication* app, VkFFTConfiguration inputLaunchConfiguration){
	CUresult res = CUDA_SUCCESS;
	cudaError_t res_t = cudaSuccess;
	if (inputLaunchConfiguration.device == 0) {
		return VKFFT_ERROR_INVALID_DEVICE;
	}
	app->configuration.device = (CUdevice*)calloc(1, sizeof(CUdevice));
	if (!app->configuration.device) {
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.device[0] = inputLaunchConfiguration.device[0];
	}

	if (inputLaunchConfiguration.num_streams != 0) {
		app->configuration.num_streams = inputLaunchConfiguration.num_streams;
		if (inputLaunchConfiguration.stream != 0) {
			app->configuration.stream = (cudaStream_t*)calloc(app->configuration.num_streams, sizeof(cudaStream_t));
			if (!app->configuration.stream) {
				return VKFFT_ERROR_MALLOC_FAILED;
			}
			else {
				for (pfUINT i = 0; i < app->configuration.num_streams; i++)
					app->configuration.stream[i] = inputLaunchConfiguration.stream[i];
			}
		}
	}
	app->configuration.streamID = 0;
	int value = 0;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.computeCapabilityMajor = value;

	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.computeCapabilityMinor = value;

	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxThreadsNum = value;

	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupCount[0] = value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupCount[1] = value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupCount[2] = value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupSize[0] = value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupSize[1] = value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupSize[2] = value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.sharedMemorySizeStatic = value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK_OPTIN, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.sharedMemorySize = value;// (value > 65536) ? 65536 : value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_WARP_SIZE, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.warpSize = value;
	res = cuDeviceGetAttribute(&value, CU_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO, app->configuration.device[0]);
	if (res != CUDA_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.useLUT_4step = (value <= 4) ? -1 : 1;
	//we don't need this in CUDA
	app->configuration.useRaderUintLUT = 0;
	if (app->configuration.num_streams > 1) {
		app->configuration.stream_event = (cudaEvent_t*)malloc(app->configuration.num_streams * sizeof(cudaEvent_t));
		if (!app->configuration.stream_event) {
			return VKFFT_ERROR_MALLOC_FAILED;
		}
		for (pfUINT i = 0; i < app->configuration.num_streams; i++) {
			res_t = cudaEventCreate(&app->configuration.stream_event[i]);
			if (res_t != cudaSuccess) {
				return VKFFT_ERROR_FAILED_TO_CREATE_EVENT;
			}
		}
	}

	app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 64 : 32;//the coalesced memory is equal to 32 bytes between L2 and VRAM.
	app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
	app->configuration.registerBoostNonPow2 = 0;
	app->configuration.registerBoost = (app->configuration.sharedMemorySize <= 65536) ? 2 : 1;
	app->configuration.registerBoost4Step = 1;
	app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 16777216 : 33554432;
	app->configuration.reorderFourStep = 3;
	app->configuration.vendorID = 0x10DE;
	
	return VKFFT_SUCCESS;
};

static inline VkFFTResult backendVkFFTallocateBuffer(VkFFTApplication* app) {
	cudaError_t res = cudaMalloc(app->configuration.tempBuffer, app->configuration.tempBufferSize[0]);
	if (res != cudaSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_ALLOCATE;
	}
	return VKFFT_SUCCESS;
};

static inline VkFFTResult deleteVkFFT_backendDestroyBuffer(int device, backendVkFFTBuffer* bufferPtr){
	cudaError_t res_t = cudaSuccess;
	
	res_t = cudaFree(*bufferPtr);
	if (res_t == cudaSuccess) return VKFFT_SUCCESS;
	
	return VKFFT_ERROR_FAILED_TO_DESTROY_BUFFER;
};


static inline void deleteVkFFT_backendFreeAPI(backendVkFFTConfiguration* config){
	if (config->device) {
		free(config->device);
		config->device = 0;
	}
	if (config->stream) {
		free(config->stream);
		config->stream = 0;
	}
	if (config->num_streams > 1) {
		cudaError_t res_t = cudaSuccess;
		for (pfUINT i = 0; i < config->num_streams; i++) {
			if (config->stream_event[i] != 0) {
				res_t = cudaEventDestroy(config->stream_event[i]);
				if (res_t == cudaSuccess) config->stream_event[i] = 0;
			}
		}
		if (config->stream_event != 0) {
			free(config->stream_event);
			config->stream_event = 0;
		}
	}	
}
	
#endif //VKFFT_BACKEND_APP_MANAGEMENT_H