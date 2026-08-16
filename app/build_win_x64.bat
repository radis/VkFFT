
set "VK_ARM64X=-x64"

cl /LD /MD /EHsc /DVK_API_VERSION=11^
 /I..\..\..\VkFFT-radis\vkFFT^
 /I"%VK_SDK_PATH%\Include"^
 /I"%VK_SDK_PATH%\Include\glslang\Include"^
 vkfft_vulkan.cpp^
 /link /LIBPATH:"%VK_SDK_PATH%\Lib%VK_ARM64X%"^
 vulkan-1.lib^
 GenericCodeGen.lib^
 glslang.lib^
 MachineIndependent.lib^
 SPIRV.lib^
 SPIRV-Tools.lib^
 SPIRV-Tools-opt.lib

copy vkfft_vulkan.dll ..\..\radis\radis\gpu\vulkan\bin\vkfft_vulkan.dll