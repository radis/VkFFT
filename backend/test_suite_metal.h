
#ifndef VKFFT_TEST_SUITE_METAL_H
#def VKFFT_TEST_SUITE_METAL_H

#ifndef NS_PRIVATE_IMPLEMENTATION
#define NS_PRIVATE_IMPLEMENTATION
#endif
#ifndef CA_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#endif
#ifndef MTL_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#endif
#include "Foundation/Foundation.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "Metal/Metal.hpp"


#define VKFFT_BACKEND_NAME_STRING "Metal"


typedef struct{
    MTL::Device* device;
    MTL::CommandQueue* queue;
	uint64_t device_id;//an id of a device, reported by Vulkan device list
} VkGPU;

///////////////////////////////////////////////////////////


VkFFTResult allocateMemoryGPU(VkGPU* vkGPU, void** buffer, void** deviceMemoryVulkan, uint64_t bufferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

    MTL::Buffer** buffer_ref = (MTL::Buffer**)buffer;
    buffer_ref[0] = vkGPU->device->newBuffer(bufferSize, MTL::ResourceStorageModePrivate);

	return VKFFT_SUCCESS;
}


VkFFTResult transferDataToCPU(VkGPU* vkGPU, void* cpu_arr, void* output_buffer, uint64_t transferSize) {
	//a function that transfers data from the GPU to the CPU using staging buffer, because the GPU memory is not host-coherent
	VkFFTResult resFFT = VKFFT_SUCCESS;

	MTL::Buffer* stagingBuffer = vkGPU->device->newBuffer(transferSize, MTL::ResourceStorageModeShared);
	MTL::CommandBuffer* copyCommandBuffer = vkGPU->queue->commandBuffer();
	if (copyCommandBuffer == 0) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	MTL::BlitCommandEncoder* blitCommandEncoder = copyCommandBuffer->blitCommandEncoder();
	if (blitCommandEncoder == 0) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	MTL::Buffer* buffer = ((MTL::Buffer**)output_buffer)[0];
	blitCommandEncoder->copyFromBuffer((MTL::Buffer*)buffer, 0, (MTL::Buffer*)stagingBuffer, 0, transferSize);
	blitCommandEncoder->endEncoding();
	copyCommandBuffer->commit();
	copyCommandBuffer->waitUntilCompleted();
	blitCommandEncoder->release();
	copyCommandBuffer->release();
	memcpy(cpu_arr, stagingBuffer->contents(), transferSize);
	stagingBuffer->release();

	return resFFT;
}


VkFFTResult transferDataFromCPU(VkGPU* vkGPU, void* cpu_arr, void* input_buffer, uint64_t transferSize) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	MTL::Buffer* stagingBuffer = vkGPU->device->newBuffer(cpu_arr, transferSize, MTL::ResourceStorageModeShared);
	MTL::CommandBuffer* copyCommandBuffer = vkGPU->queue->commandBuffer();
	if (copyCommandBuffer == 0) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	MTL::BlitCommandEncoder* blitCommandEncoder = copyCommandBuffer->blitCommandEncoder();
	if (blitCommandEncoder == 0) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	MTL::Buffer* buffer = ((MTL::Buffer**)input_buffer)[0];
	blitCommandEncoder->copyFromBuffer((MTL::Buffer*)stagingBuffer, 0, (MTL::Buffer*)buffer, 0, transferSize);
	blitCommandEncoder->endEncoding();
	copyCommandBuffer->commit();
	copyCommandBuffer->waitUntilCompleted();
	blitCommandEncoder->release();
	copyCommandBuffer->release();
	stagingBuffer->release();

	return resFFT;
}


VkFFTResult devices_list() {
	//this function creates an instance and prints the list of available devices

	NS::Array* devices = MTL::CopyAllDevices();
	for (uint64_t i = 0; i < devices->count(); i++) {
		MTL::Device* loc_device = (MTL::Device*)devices->object(i);
		printf("Device id: %" PRIu64 " name: %s\n", i, loc_device->name()->cString(NS::UTF8StringEncoding));
	}

	return VKFFT_SUCCESS;
}


VkFFTResult performVulkanFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, int inverse, uint64_t num_iter) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	MTL::CommandBuffer* commandBuffer = vkGPU->queue->commandBuffer();
	if (commandBuffer == 0) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	launchParams->commandBuffer = commandBuffer;
	MTL::ComputeCommandEncoder* commandEncoder = commandBuffer->computeCommandEncoder();
	if (commandEncoder == 0) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	launchParams->commandEncoder = commandEncoder;
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, inverse, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	commandEncoder->endEncoding();

	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	commandBuffer->commit();
	commandBuffer->waitUntilCompleted();
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;

	commandEncoder->release();
	commandBuffer->release();

	return resFFT;
}


VkFFTResult performVulkanFFTiFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, uint64_t num_iter, double* time_result) {
	VkFFTResult resFFT = VKFFT_SUCCESS;

	MTL::CommandBuffer* commandBuffer = vkGPU->queue->commandBuffer();
	if (commandBuffer == 0) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	launchParams->commandBuffer = commandBuffer;
	MTL::ComputeCommandEncoder* commandEncoder = commandBuffer->computeCommandEncoder();
	if (commandEncoder == 0) return VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_LIST;
	launchParams->commandEncoder = commandEncoder;
	for (uint64_t i = 0; i < num_iter; i++) {
		resFFT = VkFFTAppend(app, -1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
		resFFT = VkFFTAppend(app, 1, launchParams);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	commandEncoder->endEncoding();

	std::chrono::steady_clock::time_point timeSubmit = std::chrono::steady_clock::now();
	commandBuffer->commit();
	commandBuffer->waitUntilCompleted();
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	double totTime = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSubmit).count() * 0.001;
	time_result[0] = totTime / num_iter;
	commandEncoder->release();
	commandBuffer->release();

	return resFFT;
}






////////////////////////////////////////////////////////////
// Backend specific functions used in VkFFT_testSuite.cpp //
////////////////////////////////////////////////////////////

VkFFTResult backendInitDevice(VkGPU* vkGPU, uint64_t sample_id){
    NS::Array* devices = MTL::CopyAllDevices();
    MTL::Device* device = (MTL::Device*)devices->object(vkGPU->device_id);
    vkGPU->device = device;
    MTL::CommandQueue* queue = device->newCommandQueue();
    vkGPU->queue = queue;
	
	return VKFFT_SUCCESS;
};	


VkFFTResult backendFreeDevice(VkGPU* vkGPU);
    vkGPU->queue->release();
    vkGPU->device->release();
    devices->release();

	return VKFFT_SUCCESS;
};






#endif //VKFFT_TEST_SUITE_METAL_H