//general parts
#include <stdio.h>
#include <vector>
#include <memory>
#include <string.h>
#include <chrono>
#include <thread>
#include <iostream>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#if(VKFFT_BACKEND==0)
#include "vulkan/vulkan.h"
#include "glslang/Include/glslang_c_interface.h"
#elif(VKFFT_BACKEND==1)
#include <cuda.h>
#include <cuda_runtime.h>
#include <nvrtc.h>
#include <cuda_runtime_api.h>
#include <cuComplex.h>
#elif(VKFFT_BACKEND==2)
#ifndef __HIP_PLATFORM_HCC__
#define __HIP_PLATFORM_HCC__
#endif
#include <hip/hip_runtime.h>
#include <hip/hiprtc.h>
#include <hip/hip_runtime_api.h>
#include <hip/hip_complex.h>
#elif(VKFFT_BACKEND==3)
#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif 
#elif(VKFFT_BACKEND==4)
#include <ze_api.h>
#elif(VKFFT_BACKEND==5)
#include "Foundation/Foundation.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "Metal/Metal.hpp"
#endif
#include "vkFFT.h"
#include "utils_VkFFT.h"

VkFFTResult sample_50_convolution_VkFFT_single_1d_matrix(VkGPU* vkGPU, uint64_t file_output, FILE* output, uint64_t isCompilerInitialized)
{
	VkFFTResult resFFT = VKFFT_SUCCESS;
#if(VKFFT_BACKEND==0)
	VkResult res = VK_SUCCESS;
#elif(VKFFT_BACKEND==1)
	cudaError_t res = cudaSuccess;
#elif(VKFFT_BACKEND==2)
	hipError_t res = hipSuccess;
#elif(VKFFT_BACKEND==3)
	cl_int res = CL_SUCCESS;
#elif(VKFFT_BACKEND==4)
	ze_result_t res = ZE_RESULT_SUCCESS;
#elif(VKFFT_BACKEND==5)
#endif
	if (file_output)
		fprintf(output, "50 - VkFFT convolution example with identitiy kernel\n");
	printf("50 - VkFFT convolution example with identitiy kernel\n");

	int useSeparateComplexComponents = 0;
	//Configuration + FFT application.
	VkFFTConfiguration configuration = {};
	VkFFTConfiguration convolution_configuration = {};
	VkFFTApplication app_convolution = {};
	VkFFTApplication app_kernel = {};
	//Convolution sample code
	//Setting up FFT configuration. FFT is performed in-place with no performance loss. 
	
	configuration.FFTdim = 1; //FFT dimension, 1D, 2D or 3D (default 1).
	configuration.size[0] = 1024 * 1024 * 8; //Multidimensional FFT dimensions sizes (default 1). For best performance (and stability), order dimensions in descendant size order as: x>y>z. 
	configuration.size[1] = 1;
	configuration.size[2] = 1;

	configuration.kernelConvolution = true; //specify if this plan is used to create kernel for convolution
	configuration.coordinateFeatures = 9; //Specify dimensionality of the input feature vector (default 1). Each component is stored not as a vector, but as a separate system and padded on it's own according to other options (i.e. for x*y system of 3-vector, first x*y elements correspond to the first dimension, then goes x*y for the second, etc).
	//coordinateFeatures number is an important constant for convolution. If we perform 1x1 convolution, it is equal to number of features, but matrixConvolution should be equal to 1. For matrix convolution, it must be equal to matrixConvolution parameter. If we perform 2x2 convolution, it is equal to 3 for symmetric kernel (stored as xx, xy, yy) and 4 for nonsymmetric (stored as xx, xy, yx, yy). Similarly, 6 (stored as xx, xy, xz, yy, yz, zz) and 9 (stored as xx, xy, xz, yx, yy, yz, zx, zy, zz) for 3x3 convolutions. 
	configuration.normalize = 1;//normalize iFFT
	configuration.bufferSeparateComplexComponents = useSeparateComplexComponents;
	configuration.bufferNum = (configuration.bufferSeparateComplexComponents) ? 2 : 1;
	//After this, configuration file contains pointers to Vulkan objects needed to work with the GPU: VkDevice* device - created device, [uint64_t *bufferSize, VkBuffer *buffer, VkDeviceMemory* bufferDeviceMemory] - allocated GPU memory FFT is performed on. [uint64_t *kernelSize, VkBuffer *kernel, VkDeviceMemory* kernelDeviceMemory] - allocated GPU memory, where kernel for convolution is stored.
#if(VKFFT_BACKEND==5)
	configuration.device = vkGPU->device;
#else
	configuration.device = &vkGPU->device;
#endif
#if(VKFFT_BACKEND==0)
	configuration.queue = &vkGPU->queue; //to allocate memory for LUT, we have to pass a queue, vkGPU->fence, commandPool and physicalDevice pointers 
	configuration.fence = &vkGPU->fence;
	configuration.commandPool = &vkGPU->commandPool;
	configuration.physicalDevice = &vkGPU->physicalDevice;
	configuration.isCompilerInitialized = isCompilerInitialized;//compiler can be initialized before VkFFT plan creation. if not, VkFFT will create and destroy one after initialization
#elif(VKFFT_BACKEND==3)
	configuration.context = &vkGPU->context;
#elif(VKFFT_BACKEND==4)
	configuration.context = &vkGPU->context;
	configuration.commandQueue = &vkGPU->commandQueue;
	configuration.commandQueueID = vkGPU->commandQueueID;
#elif(VKFFT_BACKEND==5)
	configuration.queue = vkGPU->queue;
#endif
	//In this example, we perform a convolution for a real vectorfield (3vector) with a symmetric kernel (6 values). We use configuration to initialize convolution kernel first from real data, then we create convolution_configuration for convolution. The buffer object from configuration is passed to convolution_configuration as kernel object.
	//1. Kernel forward FFT.
	uint64_t kernelSize[2];
	uint64_t totalKernelSize = 0;

	int offsetKernelR = sizeof(float) * 1; //in bytes, just for testing
	int offsetKernelI = sizeof(float) * 8;

	if (configuration.bufferSeparateComplexComponents){
		kernelSize[0] = ((uint64_t)configuration.coordinateFeatures) * sizeof(float) * (configuration.size[0]) * configuration.size[1] * configuration.size[2] + offsetKernelR;
		kernelSize[1] = ((uint64_t)configuration.coordinateFeatures) * sizeof(float) * (configuration.size[0]) * configuration.size[1] * configuration.size[2] + offsetKernelI;
		totalKernelSize = kernelSize[0] + kernelSize[1];
		configuration.specifyOffsetsAtLaunch = 1;
	}
	else {
		kernelSize[0] = ((uint64_t)configuration.coordinateFeatures) * sizeof(float) * 2 * (configuration.size[0]) * configuration.size[1] * configuration.size[2];
		totalKernelSize = kernelSize[0];
	}
#if(VKFFT_BACKEND==0)
	VkBuffer kernel[2];
	VkDeviceMemory kernelDeviceMemory[2];
#elif(VKFFT_BACKEND==1)
	void* kernel[2];
#elif(VKFFT_BACKEND==2)
	void* kernel[2];
#elif(VKFFT_BACKEND==3)
	cl_mem kernel[2];
#elif(VKFFT_BACKEND==4)
	void* kernel[2];
#elif(VKFFT_BACKEND==5)
	MTL::Buffer* kernel[2];
#endif
	if (configuration.bufferSeparateComplexComponents) {
#if(VKFFT_BACKEND==0)
		allocateMemoryGPU(vkGPU, (void**)&kernel[0], (void**)&kernelDeviceMemory[0], kernelSize[0]);
		allocateMemoryGPU(vkGPU, (void**)&kernel[1], (void**)&kernelDeviceMemory[1], kernelSize[1]);
#else
		allocateMemoryGPU(vkGPU, (void**)&kernel[0], 0, kernelSize[0]);
		allocateMemoryGPU(vkGPU, (void**)&kernel[1], 0, kernelSize[1]);
#endif
	}
	else {
#if(VKFFT_BACKEND==0)
		allocateMemoryGPU(vkGPU, (void**)&kernel[0], (void**)&kernelDeviceMemory[0], kernelSize[0]);
#else
		allocateMemoryGPU(vkGPU, (void**)&kernel[0], 0, kernelSize[0]);
#endif
	}
	configuration.buffer = kernel;
	configuration.bufferSize = kernelSize;

	if (file_output)
		fprintf(output, "Total memory needed for kernel: %" PRIu64 " MB\n", totalKernelSize / 1024 / 1024);
	printf("Total memory needed for kernel: %" PRIu64 " MB\n", totalKernelSize / 1024 / 1024);
	//Fill kernel on CPU.
	float* kernel_input = (float*)malloc(totalKernelSize);
	if (!kernel_input) return VKFFT_ERROR_MALLOC_FAILED;

	float* kernel_inputr;
	float* kernel_inputi;

	if (configuration.bufferSeparateComplexComponents) {
		kernel_inputr = (float*)malloc(kernelSize[0]);
		if (!kernel_inputr) return VKFFT_ERROR_MALLOC_FAILED;
		kernel_inputi = (float*)malloc(kernelSize[1]);
		if (!kernel_inputi) return VKFFT_ERROR_MALLOC_FAILED;
	}
	for (uint64_t v = 0; v < configuration.coordinateFeatures; v++) {
		for (uint64_t k = 0; k < configuration.size[2]; k++) {
			for (uint64_t j = 0; j < configuration.size[1]; j++) {

				//for (uint64_t i = 0; i < configuration.size[0]; i++) {
				//	kernel_input[i + j * configuration.size[0] + k * (configuration.size[0] + 2) * configuration.size[1] + v * (configuration.size[0] + 2) * configuration.size[1] * configuration.size[2]] = 1;

				//Below is the test identity kernel for 3x3 nonsymmetric FFT
				for (uint64_t i = 0; i < configuration.size[0]; i++) {
					if ((v == 0) || (v == 4) || (v == 8))

						kernel_input[2 * (i + j * (configuration.size[0]) + k * (configuration.size[0]) * configuration.size[1] + v * (configuration.size[0]) * configuration.size[1] * configuration.size[2])] = 1;

					else
						kernel_input[2 * (i + j * (configuration.size[0]) + k * (configuration.size[0]) * configuration.size[1] + v * (configuration.size[0]) * configuration.size[1] * configuration.size[2])] = 0;
					kernel_input[2 * (i + j * (configuration.size[0]) + k * (configuration.size[0]) * configuration.size[1] + v * (configuration.size[0]) * configuration.size[1] * configuration.size[2]) + 1] = 0;
					
					if (configuration.bufferSeparateComplexComponents) {
						kernel_inputr[offsetKernelR/sizeof(float) + (i + j * (configuration.size[0]) + k * (configuration.size[0]) * configuration.size[1] + v * (configuration.size[0]) * configuration.size[1] * configuration.size[2])] = kernel_input[2 * (i + j * (configuration.size[0]) + k * (configuration.size[0]) * configuration.size[1] + v * (configuration.size[0]) * configuration.size[1] * configuration.size[2])];
						kernel_inputi[offsetKernelI/sizeof(float) + (i + j * (configuration.size[0]) + k * (configuration.size[0]) * configuration.size[1] + v * (configuration.size[0]) * configuration.size[1] * configuration.size[2])] = kernel_input[2 * (i + j * (configuration.size[0]) + k * (configuration.size[0]) * configuration.size[1] + v * (configuration.size[0]) * configuration.size[1] * configuration.size[2])+1];
					}
				}
			}
		}
	}
	//Sample buffer transfer tool. Uses staging buffer (if needed) of the same size as destination buffer, which can be reduced if transfer is done sequentially in small buffers.
	
	if (configuration.bufferSeparateComplexComponents) {
		resFFT = transferDataFromCPU(vkGPU, kernel_inputr, &kernel[0], kernelSize[0]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
		resFFT = transferDataFromCPU(vkGPU, kernel_inputi, &kernel[1], kernelSize[1]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	else {
		resFFT = transferDataFromCPU(vkGPU, kernel_input, &kernel[0], kernelSize[0]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	//Initialize application responsible for the kernel. This function loads shaders, creates pipeline and configures FFT based on configuration file. No buffer allocations inside VkFFT library.  
	resFFT = initializeVkFFT(&app_kernel, configuration);
	if (resFFT != VKFFT_SUCCESS) return resFFT;
	//Sample forward FFT command buffer allocation + execution performed on kernel. Second number determines how many times perform application in one submit. FFT can also be appended to user defined command buffers.

	//Uncomment the line below if you want to perform kernel FFT. In this sample we use predefined identitiy kernel.
	/* VkFFTLaunchParams launchParams = {};
	if (configuration.bufferSeparateComplexComponents) {
		launchParams.bufferOffset = offsetR;
		launchParams.bufferOffsetImaginary = offsetI;
	}
	resFFT = performVulkanFFT(vkGPU, &app, &launchParams, -1, 1);
	if (resFFT != VKFFT_SUCCESS) return resFFT;*/

	//The kernel has been trasnformed.


	//2. Buffer convolution with transformed kernel.
	convolution_configuration = {};
	convolution_configuration.FFTdim = configuration.FFTdim;
	convolution_configuration.size[0] = configuration.size[0]; 
	convolution_configuration.size[1] = configuration.size[1];
	convolution_configuration.size[2] = configuration.size[2];
	convolution_configuration.normalize = configuration.normalize;
	convolution_configuration.performConvolution = true;
	convolution_configuration.symmetricKernel = false;//Specify if convolution kernel is symmetric. In this case we only pass upper triangle part of it in the form of: (xx, xy, yy) for 2d and (xx, xy, xz, yy, yz, zz) for 3d.
	convolution_configuration.matrixConvolution = 3;//we do matrix convolution, so kernel is 9 numbers (3x3), but vector dimension is 3
	convolution_configuration.coordinateFeatures = 3;//equal to matrixConvolution size
	convolution_configuration.keepShaderCode = 1;
#if(VKFFT_BACKEND==3)
	//convolution_configuration.useLUT = 1; //bug? OpenCL needs LUT for correct results on this sample on Nvidia GPUs.
#endif
	if (configuration.bufferSeparateComplexComponents) {
		convolution_configuration.kernelSeparateComplexComponents = 1;
		convolution_configuration.kernelNum = 2;
		convolution_configuration.specifyOffsetsAtLaunch = 1;
		convolution_configuration.inputBufferSeparateComplexComponents = 1;
		convolution_configuration.bufferSeparateComplexComponents = 1;
	}
	convolution_configuration.isInputFormatted = 1;
	convolution_configuration.inputBufferNum = (convolution_configuration.inputBufferSeparateComplexComponents) ? 2 : 1;
	convolution_configuration.bufferNum = (convolution_configuration.bufferSeparateComplexComponents) ? 2 : 1;
#if(VKFFT_BACKEND==5)
	convolution_configuration.device = vkGPU->device;
#else
	convolution_configuration.device = &vkGPU->device;
#endif
#if(VKFFT_BACKEND==0)
	convolution_configuration.queue = &vkGPU->queue; //to allocate memory for LUT, we have to pass a queue, vkGPU->fence, commandPool and physicalDevice pointers 
	convolution_configuration.fence = &vkGPU->fence;
	convolution_configuration.commandPool = &vkGPU->commandPool;
	convolution_configuration.physicalDevice = &vkGPU->physicalDevice;
	convolution_configuration.isCompilerInitialized = isCompilerInitialized;//compiler can be initialized before VkFFT plan creation. if not, VkFFT will create and destroy one after initialization
#elif(VKFFT_BACKEND==3)
	convolution_configuration.context = &vkGPU->context;
#elif(VKFFT_BACKEND==4)
	convolution_configuration.context = &vkGPU->context;
	convolution_configuration.commandQueue = &vkGPU->commandQueue;
	convolution_configuration.commandQueueID = vkGPU->commandQueueID;
#elif(VKFFT_BACKEND==5)
	convolution_configuration.queue = vkGPU->queue;
#endif

	//Allocate separate buffer for the input data.
	uint64_t inputBufferSize[2];
	int offsetInputBufferR = sizeof(float) * 2; //in bytes, just for testing
	int offsetInputBufferI = sizeof(float) * 3;
	if(convolution_configuration.inputBufferSeparateComplexComponents){
		inputBufferSize[0] = ((uint64_t)convolution_configuration.coordinateFeatures) * sizeof(float) * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2] + offsetInputBufferR;
		inputBufferSize[1] = ((uint64_t)convolution_configuration.coordinateFeatures) * sizeof(float) * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2] + offsetInputBufferI;
	}else{
		inputBufferSize[0] = ((uint64_t)convolution_configuration.coordinateFeatures) * sizeof(float) * 2 * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2];;		
		inputBufferSize[1] = 0;
	}
	uint64_t totalInputBufferSize = inputBufferSize[0] + inputBufferSize[1];
	
	uint64_t bufferSize[2];
	int offsetBufferR = sizeof(float) * 7; //in bytes, just for testing
	int offsetBufferI = sizeof(float) * 5;
	if(convolution_configuration.bufferSeparateComplexComponents){
		bufferSize[0] = ((uint64_t)convolution_configuration.coordinateFeatures) * sizeof(float) * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2] + offsetBufferR;
		bufferSize[1] = ((uint64_t)convolution_configuration.coordinateFeatures) * sizeof(float) * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2] + offsetBufferI;
	}else{
		bufferSize[0] = ((uint64_t)convolution_configuration.coordinateFeatures) * sizeof(float) * 2 * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2];;		
		bufferSize[1] = 0;
	}
	uint64_t totalBufferSize = bufferSize[0] + bufferSize[1];
#if(VKFFT_BACKEND==0)
	VkBuffer inputBuffer[2];
	VkDeviceMemory inputBufferDeviceMemory[2];

	VkBuffer buffer[2];
	VkDeviceMemory bufferDeviceMemory[2];
#elif(VKFFT_BACKEND==1)
	void* inputBuffer[2];

	void* buffer[2];
#elif(VKFFT_BACKEND==2)
	void* inputBuffer[2];

	void* buffer[2];
#elif(VKFFT_BACKEND==3)
	cl_mem inputBuffer[2];

	cl_mem buffer[2];
#elif(VKFFT_BACKEND==4)
	void* inputBuffer[2];

	void* buffer[2];
#elif(VKFFT_BACKEND==5)
	MTL::Buffer* inputBuffer[2];

	MTL::Buffer* buffer[2];
#endif
	if (convolution_configuration.inputBufferSeparateComplexComponents) {
#if(VKFFT_BACKEND==0)
		allocateMemoryGPU(vkGPU, (void**)&inputBuffer[0], (void**)&inputBufferDeviceMemory[0], inputBufferSize[0]);
		allocateMemoryGPU(vkGPU, (void**)&inputBuffer[1], (void**)&inputBufferDeviceMemory[1], inputBufferSize[1]);
#else
		allocateMemoryGPU(vkGPU, (void**)&inputBuffer[0], 0, inputBufferSize[0]);
		allocateMemoryGPU(vkGPU, (void**)&inputBuffer[1], 0, inputBufferSize[1]);
#endif
	}
	else {
#if(VKFFT_BACKEND==0)
		allocateMemoryGPU(vkGPU, (void**)&inputBuffer[0], (void**)&inputBufferDeviceMemory[0], inputBufferSize[0]);
#else
		allocateMemoryGPU(vkGPU, (void**)&inputBuffer[0], 0, inputBufferSize[0]);
#endif
	}
	
	if (convolution_configuration.bufferSeparateComplexComponents) {
#if(VKFFT_BACKEND==0)
		allocateMemoryGPU(vkGPU, (void**)&buffer[0], (void**)&bufferDeviceMemory[0], bufferSize[0]);
		allocateMemoryGPU(vkGPU, (void**)&buffer[1], (void**)&bufferDeviceMemory[1], bufferSize[1]);
#else
		allocateMemoryGPU(vkGPU, (void**)&buffer[0], 0, bufferSize[0]);
		allocateMemoryGPU(vkGPU, (void**)&buffer[1], 0, bufferSize[1]);
#endif
	}
	else {
#if(VKFFT_BACKEND==0)
		allocateMemoryGPU(vkGPU, (void**)&buffer[0], (void**)&bufferDeviceMemory[0], bufferSize[0]);
#else
		allocateMemoryGPU(vkGPU, (void**)&buffer[0], 0, bufferSize[0]);
#endif
	}

	convolution_configuration.inputBuffer = inputBuffer;
	convolution_configuration.buffer = buffer;
	convolution_configuration.kernel = kernel;

	convolution_configuration.inputBufferSize = inputBufferSize;
	convolution_configuration.bufferSize = bufferSize;
	convolution_configuration.kernelSize = kernelSize;

	if (file_output)
		fprintf(output, "Total memory needed for buffer: %" PRIu64 " MB\n", totalBufferSize / 1024 / 1024);
	printf("Total memory needed for buffer: %" PRIu64 " MB\n", totalBufferSize / 1024 / 1024);
	//Fill data on CPU. It is best to perform all operations on GPU after initial upload.
	float* buffer_input = (float*)malloc(totalBufferSize);
	if (!buffer_input) return VKFFT_ERROR_MALLOC_FAILED;
	float* buffer_inputr;
	float* buffer_inputi;

	if (convolution_configuration.inputBufferSeparateComplexComponents) {
		buffer_inputr = (float*)malloc(inputBufferSize[0]);
		if (!buffer_inputr) return VKFFT_ERROR_MALLOC_FAILED;
		buffer_inputi = (float*)malloc(inputBufferSize[1]);
		if (!buffer_inputi) return VKFFT_ERROR_MALLOC_FAILED;
	}
	for (uint64_t v = 0; v < convolution_configuration.coordinateFeatures; v++) {
		for (uint64_t k = 0; k < convolution_configuration.size[2]; k++) {
			for (uint64_t j = 0; j < convolution_configuration.size[1]; j++) {
				for (uint64_t i = 0; i < convolution_configuration.size[0]; i++) {
					buffer_input[2 * (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])] = (float)(i % 8 - 3.5);
					buffer_input[2 * (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2]) + 1] = (float)(i % 4 - 1.5);
					if (convolution_configuration.inputBufferSeparateComplexComponents) {
						buffer_inputr[offsetInputBufferR/sizeof(float) + (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])] = (float)(i % 8 - 3.5);
						buffer_inputi[offsetInputBufferI/sizeof(float) + (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])] = (float)(i % 4 - 1.5);
					}
				}
			}
		}
	}
	//Transfer data to GPU using staging buffer.
	if (convolution_configuration.inputBufferSeparateComplexComponents) {
		resFFT = transferDataFromCPU(vkGPU, buffer_inputr, &inputBuffer[0], inputBufferSize[0]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
		resFFT = transferDataFromCPU(vkGPU, buffer_inputi, &inputBuffer[1], inputBufferSize[1]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	else {
		resFFT = transferDataFromCPU(vkGPU, buffer_input, &inputBuffer[0], inputBufferSize[0]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	
	//Initialize application responsible for the convolution.
	resFFT = initializeVkFFT(&app_convolution, convolution_configuration);
	if (resFFT != VKFFT_SUCCESS) return resFFT;
	//Sample forward FFT command buffer allocation + execution performed on kernel. FFT can also be appended to user defined command buffers.
	VkFFTLaunchParams launchParams = {};
	launchParams.inputBufferOffset = offsetInputBufferR;
	launchParams.inputBufferOffsetImaginary = offsetInputBufferI;
	launchParams.bufferOffset = offsetBufferR;
	launchParams.bufferOffsetImaginary = offsetBufferI;
	launchParams.kernelOffset = offsetKernelR;
	launchParams.kernelOffsetImaginary = offsetKernelI;
	resFFT = performVulkanFFT(vkGPU, &app_convolution, &launchParams, -1, 1);
	if (resFFT != VKFFT_SUCCESS) return resFFT;
	//The kernel has been trasnformed.

	float* buffer_output = (float*)malloc(totalBufferSize);
	float* buffer_outputr;
	float* buffer_outputi;
	if (!buffer_output) return VKFFT_ERROR_MALLOC_FAILED;
	if (convolution_configuration.bufferSeparateComplexComponents) {
		buffer_outputr = (float*)malloc(bufferSize[0]);
		buffer_outputi = (float*)malloc(bufferSize[1]);
	}
	//Transfer data from GPU using staging buffer.
	if (convolution_configuration.bufferSeparateComplexComponents) {
		resFFT = transferDataToCPU(vkGPU, buffer_outputr, &buffer[0], bufferSize[0]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
		resFFT = transferDataToCPU(vkGPU, buffer_outputi, &buffer[1], bufferSize[1]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	else {
		resFFT = transferDataToCPU(vkGPU, buffer_output, &buffer[0], bufferSize[0]);
		if (resFFT != VKFFT_SUCCESS) return resFFT;
	}
	//Print data, if needed.
	for (uint64_t v = 0; v < convolution_configuration.coordinateFeatures; v++) {
		if (file_output)
			fprintf(output, "\ncoordinate: %" PRIu64 "\n\n", v);
		printf("\ncoordinate: %" PRIu64 "\n\n", v);
		for (uint64_t k = 0; k < convolution_configuration.size[2]; k++) {
			for (uint64_t j = 0; j < convolution_configuration.size[1]; j++) {
				for (uint64_t i = 0; i < convolution_configuration.size[0]; i++) {
					if (convolution_configuration.bufferSeparateComplexComponents) {
						if (file_output)
							fprintf(output, "%.6f %.6f ", buffer_outputr[offsetBufferR/sizeof(float) + (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])], buffer_outputi[offsetBufferI/sizeof(float) + (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])]);
						printf("%.6f %.6f ", buffer_outputr[offsetBufferR/sizeof(float) + (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])], buffer_outputi[offsetBufferI/sizeof(float) + (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])]);
					}
					else {
						if (file_output)
							fprintf(output, "%.6f %.6f ", buffer_output[2 * (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])], buffer_output[2 * (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2]) + 1]);
						printf("%.6f %.6f ", buffer_output[2 * (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2])], buffer_output[2 * (i + j * convolution_configuration.size[0] + k * (convolution_configuration.size[0]) * convolution_configuration.size[1] + v * (convolution_configuration.size[0]) * convolution_configuration.size[1] * convolution_configuration.size[2]) + 1]);
					}
				}
				std::cout << "\n";
			}
		}
	}
	free(kernel_input);
	free(buffer_input);
	free(buffer_output);
	if (convolution_configuration.kernelSeparateComplexComponents) {
		free(kernel_inputr);
		free(kernel_inputi);
	}
	if (convolution_configuration.inputBufferSeparateComplexComponents) {
		free(buffer_inputr);
		free(buffer_inputi);
	}
	if (convolution_configuration.bufferSeparateComplexComponents) {
		free(buffer_outputr);
		free(buffer_outputi);
	}
#if(VKFFT_BACKEND==0)
	vkDestroyBuffer(vkGPU->device, buffer[0], NULL);
	vkFreeMemory(vkGPU->device, bufferDeviceMemory[0], NULL);
	vkDestroyBuffer(vkGPU->device, kernel[0], NULL);
	vkFreeMemory(vkGPU->device, kernelDeviceMemory[0], NULL);
#elif(VKFFT_BACKEND==1)
	cudaFree(buffer[0]);
	cudaFree(kernel[0]);
#elif(VKFFT_BACKEND==2)
	hipFree(buffer[0]);
	hipFree(kernel[0]);
#elif(VKFFT_BACKEND==3)
	clReleaseMemObject(buffer[0]);
	clReleaseMemObject(kernel[0]);
#elif(VKFFT_BACKEND==4)
	zeMemFree(vkGPU->context, buffer[0]);
	zeMemFree(vkGPU->context, kernel[0]);
#elif(VKFFT_BACKEND==5)
	buffer[0]->release();
	kernel[0]->release();
#endif	
	if (convolution_configuration.kernelSeparateComplexComponents) {
#if(VKFFT_BACKEND==0)
		vkDestroyBuffer(vkGPU->device, kernel[1], NULL);
		vkFreeMemory(vkGPU->device, kernelDeviceMemory[1], NULL);
#elif(VKFFT_BACKEND==1)
		cudaFree(kernel[1]);
#elif(VKFFT_BACKEND==2)
		hipFree(kernel);
#elif(VKFFT_BACKEND==3)
		clReleaseMemObject(kernel[1]);
#elif(VKFFT_BACKEND==4)
		zeMemFree(vkGPU->context, kernel[1]);
#elif(VKFFT_BACKEND==5)
		kernel[1]->release();
#endif	
	}
	if (convolution_configuration.bufferSeparateComplexComponents) {
#if(VKFFT_BACKEND==0)
		vkDestroyBuffer(vkGPU->device, buffer[1], NULL);
		vkFreeMemory(vkGPU->device, bufferDeviceMemory[1], NULL);
#elif(VKFFT_BACKEND==1)
		cudaFree(buffer[1]);
#elif(VKFFT_BACKEND==2)
		hipFree(buffer[1]);
#elif(VKFFT_BACKEND==3)
		clReleaseMemObject(buffer[1]);
#elif(VKFFT_BACKEND==4)
		zeMemFree(vkGPU->context, buffer[1]);
#elif(VKFFT_BACKEND==5)
		buffer[1]->release();
#endif	
	}
	if (convolution_configuration.inputBufferSeparateComplexComponents) {
#if(VKFFT_BACKEND==0)
		vkDestroyBuffer(vkGPU->device, inputBuffer[1], NULL);
		vkFreeMemory(vkGPU->device, inputBufferDeviceMemory[1], NULL);
#elif(VKFFT_BACKEND==1)
		cudaFree(inputBuffer[1]);
#elif(VKFFT_BACKEND==2)
		hipFree(inputBuffer[1]);
#elif(VKFFT_BACKEND==3)
		clReleaseMemObject(inputBuffer[1]);
#elif(VKFFT_BACKEND==4)
		zeMemFree(vkGPU->context, inputBuffer[1]);
#elif(VKFFT_BACKEND==5)
		inputBuffer[1]->release();
#endif	
	}
	deleteVkFFT(&app_kernel);
	deleteVkFFT(&app_convolution);
	return resFFT;
}
