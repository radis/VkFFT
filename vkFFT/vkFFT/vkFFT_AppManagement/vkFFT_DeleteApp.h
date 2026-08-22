// This file is part of VkFFT
//
// Copyright (C) 2021 - present Dmitrii Tolmachev <dtolm96@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#ifndef VKFFT_DELETEAPP_H
#define VKFFT_DELETEAPP_H
#include "backend.h"
#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"
#include "vkFFT/vkFFT_PlanManagement/vkFFT_API_handles/vkFFT_DeletePlan.h"
#include "vkFFT/vkFFT_PlanManagement/vkFFT_API_handles/vkFFT_UpdateBuffers.h"

static inline void deleteVkFFT(VkFFTApplication* app) {
	if (app == 0) {
		return;
	}
	if (app->numRaderFFTPrimes) {
		for (pfUINT i = 0; i < app->numRaderFFTPrimes; i++) {
			free(app->raderFFTkernel[i]);
			app->raderFFTkernel[i] = 0;
		}
	}
	if (app->configuration.bufferSize != 0) {
		free(app->configuration.bufferSize);
		app->configuration.bufferSize = 0;
	}
	if (app->configuration.buffer != 0) {
		free((void*)app->configuration.buffer);
		app->configuration.buffer = 0;
	}
	if (!app->configuration.userTempBuffer) {
		if (app->configuration.allocateTempBuffer && (app->configuration.tempBuffer != 0)) {
			app->configuration.allocateTempBuffer = 0;

			if (app->configuration.tempBuffer[0] != 0) {
				if (!deleteVkFFT_backendDestroyBuffer(VKFFT_BACKEND_DEVICE_PTR[0], VKFFT_BACKEND_GET_BUFFER_RESOURCES(app->configuration.tempBuffer, 0))){
					app->configuration.tempBuffer[0] = 0; //CHECK: Metal now also zeros the buffer, this is different from before
				}
			}
		}
	}
	if (app->configuration.tempBufferSize != 0) {
		free(app->configuration.tempBufferSize);
		app->configuration.tempBufferSize = 0;
	}
	if (app->configuration.tempBuffer != 0) {
		free(app->configuration.tempBuffer);
		app->configuration.tempBuffer = 0;
	}
	if (app->configuration.isInputFormatted) {
		if (app->configuration.inputBufferSize != 0) {
			free(app->configuration.inputBufferSize);
			app->configuration.inputBufferSize = 0;
		}
		if (app->configuration.inputBuffer != 0) {
			free((void*)app->configuration.inputBuffer);
			app->configuration.inputBuffer = 0;
		}
	}
	else {
		app->configuration.inputBufferSize = 0;
		app->configuration.inputBuffer = 0;
	}
	if (app->configuration.isOutputFormatted) {
		if (app->configuration.outputBufferSize != 0) {
			free(app->configuration.outputBufferSize);
			app->configuration.outputBufferSize = 0;
		}
		if (app->configuration.outputBuffer != 0) {
			free((void*)app->configuration.outputBuffer);
			app->configuration.outputBuffer = 0;
		}
	}
	else {
		app->configuration.outputBufferSize = 0;
		app->configuration.outputBuffer = 0;
	}
	if (app->configuration.performConvolution) {
		if (app->configuration.kernelSize != 0) {
			free(app->configuration.kernelSize);
			app->configuration.kernelSize = 0;
		}
		if (app->configuration.kernel != 0) {
			free((void*)app->configuration.kernel);
			app->configuration.kernel = 0;
		}
	}
	for (pfUINT i = 0; i < app->configuration.FFTdim; i++) {
		if (app->configuration.useRaderUintLUT) {
			for (pfUINT j = 0; j < 4; j++) {
				if (app->bufferRaderUintLUT[i][j]) {
					if (!deleteVkFFT_backendDestroyBuffer(VKFFT_BACKEND_DEVICE_PTR[0], VKFFT_BACKEND_GET_BUFFER_RESOURCES(app->bufferRaderUintLUT, i, j))){
						app->bufferRaderUintLUT[i][j] = 0;
					}
				}
			}
		}
		if (app->useBluesteinFFT[i]) {
			
			if (app->bufferBluestein[i] != 0) {
				if (!deleteVkFFT_backendDestroyBuffer(VKFFT_BACKEND_DEVICE_PTR[0], VKFFT_BACKEND_GET_BUFFER_RESOURCES(app->bufferBluestein, i))){
					app->bufferBluestein[i] = 0;
				}
			}
			if (app->bufferBluesteinFFT[i] != 0) {
				if (!deleteVkFFT_backendDestroyBuffer(VKFFT_BACKEND_DEVICE_PTR[0], VKFFT_BACKEND_GET_BUFFER_RESOURCES(app->bufferBluesteinFFT, i))){
					app->bufferBluesteinFFT[i] = 0;
				}
			}
			if (app->bufferBluesteinIFFT[i] != 0) {
				if (!deleteVkFFT_backendDestroyBuffer(VKFFT_BACKEND_DEVICE_PTR[0], VKFFT_BACKEND_GET_BUFFER_RESOURCES(app->bufferBluesteinIFFT, i))){
					app->bufferBluesteinIFFT[i] = 0;
				}
			}
		}
	}
	if (!app->configuration.makeInversePlanOnly) {
		if (app->localFFTPlan != 0) {
			for (pfUINT i = 0; i < app->configuration.FFTdim; i++) {
				if (app->localFFTPlan->numAxisUploads[i] > 0) {
					for (pfUINT j = 0; j < app->localFFTPlan->numAxisUploads[i]; j++)
						deleteAxis(app, &app->localFFTPlan->axes[i][j], 0);
				}
				if (app->useBluesteinFFT[i] && (app->localFFTPlan->numAxisUploads[i] > 1)) {
					for (pfUINT j = 1; j < app->localFFTPlan->numAxisUploads[i]; j++) {
						deleteAxis(app, &app->localFFTPlan->inverseBluesteinAxes[i][j], 1);
					}
				}
			}
			if (app->localFFTPlan->bigSequenceEvenR2C) {
				deleteAxis(app, &app->localFFTPlan->R2Cdecomposition, 0);
			}
			if (app->localFFTPlan != 0) {
				free(app->localFFTPlan);
				app->localFFTPlan = 0;
			}
		}
	}
	if (!app->configuration.makeForwardPlanOnly) {
		if (app->localFFTPlan_inverse != 0) {
			for (pfUINT i = 0; i < app->configuration.FFTdim; i++) {
				if (app->localFFTPlan_inverse->numAxisUploads[i] > 0) {
					for (pfUINT j = 0; j < app->localFFTPlan_inverse->numAxisUploads[i]; j++)
						deleteAxis(app, &app->localFFTPlan_inverse->axes[i][j], 0);
				}
				if (app->useBluesteinFFT[i] && (app->localFFTPlan_inverse->numAxisUploads[i] > 1)) {
					for (pfUINT j = 1; j < app->localFFTPlan_inverse->numAxisUploads[i]; j++) {
						deleteAxis(app, &app->localFFTPlan_inverse->inverseBluesteinAxes[i][j], 1);
					}
				}
			}
			if (app->localFFTPlan_inverse->bigSequenceEvenR2C) {
				deleteAxis(app, &app->localFFTPlan_inverse->R2Cdecomposition, 0);
			}
			if (app->localFFTPlan_inverse != 0) {
				free(app->localFFTPlan_inverse);
				app->localFFTPlan_inverse = 0;
			}
		}
	}
	if (app->configuration.saveApplicationToString) {
		if (app->saveApplicationString != 0) {
			free(app->saveApplicationString);
			app->saveApplicationString = 0;
		}
		for (pfUINT i = 0; i < app->configuration.FFTdim; i++) {
			if (app->applicationBluesteinString[i] != 0) {
				free(app->applicationBluesteinString[i]);
				app->applicationBluesteinString[i] = 0;
			}
		}
	}
	if (app->configuration.primeSizes != 0) {
		free(app->configuration.primeSizes);
		app->configuration.primeSizes = 0;
	}
	if (app->configuration.paddedSizes != 0) {
		free(app->configuration.paddedSizes);
		app->configuration.paddedSizes = 0;
	}
	
	deleteVkFFT_backendFreeAPI((backendVkFFTConfiguration*) &app->configuration);

	memset(app, 0, sizeof(VkFFTApplication));
}
#endif
