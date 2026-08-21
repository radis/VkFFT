

#if(VKFFT_BACKEND==0)
#include "backend/vulkan/backend_structs_vulkan.h"
#elif(VKFFT_BACKEND==1)
#include "backend/cuda/backend_structs_cuda.h"
#elif(VKFFT_BACKEND==2)
#include "backend/hip/backend_structs_hip.h"
#elif(VKFFT_BACKEND==3)
#include "backend/opencl/backend_structs_opencl.h"
#elif(VKFFT_BACKEND==4)
#include "backend/zero/backend_structs_zero.h"
#elif(VKFFT_BACKEND==5)
#include "backend/metal/backend_structs_metal.h"
#endif

#include "backend_structs_generic.h"