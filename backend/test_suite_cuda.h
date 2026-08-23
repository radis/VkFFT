
#ifndef VKFFT_TEST_SUITE_CUDA_H
#def VKFFT_TEST_SUITE_CUDA_H

#include <cuda.h>
#include <cuda_runtime.h>
#include <nvrtc.h>
#include <cuda_runtime_api.h>
#include <cuComplex.h>


#define VKFFT_BACKEND_NAME_STRING "CUDA"


typedef struct{
	CUdevice device;
	CUcontext context;
	uint64_t device_id;//an id of a device, reported by Vulkan device list
} VkGPU;

///////////////////////////////////////////////////////////


VkFFTResult allocateMemoryGPU(VkGPU* vkGPU, void** buffer, void** deviceMemoryVulkan, uint64_t bufferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cudaError_t res = cudaSuccess;

	res = cudaMalloc((void**)buffer, bufferSize);
	if (res != cudaSuccess) return VKFFT_ERROR_FAILED_TO_ALLOCATE;

	return VKFFT_SUCCESS;
}


VkFFTResult transferDataToCPU(VkGPU* vkGPU, void* cpu_arr, void* output_buffer, uint64_t transferSize) {
	//a function that transfers data from the GPU to the CPU using staging buffer, because the GPU memory is not host-coherent
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cudaError_t res = cudaSuccess;
	void* buffer = ((void**)output_buffer)[0];
	res = cudaMemcpy(cpu_arr, buffer, transferSize, cudaMemcpyDeviceToHost);
	if (res != cudaSuccess) {
		return VKFFT_ERROR_FAILED_TO_COPY;
	}

	return resFFT;
}


VkFFTResult transferDataFromCPU(VkGPU* vkGPU, void* cpu_arr, void* input_buffer, uint64_t transferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cudaError_t res = cudaSuccess;
	void* buffer = ((void**)input_buffer)[0];
	res = cudaMemcpy(buffer, cpu_arr, transferSize, cudaMemcpyHostToDevice);
	if (res != cudaSuccess) {
		return VKFFT_ERROR_FAILED_TO_COPY;
	}

	return resFFT;
}


VkFFTResult devices_list() {
	//this function creates an instance and prints the list of available devices

	CUresult res = CUDA_SUCCESS;
	res = cuInit(0);
	if (res != CUDA_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	int numDevices;
	res = cuDeviceGetCount(&numDevices);
	if (res != CUDA_SUCCESS) return VKFFT_ERROR_FAILED_TO_SET_DEVICE_ID;
	for (uint64_t i = 0; i < numDevices; i++) {
		char deviceName[256];
		CUdevice device = {};
		res = cuDeviceGet(&device, (int)i);
		if (res != CUDA_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		res = cuDeviceGetName(deviceName, 256, device);
		if (res != CUDA_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		printf("Device id: %" PRIu64 " name: %s\n", i, deviceName);
	}

	return VKFFT_SUCCESS;
}


VkFFTResult performVulkanFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, int inverse, uint64_t num_iter) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cudaError_t res = cudaSuccess;
	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, inverse, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	res = cudaDeviceSynchronize();
	if (res != cudaSuccess) return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;

	return resFFT;
}


VkFFTResult performVulkanFFTiFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, uint64_t num_iter, double* time_result) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cudaError_t res = cudaSuccess;
	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, -1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
		resFFT = VkFFTAppend(app, 1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	res = cudaDeviceSynchronize();
	if (res != cudaSuccess) return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;
	time_result[0] = totTime / num_iter;

	return resFFT;
}






////////////////////////////////////////////////////////////
// Backend specific functions used in VkFFT_testSuite.cpp //
////////////////////////////////////////////////////////////

VkFFTResult backendInitDevice(VkGPU* vkGPU, uint64_t sample_id){
	CUresult res = CUDA_SUCCESS;
	cudaError_t res2 = cudaSuccess;
	res = cuInit(0);
	if (res != CUDA_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	res2 = cudaSetDevice((int)vkGPU->device_id);
	if (res2 != cudaSuccess) return VKFFT_ERROR_FAILED_TO_SET_DEVICE_ID;
	res = cuDeviceGet(&vkGPU->device, (int)vkGPU->device_id);
	if (res != CUDA_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
	
	return VKFFT_SUCCESS;
};


VkFFTResult backendFreeDevice(VkGPU* vkGPU);
	return VKFFT_SUCCESS;
};

#endif //VKFFT_TEST_SUITE_CUDA_H