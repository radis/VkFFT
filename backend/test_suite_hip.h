
#ifndef VKFFT_TEST_SUITE_HIP_H
#def VKFFT_TEST_SUITE_HIP_H

#ifndef __HIP_PLATFORM_HCC__
#define __HIP_PLATFORM_HCC__
#endif
#include <hip/hip_runtime.h>
#include <hip/hiprtc.h>
#include <hip/hip_runtime_api.h>
#include <hip/hip_complex.h>


#define VKFFT_BACKEND_NAME_STRING "HIP"


typedef struct{
	hipDevice_t device;
	hipCtx_t context;	
	uint64_t device_id;//an id of a device, reported by Vulkan device list
} VkGPU;

///////////////////////////////////////////////////////////


VkFFTResult allocateMemoryGPU(VkGPU* vkGPU, void** buffer, void** deviceMemoryVulkan, uint64_t bufferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	hipError_t res = hipSuccess;

	res = hipMalloc((void**)buffer, bufferSize);
	if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_ALLOCATE;

	return VKFFT_SUCCESS;
}


VkFFTResult transferDataToCPU(VkGPU* vkGPU, void* cpu_arr, void* output_buffer, uint64_t transferSize) {
	//a function that transfers data from the GPU to the CPU using staging buffer, because the GPU memory is not host-coherent
	VkFFTResult resFFT = VKFFT_SUCCESS;

	hipError_t res = hipSuccess;
	void* buffer = ((void**)output_buffer)[0];
	res = hipMemcpy(cpu_arr, buffer, transferSize, hipMemcpyDeviceToHost);
	if (res != hipSuccess) {
		return VKFFT_ERROR_FAILED_TO_COPY;
	}

	return resFFT;
}


VkFFTResult transferDataFromCPU(VkGPU* vkGPU, void* cpu_arr, void* input_buffer, uint64_t transferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	hipError_t res = hipSuccess;
	void* buffer = ((void**)input_buffer)[0];
	res = hipMemcpy(buffer, cpu_arr, transferSize, hipMemcpyHostToDevice);
	if (res != hipSuccess) {
		return VKFFT_ERROR_FAILED_TO_COPY;
	}

	return resFFT;
}



VkFFTResult devices_list() {
	//this function creates an instance and prints the list of available devices

	hipError_t res = hipSuccess;
	res = hipInit(0);
	if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	int numDevices;
	res = hipGetDeviceCount(&numDevices);
	if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_SET_DEVICE_ID;
	for (uint64_t i = 0; i < numDevices; i++) {
		char deviceName[256];
		hipDevice_t device = {};
		res = hipDeviceGet(&device, i);
		if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		res = hipDeviceGetName(deviceName, 256, device);
		if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		printf("Device id: %" PRIu64 " name: %s\n", i, deviceName);
	}

	return VKFFT_SUCCESS;
}


VkFFTResult performVulkanFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, int inverse, uint64_t num_iter) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	hipError_t res = hipSuccess;
	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, inverse, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	res = hipDeviceSynchronize();
	if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;

	return resFFT;
}



VkFFTResult performVulkanFFTiFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, uint64_t num_iter, double* time_result) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	hipError_t res = hipSuccess;
	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, -1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
		resFFT = VkFFTAppend(app, 1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	res = hipDeviceSynchronize();
	if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;
	time_result[0] = totTime / num_iter;

	return resFFT;
}





////////////////////////////////////////////////////////////
// Backend specific functions used in VkFFT_testSuite.cpp //
////////////////////////////////////////////////////////////

VkFFTResult backendInitDevice(VkGPU* vkGPU, uint64_t sample_id){
	hipError_t res = hipSuccess;
	res = hipInit(0);
	if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	res = hipSetDevice((int)vkGPU->device_id);
	if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_SET_DEVICE_ID;
	res = hipDeviceGet(&vkGPU->device, (int)vkGPU->device_id);
	if (res != hipSuccess) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;

	return VKFFT_SUCCESS;
};


VkFFTResult backendFreeDevice(VkGPU* vkGPU);
	return VKFFT_SUCCESS;
};

#endif // VKFFT_TEST_SUITE_HIP_H