

#if(VKFFT_BACKEND==0)
#include "backend/vulkan/backend_app_management_vulkan.h"
#elif(VKFFT_BACKEND==1)
#include "backend/cuda/backend_app_management_cuda.h"
#elif(VKFFT_BACKEND==2)
#include "backend/hip/backend_app_management_hip.h"
#elif(VKFFT_BACKEND==3)
#include "backend/opencl/backend_app_management_opencl.h"
#elif(VKFFT_BACKEND==4)
#include "backend/zero/backend_app_management_zero.h"
#elif(VKFFT_BACKEND==5)
#include "backend/metal/backend_app_management_metal.h"
#endif

