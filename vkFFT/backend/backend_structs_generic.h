
#ifndef VKFFT_BACKEND_STRUCTS_GENERIC_H
#define VKFFT_BACKEND_STRUCTS_GENERIC_H


typedef struct{ VKFFT_BACKEND_FFT_CONFIGURATION } backendVkFFTConfiguration;
typedef struct{ VKFFT_BACKEND_LAUNCH_PARAMS     } backendVkFFTLaunchParams;
typedef struct{ VKFFT_BACKEND_SPEC_CONST_LAYOUT } backendVkFFTSpecializationConstantsLayout;
typedef struct{ VKFFT_BACKEND_PUSH_CONST_LAYOUT } backendVkFFTPushConstantsLayout;
typedef struct{ VKFFT_BACKEND_AXIS              } backendVkFFTAxis;
typedef struct{ VKFFT_BACKEND_APPLICATION       } backendVkFFTApplication;


#define VKFFT_BACKEND_GET_BUFFER_0(buf)          VKFFT_BACKEND_GET_BUFFER_RESOURCES_IDX(buf,)
#define VKFFT_BACKEND_GET_BUFFER_1(buf, i)       VKFFT_BACKEND_GET_BUFFER_RESOURCES_IDX(buf,[i])
#define VKFFT_BACKEND_GET_BUFFER_2(buf, i, j)    VKFFT_BACKEND_GET_BUFFER_RESOURCES_IDX(buf,[i][j])

#define VKFFT_BACKEND_SELECT(_0, _1, _2, NAME, ...) NAME
#define VKFFT_BACKEND_GLUE(macro, args) macro##args
#define VKFFT_BACKEND_GET_BUFFER_RESOURCES(...) VKFFT_BACKEND_GLUE(\
	VKFFT_BACKEND_SELECT,(__VA_ARGS__,\
		    VKFFT_BACKEND_GET_BUFFER_2,\
		    VKFFT_BACKEND_GET_BUFFER_1,\
		    VKFFT_BACKEND_GET_BUFFER_0)\
	)(__VA_ARGS__)

#endif //VKFFT_BACKEND_STRUCTS_GENERIC_H

