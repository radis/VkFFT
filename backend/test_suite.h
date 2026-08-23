
#ifndef VKFFT_TEST_SUITE_H
#def VKFFT_TEST_SUITE_H

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

#endif //VKFFT_TEST_SUITE_H