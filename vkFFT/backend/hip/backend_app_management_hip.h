
#ifndef VKFFT_BACKEND_APP_MANAGEMENT_H
#define VKFFT_BACKEND_APP_MANAGEMENT_H

#include "backend/backend_structs.h"
#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"

//vkFFT/vkFFT_AppManagement/vkFFT_RunApp.h

static inline VkFFTResult VkFFTSync(VkFFTApplication* app) {
    if (app->configuration.num_streams > 1) {
        hipError_t res = hipSuccess;
        for (pfUINT s = 0; s < app->configuration.num_streams; s++) {
            res = hipEventSynchronize(app->configuration.stream_event[s]);
            if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
        }
        app->configuration.streamCounter = 0;
    }
	return VKFFT_SUCCESS;
};

static inline void VkFFTAppend_backendSetCommandBuffer(VkFFTApplication* app){
    app->configuration.streamCounter = 0;
};
	
static inline void VkFFTAppend_backendBindPipelineAndDescriptorSets(VkFFTApplication* app, VkFFTAxis* axis) {};

//vkFFT/vkFFT_AppManagement/vkFFT_InitializeApp.h

static inline VkFFTResult setConfigurationVkFFT_backendSetDeviceParams(VkFFTApplication* app, VkFFTConfiguration inputLaunchConfiguration){
	hipError_t res = hipSuccess;
	if (inputLaunchConfiguration.device == 0) {
		
		return VKFFT_ERROR_INVALID_DEVICE;
	}
	app->configuration.device = (hipDevice_t*)calloc(1, sizeof(hipDevice_t));
	if (!app->configuration.device) {
		
		return VKFFT_ERROR_MALLOC_FAILED;
	}
	else {
		app->configuration.device[0] = inputLaunchConfiguration.device[0];
	}

	if (inputLaunchConfiguration.num_streams != 0) {
		app->configuration.num_streams = inputLaunchConfiguration.num_streams;
		if (inputLaunchConfiguration.stream != 0) {
			app->configuration.stream = (hipStream_t*)calloc(app->configuration.num_streams, sizeof(hipStream_t));
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
	res = hipDeviceGetAttribute(&value, hipDeviceAttributeComputeCapabilityMajor, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.computeCapabilityMajor = value;

	res = hipDeviceGetAttribute(&value, hipDeviceAttributeComputeCapabilityMinor, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.computeCapabilityMinor = value;

	res = hipDeviceGetAttribute(&value, hipDeviceAttributeMaxThreadsPerBlock, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxThreadsNum = value;

	res = hipDeviceGetAttribute(&value, hipDeviceAttributeMaxGridDimX, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupCount[0] = value;
	res = hipDeviceGetAttribute(&value, hipDeviceAttributeMaxGridDimY, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupCount[1] = value;
	res = hipDeviceGetAttribute(&value, hipDeviceAttributeMaxGridDimZ, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupCount[2] = value;
	res = hipDeviceGetAttribute(&value, hipDeviceAttributeMaxBlockDimX, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupSize[0] = value;
	res = hipDeviceGetAttribute(&value, hipDeviceAttributeMaxBlockDimY, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupSize[1] = value;
	res = hipDeviceGetAttribute(&value, hipDeviceAttributeMaxBlockDimZ, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.maxComputeWorkGroupSize[2] = value;
	res = hipDeviceGetAttribute(&value, hipDeviceAttributeMaxSharedMemoryPerBlock, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.sharedMemorySizeStatic = value;
	//hipDeviceGetAttribute(&value, hipDeviceAttributeMaxSharedMemoryPerBlockOptin, app->configuration.device[0]);
	app->configuration.sharedMemorySize = value;// (value > 65536) ? 65536 : value;
	res = hipDeviceGetAttribute(&value, hipDeviceAttributeWarpSize, app->configuration.device[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE;
	}
	app->configuration.warpSize = value;
	app->configuration.useRaderUintLUT = 0;
	if (app->configuration.num_streams > 1) {
		app->configuration.stream_event = (hipEvent_t*)malloc(app->configuration.num_streams * sizeof(hipEvent_t));
		if (!app->configuration.stream_event) {
			
			return VKFFT_ERROR_MALLOC_FAILED;
		}
		for (pfUINT i = 0; i < app->configuration.num_streams; i++) {
			res = hipEventCreate(&app->configuration.stream_event[i]);
			if (res != hipSuccess) {
				
				return VKFFT_ERROR_FAILED_TO_CREATE_EVENT;
			}
		}
	}
	app->configuration.coalescedMemory = (app->configuration.halfPrecision) ? 64 : 32;
	app->configuration.useLUT = (app->configuration.doublePrecision || app->configuration.doublePrecisionFloatMemory || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1 : -1;
	app->configuration.useLUT_4step = -1;
	app->configuration.registerBoostNonPow2 = 0;
	app->configuration.registerBoost = 4;
	app->configuration.registerBoost4Step = 1;
	app->configuration.reorderFourStep = 3;
	app->configuration.swapTo3Stage4Step = (app->configuration.doublePrecision || app->configuration.quadDoubleDoublePrecision || app->configuration.quadDoubleDoublePrecisionDoubleMemory) ? 1048576 : 2097152;
	app->configuration.vendorID = 0x1002;
	app->configuration.optimizePow2StridesTempBuffer = 2;
	
	return VKFFT_SUCCESS;
};

static inline VkFFTResult backendVkFFTallocateBuffer(VkFFTApplication* app) {
	hipError_t res = hipMalloc(app->configuration.tempBuffer, app->configuration.tempBufferSize[0]);
	if (res != hipSuccess) {
		
		return VKFFT_ERROR_FAILED_TO_ALLOCATE;
	}
	return VKFFT_SUCCESS;
};


static inline VkFFTResult deleteVkFFT_backendDestroyBuffer(int device, backendVkFFTBuffer buffer){
	hipError_t res_t = hipSuccess;
	res_t = hipFree(buffer);
	if (res_t == hipSuccess) return VKFFT_SUCCESS;
	return VKFFT_ERROR_FAILED_TO_DESTROY_BUFFER;
};


static inline void deleteVkFFT_backendFreeAPI(VkFFTApplication* app){
	if (app->configuration.device) {
		free(app->configuration.device);
		app->configuration.device = 0;
	}
	if (app->configuration.stream) {
		free(app->configuration.stream);
		app->configuration.stream = 0;
	}
	if (app->configuration.num_streams > 1) {
		hipError_t res_t = hipSuccess;
		for (pfUINT i = 0; i < app->configuration.num_streams; i++) {
			if (app->configuration.stream_event[i] != 0) {
				res_t = hipEventDestroy(app->configuration.stream_event[i]);
				if (res_t == hipSuccess) app->configuration.stream_event[i] = 0;
			}
		}
		if (app->configuration.stream_event != 0) {
			free(app->configuration.stream_event);
			app->configuration.stream_event = 0;
		}
	}	
}


#endif //VKFFT_BACKEND_APP_MANAGEMENT_H