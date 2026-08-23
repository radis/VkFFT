
#ifndef VKFFT_TEST_SUITE_ZERO_H
#def VKFFT_TEST_SUITE_ZERO_H

#include <ze_api.h>


#define VKFFT_BACKEND_NAME_STRING "Level Zero"


typedef struct{
	ze_driver_handle_t driver;
	ze_device_handle_t device;
	ze_context_handle_t context;
	ze_command_queue_handle_t commandQueue;
	uint32_t commandQueueID;
	uint64_t device_id;//an id of a device, reported by Vulkan device list	
} VkGPU;

///////////////////////////////////////////////////////////

VkFFTResult allocateMemoryGPU(VkGPU* vkGPU, void** buffer, void** deviceMemoryVulkan, uint64_t bufferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	ze_result_t res = ZE_RESULT_SUCCESS;

	ze_device_mem_alloc_desc_t device_desc = {};
	device_desc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
	res = zeMemAllocDevice(vkGPU->context, &device_desc, bufferSize, sizeof(float), vkGPU->device, buffer);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_ALLOCATE;

	return VKFFT_SUCCESS;
}


VkFFTResult transferDataToCPU(VkGPU* vkGPU, void* cpu_arr, void* output_buffer, uint64_t transferSize) {
	//a function that transfers data from the GPU to the CPU using staging buffer, because the GPU memory is not host-coherent
	VkFFTResult resFFT = VKFFT_SUCCESS;

	ze_result_t res = ZE_RESULT_SUCCESS;
	void* buffer = ((void**)output_buffer)[0];
	ze_command_queue_desc_t commandQueueCopyDesc = {
			ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
			0,
			vkGPU->commandQueueID,
			0, // index
			0, // flags
			ZE_COMMAND_QUEUE_MODE_DEFAULT,
			ZE_COMMAND_QUEUE_PRIORITY_NORMAL
	};
	ze_command_list_handle_t copyCommandList;
	res = zeCommandListCreateImmediate(vkGPU->context, vkGPU->device, &commandQueueCopyDesc, &copyCommandList);
	if (res != ZE_RESULT_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	}
	res = zeCommandListAppendMemoryCopy(copyCommandList, cpu_arr, buffer, transferSize, 0, 0, 0);
	if (res != ZE_RESULT_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_COPY;
	}
	res = zeCommandQueueSynchronize(vkGPU->commandQueue, UINT32_MAX);
	if (res != ZE_RESULT_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
	}

	return resFFT;
}


VkFFTResult transferDataFromCPU(VkGPU* vkGPU, void* cpu_arr, void* input_buffer, uint64_t transferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	ze_result_t res = ZE_RESULT_SUCCESS;
	void* buffer = ((void**)input_buffer)[0];
	ze_command_queue_desc_t commandQueueCopyDesc = {
			ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
			0,
			vkGPU->commandQueueID,
			0, // index
			0, // flags
			ZE_COMMAND_QUEUE_MODE_DEFAULT,
			ZE_COMMAND_QUEUE_PRIORITY_NORMAL
	};
	ze_command_list_handle_t copyCommandList;
	res = zeCommandListCreateImmediate(vkGPU->context, vkGPU->device, &commandQueueCopyDesc, &copyCommandList);
	if (res != ZE_RESULT_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	}
	res = zeCommandListAppendMemoryCopy(copyCommandList, buffer, cpu_arr, transferSize, 0, 0, 0);
	if (res != ZE_RESULT_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_COPY;
	}
	res = zeCommandQueueSynchronize(vkGPU->commandQueue, UINT32_MAX);
	if (res != ZE_RESULT_SUCCESS) {
		return VKFFT_ERROR_FAILED_TO_SYNCHRONIZE;
	}

	return resFFT;
}


VkFFTResult devices_list() {
	//this function creates an instance and prints the list of available devices

	ze_result_t res = ZE_RESULT_SUCCESS;
	res = zeInit(0);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	uint32_t numDrivers = 0;
	res = zeDriverGet(&numDrivers, 0);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	ze_driver_handle_t* drivers = (ze_driver_handle_t*)malloc(numDrivers * sizeof(ze_driver_handle_t));
	if (!drivers) return VKFFT_ERROR_MALLOC_FAILED;
	res = zeDriverGet(&numDrivers, drivers);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	uint64_t k = 0;
	for (uint64_t j = 0; j < numDrivers; j++) {
		uint32_t numDevices = 0;
		res = zeDeviceGet(drivers[j], &numDevices, nullptr);
		if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		ze_device_handle_t* deviceList = (ze_device_handle_t*)malloc(numDevices * sizeof(ze_device_handle_t));
		if (!deviceList) return VKFFT_ERROR_MALLOC_FAILED;
		res = zeDeviceGet(drivers[j], &numDevices, deviceList);
		if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		for (uint64_t i = 0; i < numDevices; i++) {
			ze_device_properties_t device_properties;
			res = zeDeviceGetProperties(deviceList[i], &device_properties);
			if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_ENUMERATE_DEVICES;
			printf("Driver id: %" PRIu64 " Device id: %" PRIu64 " name: %s\n", j, k, device_properties.name);
			k++;
		}

		free(deviceList);
	}
	free(drivers);

	return VKFFT_SUCCESS;
}


VkFFTResult performVulkanFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, int inverse, uint64_t num_iter) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	ze_result_t res = ZE_RESULT_SUCCESS;
	ze_command_list_desc_t commandListDescription = {};
	commandListDescription.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC;
	ze_command_list_handle_t commandList = {};
	res = zeCommandListCreate(vkGPU->context, vkGPU->device, &commandListDescription, &commandList);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;

	launchParams->commandList = &commandList;
	//Record commands num_iter times. Allows to perform multiple convolutions/transforms in one submit.
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, inverse, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	res = zeCommandListClose(commandList);
	if (res != 0) return VKFFT_ERROR_FAILED_TO_END_COMMAND_BUFFER;

	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	res = zeCommandQueueExecuteCommandLists(vkGPU->commandQueue, 1, &commandList, 0);
	if (res != 0) return VKFFT_ERROR_FAILED_TO_SUBMIT_QUEUE;
	res = zeCommandQueueSynchronize(vkGPU->commandQueue, UINT32_MAX);
	if (res != 0) return VKFFT_ERROR_FAILED_TO_WAIT_FOR_FENCES;

	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;
	//printf("Pure submit execution time per num_iter: %.3f ms\n", totTime / num_iter);
	res = zeCommandListDestroy(commandList);
	if (res != 0) return VKFFT_ERROR_FAILED_TO_DESTROY_COMMAND_LIST;

	return resFFT;
}


VkFFTResult performVulkanFFTiFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, uint64_t num_iter, double* time_result) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	ze_result_t res = ZE_RESULT_SUCCESS;
	ze_command_list_desc_t commandListDescription = {};
	commandListDescription.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC;
	ze_command_list_handle_t commandList = {};
	res = zeCommandListCreate(vkGPU->context, vkGPU->device, &commandListDescription, &commandList);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;

	launchParams->commandList = &commandList;
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, -1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
		resFFT = VkFFTAppend(app, 1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	res = zeCommandListClose(commandList);
	if (res != 0) return VKFFT_ERROR_FAILED_TO_END_COMMAND_BUFFER;

	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	res = zeCommandQueueExecuteCommandLists(vkGPU->commandQueue, 1, &commandList, 0);
	if (res != 0) return VKFFT_ERROR_FAILED_TO_SUBMIT_QUEUE;
	res = zeCommandQueueSynchronize(vkGPU->commandQueue, UINT32_MAX);
	if (res != 0) return VKFFT_ERROR_FAILED_TO_WAIT_FOR_FENCES;

	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;
	time_result[0] = totTime / num_iter;
	res = zeCommandListDestroy(commandList);
	if (res != 0) return VKFFT_ERROR_FAILED_TO_DESTROY_COMMAND_LIST;

	return resFFT;
}




////////////////////////////////////////////////////////////
// Backend specific functions used in VkFFT_testSuite.cpp //
////////////////////////////////////////////////////////////

VkFFTResult backendInitDevice(VkGPU* vkGPU, uint64_t sample_id){
ze_result_t res = ZE_RESULT_SUCCESS;
	res = zeInit(0);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	uint32_t numDrivers = 0;
	res = zeDriverGet(&numDrivers, 0);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	ze_driver_handle_t* drivers = (ze_driver_handle_t*)malloc(numDrivers * sizeof(ze_driver_handle_t));
	if (!drivers) return VKFFT_ERROR_MALLOC_FAILED;
	res = zeDriverGet(&numDrivers, drivers);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_INITIALIZE;
	uint64_t k = 0;
	for (uint64_t j = 0; j < numDrivers; j++) {
		uint32_t numDevices = 0;
		res = zeDeviceGet(drivers[j], &numDevices, nullptr);
		if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		ze_device_handle_t* deviceList = (ze_device_handle_t*)malloc(numDevices * sizeof(ze_device_handle_t));
		if (!deviceList) return VKFFT_ERROR_MALLOC_FAILED;
		res = zeDeviceGet(drivers[j], &numDevices, deviceList);
		if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_GET_DEVICE;
		for (uint64_t i = 0; i < numDevices; i++) {
			if (k == vkGPU->device_id) {
				vkGPU->driver = drivers[j];
				vkGPU->device = deviceList[i];
				ze_context_desc_t contextDescription = {};
				contextDescription.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC;
				res = zeContextCreate(vkGPU->driver, &contextDescription, &vkGPU->context);
				if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_CONTEXT;

				uint32_t queueGroupCount = 0;
				res = zeDeviceGetCommandQueueGroupProperties(vkGPU->device, &queueGroupCount, 0);
				if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE;

				ze_command_queue_group_properties_t* cmdqueueGroupProperties = (ze_command_queue_group_properties_t*)malloc(queueGroupCount * sizeof(ze_command_queue_group_properties_t));
				if (!cmdqueueGroupProperties) return VKFFT_ERROR_MALLOC_FAILED;
				res = zeDeviceGetCommandQueueGroupProperties(vkGPU->device, &queueGroupCount, cmdqueueGroupProperties);
				if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE;

				uint32_t commandQueueID = -1;
				for (uint32_t i = 0; i < queueGroupCount; ++i) {
					if ((cmdqueueGroupProperties[i].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE) && (cmdqueueGroupProperties[i].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY)) {
						commandQueueID = i;
						break;
					}
				}
				if (commandQueueID == -1) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE;
				vkGPU->commandQueueID = commandQueueID;
				ze_command_queue_desc_t commandQueueDescription = {};
				commandQueueDescription.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC;
				commandQueueDescription.ordinal = commandQueueID;
				commandQueueDescription.priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL;
				commandQueueDescription.mode = ZE_COMMAND_QUEUE_MODE_DEFAULT;
				res = zeCommandQueueCreate(vkGPU->context, vkGPU->device, &commandQueueDescription, &vkGPU->commandQueue);
				if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE;
				free(cmdqueueGroupProperties);
				i=numDevices;
				j=numDrivers;
			}
			else {
				k++;
			}
		}

		free(deviceList);
	}
	free(drivers);
	
	return VKFFT_SUCCESS;
};


VkFFTResult backendFreeDevice(VkGPU* vkGPU);
	res = zeCommandQueueDestroy(vkGPU->commandQueue);
	if (res != ZE_RESULT_SUCCESS) return VKFFT_ERROR_FAILED_TO_RELEASE_COMMAND_QUEUE;
	res = zeContextDestroy(vkGPU->context);

	return VKFFT_SUCCESS;
};





#endif //VKFFT_TEST_SUITE_ZERO_H