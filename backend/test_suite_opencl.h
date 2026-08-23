
#ifndef VKFFT_TEST_SUITE_OPENCL_H
#def VKFFT_TEST_SUITE_OPENCL_H

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif 


#define VKFFT_BACKEND_NAME_STRING "OpenCL"


typedef struct{
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue commandQueue;
	uint64_t device_id;//an id of a device, reported by Vulkan device list
} VkGPU;

///////////////////////////////////////////////////////////

VkFFTResult allocateMemoryGPU(VkGPU* vkGPU, void** buffer, void** deviceMemoryVulkan, uint64_t bufferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cl_int res = CL_SUCCESS;

	cl_mem* buffer_ref = (cl_mem*)buffer;
	buffer_ref[0] = clCreateBuffer(vkGPU->context, CL_MEM_READ_WRITE, bufferSize, 0, &res);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_ALLOCATE;

	return VKFFT_SUCCESS;
}


VkFFTResult transferDataToCPU(VkGPU* vkGPU, void* cpu_arr, void* output_buffer, uint64_t transferSize) {
	//a function that transfers data from the GPU to the CPU using staging buffer, because the GPU memory is not host-coherent
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cl_int res = CL_SUCCESS;
	cl_mem* buffer = (cl_mem*)output_buffer;
	cl_command_queue commandQueue = clCreateCommandQueue(vkGPU->context, vkGPU->device, 0, &res);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE;
	res = clEnqueueReadBuffer(commandQueue, buffer[0], CL_TRUE, 0, transferSize, cpu_arr, 0, NULL, NULL);
	if (res != CL_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_COPY;
	}
	res = clReleaseCommandQueue(commandQueue);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_RELEASE_COMMAND_QUEUE;

	return resFFT;
}


VkFFTResult transferDataFromCPU(VkGPU* vkGPU, void* cpu_arr, void* input_buffer, uint64_t transferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cl_int res = CL_SUCCESS;
	cl_mem* buffer = (cl_mem*)input_buffer;
	cl_command_queue commandQueue = clCreateCommandQueue(vkGPU->context, vkGPU->device, 0, &res);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE;
	res = clEnqueueWriteBuffer(commandQueue, buffer[0], CL_TRUE, 0, transferSize, cpu_arr, 0, NULL, NULL);
	if (res != CL_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_COPY;
	}
	res = clReleaseCommandQueue(commandQueue);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_RELEASE_COMMAND_QUEUE;

	return resFFT;
}


VkFFTResult devices_list() {
	//this function creates an instance and prints the list of available devices

	cl_int res = CL_SUCCESS;
	cl_uint numPlatforms;
	res = clGetPlatformIDs(0, 0, &numPlatforms);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	cl_platform_id* platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * numPlatforms);
	if (!platforms) return VKFFT_ERROR_MALLOC_FAILED;
	res = clGetPlatformIDs(numPlatforms, platforms, 0);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	uint64_t k = 0;
	for (uint64_t j = 0; j < numPlatforms; j++) {
		cl_uint numDevices;
		res = clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL, 0, 0, &numDevices);
		cl_device_id* deviceList = (cl_device_id*)malloc(sizeof(cl_device_id) * numDevices);
		if (!deviceList) return VKFFT_ERROR_MALLOC_FAILED;
		res = clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL, numDevices, deviceList, 0);
		if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		for (uint64_t i = 0; i < numDevices; i++) {
			char deviceName[256];
			char apiVersion[256];
			res = clGetDeviceInfo(deviceList[i], CL_DEVICE_NAME, 256 * sizeof(char), deviceName, 0);
			if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_ENUMERATE_DEVICES;
			res = clGetDeviceInfo(deviceList[i], CL_DEVICE_VERSION, 256 * sizeof(char), apiVersion, 0);
			if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_ENUMERATE_DEVICES;
			printf("Platform id: %" PRIu64 " Device id: %" PRIu64 " name: %s API:%s\n", j, k, deviceName, apiVersion);
			k++;
		}
		free(deviceList);
	}
	free(platforms);

	return VKFFT_SUCCESS;
}


VkFFTResult performVulkanFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, int inverse, uint64_t num_iter) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cl_int res = CL_SUCCESS;
	launchParams->commandQueue = &vkGPU->commandQueue;
	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, inverse, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	res = clFinish(vkGPU->commandQueue);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;

	return resFFT;
}


VkFFTResult performVulkanFFTiFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, uint64_t num_iter, double* time_result) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	cl_int res = CL_SUCCESS;
	launchParams->commandQueue = &vkGPU->commandQueue;
	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, -1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
		resFFT = VkFFTAppend(app, 1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	res = clFinish(vkGPU->commandQueue);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;
	time_result[0] = totTime / num_iter;

	return resFFT;
}






////////////////////////////////////////////////////////////
// Backend specific functions used in VkFFT_testSuite.cpp //
////////////////////////////////////////////////////////////

VkFFTResult backendInitDevice(VkGPU* vkGPU, uint64_t sample_id){
	cl_int res = CL_SUCCESS;
	cl_uint numPlatforms;
	res = clGetPlatformIDs(0, 0, &numPlatforms);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	cl_platform_id* platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * numPlatforms);
	if (!platforms) return VKFFT_ERROR_MALLOC_FAILED;
	res = clGetPlatformIDs(numPlatforms, platforms, 0);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	uint64_t k = 0;
	for (uint64_t j = 0; j < numPlatforms; j++) {
		cl_uint numDevices;
		res = clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL, 0, 0, &numDevices);
		cl_device_id* deviceList = (cl_device_id*)malloc(sizeof(cl_device_id) * numDevices);
		if (!deviceList) return VKFFT_ERROR_MALLOC_FAILED;
		res = clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL, numDevices, deviceList, 0);
		if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		for (uint64_t i = 0; i < numDevices; i++) {
			if (k == vkGPU->device_id) {
				vkGPU->platform = platforms[j];
				vkGPU->device = deviceList[i];
				vkGPU->context = clCreateContext(NULL, 1, &vkGPU->device, NULL, NULL, &res);
				if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_CONTEXT;
				cl_command_queue commandQueue = clCreateCommandQueue(vkGPU->context, vkGPU->device, 0, &res);
				if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE;
				vkGPU->commandQueue = commandQueue;
				i=numDevices;
				j=numPlatforms;
			}
			else {
				k++;
			}
		}
		free(deviceList);
	}
	free(platforms);
	
	return VKFFT_SUCCESS;
};


VkFFTResult backendFreeDevice(VkGPU* vkGPU);
	res = clReleaseCommandQueue(vkGPU->commandQueue);
	if (res != CL_SUCCESS) return VKFFT_ERROR_FAILED_TO_RELEASE_COMMAND_QUEUE;
	clReleaseContext(vkGPU->context);

	return VKFFT_SUCCESS;
};






#endif //VKFFT_TEST_SUITE_OPENCL_H