#include <vector>
#include <memory>
#include <string.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "vkFFT.h"
#include "utils_VkFFT.h"
#include "half.hpp"
#include "user_benchmark_VkFFT.h"
#include "sample_0_benchmark_VkFFT_single.h"
#include "sample_1_benchmark_VkFFT_double.h"
#include "sample_2_benchmark_VkFFT_half.h"
#include "sample_3_benchmark_VkFFT_single_3d.h"
#include "sample_4_benchmark_VkFFT_single_3d_zeropadding.h"
#include "sample_5_benchmark_VkFFT_single_disableReorderFourStep.h"
#include "sample_6_benchmark_VkFFT_single_r2c.h"
#include "sample_7_benchmark_VkFFT_single_Bluestein.h"
#include "sample_8_benchmark_VkFFT_double_Bluestein.h"
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
#include "sample_9_benchmark_VkFFT_quadDoubleDouble.h"
#endif
#include "sample_10_benchmark_VkFFT_single_multipleBuffers.h"
#ifdef USE_FFTW
#include "sample_11_precision_VkFFT_single.h"
#include "sample_12_precision_VkFFT_double.h"
#include "sample_13_precision_VkFFT_half.h"
#include "sample_14_precision_VkFFT_single_nonPow2.h"
#include "sample_15_precision_VkFFT_single_r2c.h"
#include "sample_16_precision_VkFFT_single_dct.h"
#include "sample_17_precision_VkFFT_double_dct.h"
#include "sample_18_precision_VkFFT_double_nonPow2.h"
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
#include "sample_19_precision_VkFFT_quadDoubleDouble_nonPow2.h"
#endif
#endif
#include "sample_50_convolution_VkFFT_single_1d_matrix.h"
#include "sample_51_convolution_VkFFT_single_3d_matrix_zeropadding_r2c.h"
#include "sample_52_convolution_VkFFT_single_2d_batched_r2c.h"
#include "sample_53_convolution_VkFFT_single_2d_Nimages_1kernel.h"

#include "sample_100_benchmark_VkFFT_single_nd_dct.h"
#include "sample_101_benchmark_VkFFT_double_nd_dct.h"
#include "sample_1000_benchmark_VkFFT_single_2_4096.h"
#include "sample_1001_benchmark_VkFFT_double_2_4096.h"
#include "sample_1002_benchmark_VkFFT_half_2_4096.h"
#include "sample_1003_benchmark_VkFFT_single_3d_2_512.h"
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
#include "sample_1004_benchmark_VkFFT_quadDoubleDouble_2_4096.h"
#endif
#ifdef USE_cuFFT
#include "user_benchmark_cuFFT.h"
#include "sample_0_benchmark_cuFFT_single.h"
#include "sample_1_benchmark_cuFFT_double.h"
#include "sample_2_benchmark_cuFFT_half.h"
#include "sample_3_benchmark_cuFFT_single_3d.h"
#include "sample_6_benchmark_cuFFT_single_r2c.h"
#include "sample_7_benchmark_cuFFT_single_Bluestein.h"
#include "sample_8_benchmark_cuFFT_double_Bluestein.h"
#include "sample_1000_benchmark_cuFFT_single_2_4096.h"
#include "sample_1001_benchmark_cuFFT_double_2_4096.h"
#include "sample_1003_benchmark_cuFFT_single_3d_2_512.h"
#endif  
#ifdef USE_rocFFT
#include "user_benchmark_rocFFT.h"
#include "sample_0_benchmark_rocFFT_single.h"
#include "sample_1_benchmark_rocFFT_double.h"
#include "sample_3_benchmark_rocFFT_single_3d.h"
#include "sample_6_benchmark_rocFFT_single_r2c.h"
#include "sample_7_benchmark_rocFFT_single_Bluestein.h"
#include "sample_8_benchmark_rocFFT_double_Bluestein.h"
#include "sample_1000_benchmark_rocFFT_single_2_4096.h"
#include "sample_1001_benchmark_rocFFT_double_2_4096.h"
#include "sample_1003_benchmark_rocFFT_single_3d_2_512.h"
#endif 
#ifdef USE_FFTW
#include "fftw3.h"
#endif

VkFFTResult launchVkFFT(VkGPU* vkGPU, uint64_t sample_id, bool file_output, FILE* output, VkFFTUserSystemParameters* userParams) {
	//Sample Vulkan project GPU initialization.
	VkFFTResult resFFT = VKFFT_SUCCESS;

	resFFT = backendInitDevice(vkGPU, sample_id);
	if (resFFT != VKFFT_SUCCESS){
		return resFFT;
	}

	uint64_t isCompilerInitialized = 1;

    switch (sample_id) {
    case 0:
    {
        resFFT = sample_0_benchmark_VkFFT_single(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 1:
    {
        resFFT = sample_1_benchmark_VkFFT_double(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
	case 2:
    {
        resFFT = sample_2_benchmark_VkFFT_half(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 3:
    {
        resFFT = sample_3_benchmark_VkFFT_single_3d(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 4:
    {
        resFFT = sample_4_benchmark_VkFFT_single_3d_zeropadding(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 5:
    {
        resFFT = sample_5_benchmark_VkFFT_single_disableReorderFourStep(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 6:
    {
        resFFT = sample_6_benchmark_VkFFT_single_r2c(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 7:
    {
        resFFT = sample_7_benchmark_VkFFT_single_Bluestein(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 8:
    {
        resFFT = sample_8_benchmark_VkFFT_double_Bluestein(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
	case 9:
	{
		resFFT = sample_9_benchmark_VkFFT_quadDoubleDouble(vkGPU, file_output, output, isCompilerInitialized);
		break;
	}
#endif
#ifdef VKFFT_USE_MULTIPLE_BUFFER
    case 10:
    {
        resFFT = sample_10_benchmark_VkFFT_single_multipleBuffers(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
#endif
#ifdef USE_FFTW
    case 11:
    {
        resFFT = sample_11_precision_VkFFT_single(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 12:
    {
        resFFT = sample_12_precision_VkFFT_double(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
	case 13:
    {
        resFFT = sample_13_precision_VkFFT_half(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 14:
    {
        resFFT = sample_14_precision_VkFFT_single_nonPow2(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 15:
    {
        resFFT = sample_15_precision_VkFFT_single_r2c(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 16:
    {
        resFFT = sample_16_precision_VkFFT_single_dct(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 17:
    {
        resFFT = sample_17_precision_VkFFT_double_dct(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 18:
    {
        resFFT = sample_18_precision_VkFFT_double_nonPow2(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
	case 19:
    {
        resFFT = sample_19_precision_VkFFT_quadDoubleDouble_nonPow2(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
#endif
#endif
    case 50:
    {
        resFFT = sample_50_convolution_VkFFT_single_1d_matrix(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 51:
    {
        resFFT = sample_51_convolution_VkFFT_single_3d_matrix_zeropadding_r2c(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 52:
    {
        resFFT = sample_52_convolution_VkFFT_single_2d_batched_r2c(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
	case 53:
    {
        resFFT = sample_53_convolution_VkFFT_single_2d_Nimages_1kernel(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 110:
    {
        resFFT = sample_100_benchmark_VkFFT_single_nd_dct(vkGPU, file_output, output, isCompilerInitialized, 1);
        break;
    }
    case 120:
    {
        resFFT = sample_100_benchmark_VkFFT_single_nd_dct(vkGPU, file_output, output, isCompilerInitialized, 2);
        break;
    }
    case 130:
    {
        resFFT = sample_100_benchmark_VkFFT_single_nd_dct(vkGPU, file_output, output, isCompilerInitialized, 3);
        break;
    }
    case 140:
    {
        resFFT = sample_100_benchmark_VkFFT_single_nd_dct(vkGPU, file_output, output, isCompilerInitialized, 4);
        break;
    }
    case 111:
    {
        resFFT = sample_101_benchmark_VkFFT_double_nd_dct(vkGPU, file_output, output, isCompilerInitialized, 1);
        break;
    }
    case 121:
    {
        resFFT = sample_101_benchmark_VkFFT_double_nd_dct(vkGPU, file_output, output, isCompilerInitialized, 2);
        break;
    }
    case 131:
    {
        resFFT = sample_101_benchmark_VkFFT_double_nd_dct(vkGPU, file_output, output, isCompilerInitialized, 3);
        break;
    }
    case 141:
    {
        resFFT = sample_101_benchmark_VkFFT_double_nd_dct(vkGPU, file_output, output, isCompilerInitialized, 4);
        break;
    }
    case 200: case 201: case 202:
    {
        resFFT = user_benchmark_VkFFT(vkGPU, file_output, output, isCompilerInitialized, userParams);
        break;
    }
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
	case 203:
    {
        resFFT = user_benchmark_VkFFT(vkGPU, file_output, output, isCompilerInitialized, userParams);
        break;
    }
#endif
    case 1000:
    {
        resFFT = sample_1000_benchmark_VkFFT_single_2_4096(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
    case 1001:
    {
        resFFT = sample_1001_benchmark_VkFFT_double_2_4096(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
	case 1002:
	{
		resFFT = sample_1002_benchmark_VkFFT_half_2_4096(vkGPU, file_output, output, isCompilerInitialized);
		break;
	}
    case 1003:
    {
        resFFT = sample_1003_benchmark_VkFFT_single_3d_2_512(vkGPU, file_output, output, isCompilerInitialized);
        break;
    }
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128	
	case 1004:
	{
		resFFT = sample_1004_benchmark_VkFFT_quadDoubleDouble_2_4096(vkGPU, file_output, output, isCompilerInitialized);
		break;
	}
#endif
    }

	resFFT = backendFreeDevice(vkGPU);
	if (resFFT != VKFFT_SUCCESS){
		return resFFT;
	}

	return resFFT;
}

bool findFlag(char** start, char** end, const std::string& flag) {
	return (std::find(start, end, flag) != end);
}
char* getFlagValue(char** start, char** end, const std::string& flag)
{
	char** value = std::find(start, end, flag);
	value++;
	if (value != end)
	{
		return *value;
	}
	return 0;
}
int main(int argc, char* argv[])
{
	VkGPU vkGPU = {};
// Moved to backendInitDevice()
// #if(VKFFT_BACKEND==0)
	// vkGPU.enableValidationLayers = 0;
// #endif
	bool file_output = false;
	FILE* output = NULL;
	int sscanf_res = 0;
	if (findFlag(argv, argv + argc, "-h"))
	{
		//print help
		int version = VkFFTGetVersion();
		int version_decomposed[3];
		version_decomposed[0] = version / 10000;
		version_decomposed[1] = (version - version_decomposed[0] * 10000) / 100;
		version_decomposed[2] = (version - version_decomposed[0] * 10000 - version_decomposed[1] * 100);
		printf("VkFFT v%d.%d.%d. Author: Tolmachev Dmitrii\n", version_decomposed[0], version_decomposed[1], version_decomposed[2]);
		printf(VKFFT_BACKEND_NAME_STRING);
		printf(" backend\n");
		printf("	-h: print help\n");
		printf("	-devices: print the list of available device ids, used as -d argument\n");
		printf("	-d X: select device (default 0)\n");
		printf("	-o NAME: specify output file path\n");
		printf("	-vkfft X: launch VkFFT sample X:\n");
		printf("		0 - FFT + iFFT C2C benchmark 1D batched in single precision\n");
		printf("		1 - FFT + iFFT C2C benchmark 1D batched in double precision LUT\n");
		printf("		2 - FFT + iFFT C2C benchmark 1D batched in half precision\n");	
		printf("		3 - FFT + iFFT C2C multidimensional benchmark in single precision\n");
		printf("		4 - FFT + iFFT C2C multidimensional benchmark in single precision, native zeropadding\n");
		printf("		5 - FFT + iFFT C2C benchmark 1D batched in single precision, no reshuffling\n");
		printf("		6 - FFT + iFFT R2C / C2R benchmark\n");
		printf("		7 - FFT + iFFT C2C Bluestein benchmark in single precision\n");
		printf("		8 - FFT + iFFT C2C Bluestein benchmark in double precision\n");
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
		printf("		9 - FFT + iFFT C2C benchmark 1D batched in double-double emulation of quad precision LUT\n");
#endif
#ifdef VKFFT_USE_MULTIPLE_BUFFER
		printf("		10 - multiple buffer(4 by default) split version of benchmark 0\n");
#endif
#ifdef USE_FFTW
#ifdef USE_cuFFT
		printf("		11 - VkFFT / cuFFT / FFTW C2C precision test in single precision\n");
		printf("		12 - VkFFT / cuFFT / FFTW C2C precision test in double precision\n");
		printf("		13 - VkFFT / cuFFT / FFTW C2C precision test in half precision\n");
		printf("		14 - VkFFT / FFTW C2C radix 3 / 5 / 7 / 11 / 13 / Bluestein precision test in single precision\n");
		printf("		15 - VkFFT / cuFFT / FFTW R2C+C2R precision test in single precision\n");
		printf("		16 - VkFFT / FFTW R2R DCT-I, II, III and IV precision test in single precision\n");
		printf("		17 - VkFFT / FFTW R2R DCT-I, II, III and IV precision test in double precision\n");
		printf("		18 - VkFFT / FFTW C2C radix 3 / 5 / 7 / 11 / 13 / Bluestein precision test in double precision\n");
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
		printf("		19 - VkFFT / FFTW C2C precision test in double-double emulation of quad precision\n");
#endif
#elif USE_rocFFT
		printf("		11 - VkFFT / rocFFT / FFTW C2C precision test in single precision\n");
		printf("		12 - VkFFT / rocFFT / FFTW C2C precision test in double precision\n");
		printf("		13 - VkFFT / FFTW C2C precision test in half precision\n");
		printf("		14 - VkFFT / FFTW C2C radix 3 / 5 / 7 / 11 / 13 / Bluestein precision test in single precision\n");
		printf("		15 - VkFFT / rocFFT / FFTW R2C+C2R precision test in single precision\n");
		printf("		16 - VkFFT / FFTW R2R DCT-I, II, III and IV precision test in single precision\n");
		printf("		17 - VkFFT / FFTW R2R DCT-I, II, III and IV precision test in double precision\n");
		printf("		18 - VkFFT / FFTW C2C radix 3 / 5 / 7 / 11 / 13 / Bluestein precision test in double precision\n");
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
		printf("		19 - VkFFT / FFTW C2C precision test in double-double emulation of quad precision\n");
#endif
#else
		printf("		11 - VkFFT / FFTW C2C precision test in single precision\n");
		printf("		12 - VkFFT / FFTW C2C precision test in double precision\n");
		printf("		13 - VkFFT / FFTW C2C precision test in half precision\n");
		printf("		14 - VkFFT / FFTW C2C radix 3 / 5 / 7 / 11 / 13 / Bluestein precision test in single precision\n");
		printf("		15 - VkFFT / FFTW R2C+C2R precision test in single precision\n");
		printf("		16 - VkFFT / FFTW R2R DCT-I, II, III and IV precision test in single precision\n");
		printf("		17 - VkFFT / FFTW R2R DCT-I, II, III and IV precision test in double precision\n");
		printf("		18 - VkFFT / FFTW C2C radix 3 / 5 / 7 / 11 / 13 / Bluestein precision test in double precision\n");
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
		printf("		19 - VkFFT / FFTW C2C precision test in double-double emulation of quad precision\n");
#endif
#endif
#endif
		printf("		50 - convolution example with identity kernel\n");
		printf("		51 - zeropadding convolution example with identity kernel\n");
		printf("		52 - batched convolution example with identity kernel\n");
		printf("		53 - convolution example with one scaling kernel of three colors, multiple images of three colors\n");
		printf("		110 - VkFFT FFT + iFFT R2R DCT-1 multidimensional benchmark in single precision\n");
		printf("		111 - VkFFT FFT + iFFT R2R DCT-1 multidimensional benchmark in double precision\n");
		printf("		120 - VkFFT FFT + iFFT R2R DCT-2 multidimensional benchmark in single precision\n");
		printf("		121 - VkFFT FFT + iFFT R2R DCT-2 multidimensional benchmark in double precision\n");
		printf("		130 - VkFFT FFT + iFFT R2R DCT-3 multidimensional benchmark in single precision\n");
		printf("		131 - VkFFT FFT + iFFT R2R DCT-3 multidimensional benchmark in double precision\n");
		printf("		140 - VkFFT FFT + iFFT R2R DCT-4 multidimensional benchmark in single precision\n");
		printf("		141 - VkFFT FFT + iFFT R2R DCT-4 multidimensional benchmark in double precision\n");

		printf("		1000 - FFT + iFFT C2C benchmark 1D batched in single precision: all supported systems from 2 to 4096\n");
		printf("		1001 - FFT + iFFT C2C benchmark 1D batched in double precision: all supported systems from 2 to 4096\n");
		printf("		1002 - FFT + iFFT C2C benchmark 1D batched in half precision: all supported systems from 2 to 4096\n");
		printf("		1003 - FFT + iFFT C2C multidimensional benchmark in single precision: all supported cubes from 2 to 512\n");
#ifdef VKFFT_USE_DOUBLEDOUBLE_FP128
		printf("		1004 - FFT + iFFT C2C benchmark 1D batched in double-double emulation of quad precision: all supported systems from 2 to 4096\n");
#endif
		printf("	-benchmark_vkfft: run VkFFT benchmark on a user-defined system:\n\
		-X uint, -Y uint, -Z uint - FFT dimensions (default Y and Z are 1)\n");
		printf("\
		-P uint - precision (0 - single, 1 - double, 2 - half, 3 - double-double) (default 0)\n");
		printf("\
		-B uint - number of batched systems (default 1)\n\
		-N uint - number of consecutive FFT+iFFT iterations (default 1)\n\
		-R2C uint - use R2C (0 - off, 1 - on) (default 0)\n\
		-DCT uint - perform DCT (0 - off, else type: 1, 2, 3 or 4) (default 0)\n\
		-save - save generated binaries\n\
		-load - load previously generated binaries\n");
#ifdef USE_cuFFT
		printf("	-cufft X: launch cuFFT sample X:\n");
		printf("		0 - FFT + iFFT C2C benchmark 1D batched in single precision\n");
		printf("		1 - FFT + iFFT C2C benchmark 1D batched in double precision LUT\n");
		printf("		2 - FFT + iFFT C2C benchmark 1D batched in half precision\n");
		printf("		3 - FFT + iFFT C2C multidimensional benchmark in single precision\n");
		printf("		6 - FFT + iFFT R2C / C2R benchmark\n");
		printf("		7 - FFT + iFFT C2C big prime benchmark in single precision (similar to VkFFT Bluestein)\n");
		printf("		8 - FFT + iFFT C2C big prime benchmark in double precision (similar to VkFFT Bluestein)\n");
		printf("		1000 - FFT + iFFT C2C benchmark 1D batched in single precision: all supported systems from 2 to 4096\n");
		printf("		1001 - FFT + iFFT C2C benchmark 1D batched in double precision: all supported systems from 2 to 4096\n");
		printf("		1003 - FFT + iFFT C2C multidimensional benchmark in single precision: all supported cubes from 2 to 512\n");
		printf("	-test: (or no -vkfft and -cufft keys) run vkfft benchmarks 0-6 and cufft benchmarks 0-6\n");
		printf("	-benchmark_cufft: run cuFFT benchmark on a user-defined system:\n\
		-X uint, -Y uint, -Z uint - FFT dimensions (default Y and Z are 1)\n\
		-P uint - precision (0 - single, 1 - double) (default 0)\n\
		-B uint - number of batched systems (default 1)\n\
		-N uint - number of consecutive FFT+iFFT iterations (default 1)\n\
		-R2C uint - use R2C (0 - off, 1 - on) (default 0)\n");
#elif USE_rocFFT
		printf("	-rocfft X: launch rocFFT sample X:\n");
		printf("		0 - FFT + iFFT C2C benchmark 1D batched in single precision\n");
		printf("		1 - FFT + iFFT C2C benchmark 1D batched in double precision LUT\n");
		printf("		3 - FFT + iFFT C2C multidimensional benchmark in single precision\n");
		printf("		6 - FFT + iFFT R2C / C2R benchmark\n");
		printf("		7 - FFT + iFFT C2C big prime benchmark in single precision (similar to VkFFT Bluestein)\n");
		printf("		8 - FFT + iFFT C2C big prime benchmark in double precision (similar to VkFFT Bluestein)\n");
		printf("		1000 - FFT + iFFT C2C benchmark 1D batched in single precision: all supported systems from 2 to 4096\n");
		printf("		1001 - FFT + iFFT C2C benchmark 1D batched in double precision: all supported systems from 2 to 4096\n");
		printf("		1003 - FFT + iFFT C2C multidimensional benchmark in single precision: all supported cubes from 2 to 512\n");
		printf("	-test: (or no -vkfft and -rocfft keys) run vkfft benchmarks 0-6 and rocfft benchmarks 0-6\n");
		printf("	-benchmark_rocfft: run rocFFT benchmark on a user-defined system:\n\
		-X uint, -Y uint, -Z uint - FFT dimensions (default Y and Z are 1)\n\
		-P uint - precision (0 - single, 1 - double) (default 0)\n\
		-B uint - number of batched systems (default 1)\n\
		-N uint - number of consecutive FFT+iFFT iterations (default 1)\n\
		-R2C uint - use R2C (0 - off, 1 - on) (default 0)\n");
#else
		printf("	-test: run vkfft benchmarks 0-6\n");
		printf("	-cufft command is disabled\n");
		printf("	-rocfft command is disabled\n");
#endif
		return 0;
	}
	if (findFlag(argv, argv + argc, "-devices"))
	{
		//print device list
		VkFFTResult resFFT = devices_list();
		return resFFT;
	}
	if (findFlag(argv, argv + argc, "-d"))
	{
		//select device_id
		char* value = getFlagValue(argv, argv + argc, "-d");
		if (value != 0) {
			sscanf_res = sscanf(value, "%" PRIu64 "", &vkGPU.device_id);
			if (sscanf_res <= 0) {
				printf("sscanf failed\n");
				return 1;
			}
		}
		else {
			printf("No device is selected with -d flag\n");
			return 1;
		}
	}
	if (findFlag(argv, argv + argc, "-o"))
	{
		//specify output file
		char* value = getFlagValue(argv, argv + argc, "-o");
		if (value != 0) {
			file_output = true;
			output = fopen(value, "a");
		}
		else {
			printf("No output file is selected with -o flag\n");
			return 1;
		}
	}
	if (findFlag(argv, argv + argc, "-benchmark_vkfft") || findFlag(argv, argv + argc, "-benchmark_cufft") || findFlag(argv, argv + argc, "-benchmark_rocfft"))
	{
		//select sample_id
		VkFFTUserSystemParameters userParams = {};
		userParams.X = 1;
		userParams.Y = 1;
		userParams.Z = 1;
		userParams.P = 0;
		userParams.B = 1;
		userParams.N = 1;
		userParams.R2C = 0;
		userParams.DCT = 0;
		if (findFlag(argv, argv + argc, "-X"))
		{
			char* value = getFlagValue(argv, argv + argc, "-X");
			if (value != 0) {
				sscanf_res = sscanf(value, "%" PRIu64 "", &userParams.X);
				if (sscanf_res <= 0) {
					printf("sscanf failed\n");
					return 1;
				}
			}
			else {
				printf("No dimension is selected with -X flag\n");
				return 1;
			}
		}
		else {
			printf("No -X flag is selected\n");
			return 1;
		}
		if (findFlag(argv, argv + argc, "-Y"))
		{
			char* value = getFlagValue(argv, argv + argc, "-Y");
			if (value != 0) {
				sscanf_res = sscanf(value, "%" PRIu64 "", &userParams.Y);
				if (sscanf_res <= 0) {
					printf("sscanf failed\n");
					return 1;
				}
			}
			else {
				printf("No dimension is selected with -Y flag\n");
				return 1;
			}
		}
		if (findFlag(argv, argv + argc, "-Z"))
		{
			char* value = getFlagValue(argv, argv + argc, "-Z");
			if (value != 0) {
				sscanf_res = sscanf(value, "%" PRIu64 "", &userParams.Z);
				if (sscanf_res <= 0) {
					printf("sscanf failed\n");
					return 1;
				}
			}
			else {
				printf("No dimension is selected with -Z flag\n");
				return 1;
			}
		}
		if (findFlag(argv, argv + argc, "-P"))
		{
			char* value = getFlagValue(argv, argv + argc, "-P");
			if (value != 0) {
				sscanf_res = sscanf(value, "%" PRIu64 "", &userParams.P);
				if (sscanf_res <= 0) {
					printf("sscanf failed\n");
					return 1;
				}
			}
			else {
				printf("No precision is selected with -P flag\n");
				return 1;
			}
		}
		if (findFlag(argv, argv + argc, "-B"))
		{
			char* value = getFlagValue(argv, argv + argc, "-B");
			if (value != 0) {
				sscanf_res = sscanf(value, "%" PRIu64 "", &userParams.B);
				if (sscanf_res <= 0) {
					printf("sscanf failed\n");
					return 1;
				}
			}
			else {
				printf("No batch is selected with -B flag\n");
				return 1;
			}
		}
		if (findFlag(argv, argv + argc, "-N"))
		{
			char* value = getFlagValue(argv, argv + argc, "-N");
			if (value != 0) {
				sscanf_res = sscanf(value, "%" PRIu64 "", &userParams.N);
				if (sscanf_res <= 0) {
					printf("sscanf failed\n");
					return 1;
				}
			}
			else {
				printf("No number of iterations is selected with -N flag\n");
				return 1;
			}
		}
		if (findFlag(argv, argv + argc, "-R2C"))
		{
			char* value = getFlagValue(argv, argv + argc, "-R2C");
			if (value != 0) {
				sscanf_res = sscanf(value, "%" PRIu64 "", &userParams.R2C);
				if (sscanf_res <= 0) {
					printf("sscanf failed\n");
					return 1;
				}
			}
			else {
				printf("No R2C parameter is selected with -R2C flag\n");
				return 1;
			}
		}
		if (findFlag(argv, argv + argc, "-DCT"))
		{
			char* value = getFlagValue(argv, argv + argc, "-DCT");
			if (value != 0) {
				sscanf_res = sscanf(value, "%" PRIu64 "", &userParams.DCT);
				if (sscanf_res <= 0) {
					printf("sscanf failed\n");
					return 1;
				}
			}
			else {
				printf("No DCT parameter is selected with -DCT flag\n");
				return 1;
			}
		}
		if (findFlag(argv, argv + argc, "-save"))
		{
			userParams.saveApplicationToString = 1;
		}
		if (findFlag(argv, argv + argc, "-load"))
		{
			userParams.loadApplicationFromString = 1;
		}
		if (findFlag(argv, argv + argc, "-benchmark_vkfft")) {
			VkFFTResult resFFT = launchVkFFT(&vkGPU, 200 + userParams.P, file_output, output, &userParams);
			if (resFFT != VKFFT_SUCCESS) return resFFT;
		}
		else {
#ifdef USE_cuFFT
			if (findFlag(argv, argv + argc, "-benchmark_cufft")) {
				user_benchmark_cuFFT(file_output, output, (cuFFTUserSystemParameters*)(&userParams), (int)vkGPU.device_id);
			}
			return 0;
#elif USE_rocFFT
			if (findFlag(argv, argv + argc, "-benchmark_rocfft")) {
				user_benchmark_rocFFT(file_output, output, (rocFFTUserSystemParameters*)(&userParams), (int)vkGPU.device_id);
			}
			return 0;
#endif
			return 1;
		}
		return 0;
	}

	if (findFlag(argv, argv + argc, "-vkfft"))
	{
		//select sample_id
		char* value = getFlagValue(argv, argv + argc, "-vkfft");
		if (value != 0) {
			uint64_t sample_id = 0;
			sscanf_res = sscanf(value, "%" PRIu64 "", &sample_id);
			if (sscanf_res <= 0) {
				printf("sscanf failed\n");
				return 1;
			}
			VkFFTResult resFFT = launchVkFFT(&vkGPU, sample_id, file_output, output, 0);
			if (resFFT != VKFFT_SUCCESS) return resFFT;
		}
		else {
			printf("No sample is selected with -vkfft flag\n");
			return 1;
		}
	}
#ifdef USE_cuFFT
	if (findFlag(argv, argv + argc, "-cufft"))
	{
		//select sample_id
		char* value = getFlagValue(argv, argv + argc, "-cufft");
		if (value != 0) {
			uint64_t sample_id = 0;
			sscanf_res = sscanf(value, "%" PRIu64 "", &sample_id);
			if (sscanf_res <= 0) {
				printf("sscanf failed\n");
				return 1;
			}
			switch (sample_id) {
			case 0:
				sample_0_benchmark_cuFFT_single(file_output, output, (int)vkGPU.device_id);
				break;
			case 1:
				sample_1_benchmark_cuFFT_double(file_output, output, (int)vkGPU.device_id);
				break;
			case 2:
				sample_2_benchmark_cuFFT_half(file_output, output, (int)vkGPU.device_id);
				break;
			case 3:
				sample_3_benchmark_cuFFT_single_3d(file_output, output, (int)vkGPU.device_id);
				break;
			case 6:
				sample_6_benchmark_cuFFT_single_r2c(file_output, output, (int)vkGPU.device_id);
				break;
			case 7:
				sample_7_benchmark_cuFFT_single_Bluestein(file_output, output, (int)vkGPU.device_id);
				break;
			case 8:
				sample_8_benchmark_cuFFT_double_Bluestein(file_output, output, (int)vkGPU.device_id);
				break;
			case 1000:
				sample_1000_benchmark_cuFFT_single_2_4096(file_output, output, (int)vkGPU.device_id);
				break;
			case 1001:
				sample_1001_benchmark_cuFFT_double_2_4096(file_output, output, (int)vkGPU.device_id);
				break;
			case 1003:
				sample_1003_benchmark_cuFFT_single_3d_2_512(file_output, output, (int)vkGPU.device_id);
				break;
			}
		}
		else {
			printf("No cuFFT script is selected with -cufft flag\n");
			return 1;
		}
	}
#elif USE_rocFFT
	if (findFlag(argv, argv + argc, "-rocfft"))
	{
		//select sample_id
		char* value = getFlagValue(argv, argv + argc, "-rocfft");
		if (value != 0) {
			uint64_t sample_id = 0;
			sscanf_res = sscanf(value, "%" PRIu64 "", &sample_id);
			if (sscanf_res <= 0) {
				printf("sscanf failed\n");
				return 1;
			}
			switch (sample_id) {
			case 0:
				sample_0_benchmark_rocFFT_single(file_output, output, (int)vkGPU.device_id);
				break;
			case 1:
				sample_1_benchmark_rocFFT_double(file_output, output, (int)vkGPU.device_id);
				break;
			case 3:
				sample_3_benchmark_rocFFT_single_3d(file_output, output, (int)vkGPU.device_id);
				break;
			case 6:
				sample_6_benchmark_rocFFT_single_r2c(file_output, output, (int)vkGPU.device_id);
				break;
			case 7:
				sample_7_benchmark_rocFFT_single_Bluestein(file_output, output, (int)vkGPU.device_id);
				break;
			case 8:
				sample_8_benchmark_rocFFT_double_Bluestein(file_output, output, (int)vkGPU.device_id);
				break;
			case 1000:
				sample_1000_benchmark_rocFFT_single_2_4096(file_output, output, (int)vkGPU.device_id);
				break;
			case 1001:
				sample_1001_benchmark_rocFFT_double_2_4096(file_output, output, (int)vkGPU.device_id);
				break;
			case 1003:
				sample_1003_benchmark_rocFFT_single_3d_2_512(file_output, output, (int)vkGPU.device_id);
				break;
			}
		}
		else {
			printf("No rocFFT script is selected with -rocfft flag\n");
			return 1;
		}
	}
#endif
	if ((findFlag(argv, argv + argc, "-test")) || ((!findFlag(argv, argv + argc, "-cufft")) && (!findFlag(argv, argv + argc, "-rocfft")) && (!findFlag(argv, argv + argc, "-vkfft"))))
	{
		if (output == NULL) {
			file_output = true;
			output = fopen("result.txt", "a");
		}
		for (uint64_t i = 0; i < 9; i++) {
#if((VKFFT_BACKEND>0) || (VK_API_VERSION == 10))
			if (i == 2) i++;
#endif
			VkFFTResult resFFT = launchVkFFT(&vkGPU, i, file_output, output, 0);
			if (resFFT != VKFFT_SUCCESS) return resFFT;
		}
#ifdef USE_cuFFT
		sample_0_benchmark_cuFFT_single(file_output, output, (int)vkGPU.device_id);
		sample_1_benchmark_cuFFT_double(file_output, output, (int)vkGPU.device_id);
		sample_2_benchmark_cuFFT_half(file_output, output, (int)vkGPU.device_id);
		sample_3_benchmark_cuFFT_single_3d(file_output, output, (int)vkGPU.device_id);
		sample_6_benchmark_cuFFT_single_r2c(file_output, output, (int)vkGPU.device_id);
		sample_7_benchmark_cuFFT_single_Bluestein(file_output, output, (int)vkGPU.device_id);
		sample_8_benchmark_cuFFT_double_Bluestein(file_output, output, (int)vkGPU.device_id);
#elif USE_rocFFT
		sample_0_benchmark_rocFFT_single(file_output, output, (int)vkGPU.device_id);
		sample_1_benchmark_rocFFT_double(file_output, output, (int)vkGPU.device_id);
		sample_3_benchmark_rocFFT_single_3d(file_output, output, (int)vkGPU.device_id);
		sample_6_benchmark_rocFFT_single_r2c(file_output, output, (int)vkGPU.device_id);
		sample_7_benchmark_rocFFT_single_Bluestein(file_output, output, (int)vkGPU.device_id);
		sample_8_benchmark_rocFFT_double_Bluestein(file_output, output, (int)vkGPU.device_id);
#endif
	}
	return 0;
}
