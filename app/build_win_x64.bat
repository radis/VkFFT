set "VK_DIR=C:\VulkanSDK\1.4.357.0"
set "VK_ARM64X=-x64"

cl /LD /MD /EHsc /DVK_API_VERSION=11^
 /I..\..\..\VkFFT-radis\vkFFT^
 /I"%VK_DIR%\Include"^
 /I"%VK_DIR%\Include\glslang\Include"^
 vkfft_vulkan.cpp^
 /link /LIBPATH:"%VK_DIR%\Lib%VK_ARM64X%"^
 vulkan-1.lib^
 GenericCodeGen.lib^
 glslang.lib^
 MachineIndependent.lib^
 SPIRV.lib^
 SPIRV-Tools.lib^
 SPIRV-Tools-opt.lib

copy vkfft_vulkan.dll ..\..\radis\radis\gpu\vulkan\bin\vkfft_vulkan.dll