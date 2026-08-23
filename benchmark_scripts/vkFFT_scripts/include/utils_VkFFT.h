#ifndef VKFFT_UTILS_H
#define VKFFT_UTILS_H
#include "vkFFT.h"
#include <vector>

typedef struct {
	uint64_t X;
	uint64_t Y;
	uint64_t Z;
	uint64_t P;
	uint64_t B;
	uint64_t N;
	uint64_t R2C;
	uint64_t DCT;
	uint64_t saveApplicationToString;
	uint64_t loadApplicationFromString;
} VkFFTUserSystemParameters;//an example structure used to pass user-defined system for benchmarking


#if(VKFFT_BACKEND==0)
#include "test_suite_vulkan.h"
#elif(VKFFT_BACKEND==1)
#include "test_suite_cuda.h"
#elif(VKFFT_BACKEND==2)
#include "test_suite_opencl.h"
#elif(VKFFT_BACKEND==3)
#include "test_suite_hip.h"
#elif(VKFFT_BACKEND==4)
#include "test_suite_zero.h"
#elif(VKFFT_BACKEND==5)
#include "test_suite_metal.h"	
#endif


VkFFTResult allocateMemoryGPU(VkGPU* vkGPU, void** buffer, void** deviceMemoryVulkan, uint64_t bufferSize);
VkFFTResult transferDataToCPU(VkGPU* vkGPU, void* cpu_arr, void* output_buffer, uint64_t bufferSize);
VkFFTResult transferDataFromCPU(VkGPU* vkGPU, void* cpu_arr, void* input_buffer, uint64_t bufferSize);
VkFFTResult devices_list();
VkFFTResult performVulkanFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, int inverse, uint64_t num_iter);
VkFFTResult performVulkanFFTiFFT(VkGPU* vkGPU, VkFFTApplication* app, VkFFTLaunchParams* launchParams, uint64_t num_iter, double* time_result);


#endif //VKFFT_UTILS_H
