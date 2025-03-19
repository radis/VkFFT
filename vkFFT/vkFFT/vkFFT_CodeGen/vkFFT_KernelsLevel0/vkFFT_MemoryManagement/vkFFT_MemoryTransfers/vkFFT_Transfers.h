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
#ifndef VKFFT_TRANSFERS_H
#define VKFFT_TRANSFERS_H
#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"
#include "vkFFT/vkFFT_CodeGen/vkFFT_StringManagement/vkFFT_StringManager.h"

static inline void appendSharedToRegisters(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* sdataID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s];\n", out->data.c[0].name, sdataID->name);
		PfAppendLine(sc);
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s + %" PRIi64 "];\n", out->data.c[1].name, sdataID->name, sc->offsetImaginaryShared.data.i);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s];\n", out->name, sdataID->name);
		PfAppendLine(sc);
	}
	return;
}
static inline void appendSharedToRegisters_x_x(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* sdataID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s];\n", out->data.c[0].name, sdataID->name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s].x;\n", out->data.c[0].name, sdataID->name);
		PfAppendLine(sc);
	}
	return;
}
static inline void appendSharedToRegisters_x_y(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* sdataID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s + %" PRIi64 "];\n", out->data.c[0].name, sdataID->name, sc->offsetImaginaryShared.data.i);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s].y;\n", out->data.c[0].name, sdataID->name);
		PfAppendLine(sc);
	}
	return;
}
static inline void appendSharedToRegisters_y_x(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* sdataID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s];\n", out->data.c[1].name, sdataID->name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s].x;\n", out->data.c[1].name, sdataID->name);
		PfAppendLine(sc);
	}
	return;
}
static inline void appendSharedToRegisters_y_y(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* sdataID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s + %" PRIi64 "];\n", out->data.c[1].name, sdataID->name, sc->offsetImaginaryShared.data.i);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
%s = sdata[%s].y;\n", out->data.c[1].name, sdataID->name);
		PfAppendLine(sc);
	}
	return;
}

static inline void appendRegistersToShared(VkFFTSpecializationConstantsLayout* sc, PfContainer* sdataID, PfContainer* out)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s] = %s;\n", sdataID->name, out->data.c[0].name);
		PfAppendLine(sc);
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s + %" PRIi64 "] = %s;\n", sdataID->name, sc->offsetImaginaryShared.data.i, out->data.c[1].name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s] = %s;\n", sdataID->name, out->name);
		PfAppendLine(sc);
	}
	return;
}
static inline void appendRegistersToShared_x_x(VkFFTSpecializationConstantsLayout* sc, PfContainer* sdataID, PfContainer* out)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s] = %s;\n", sdataID->name, out->data.c[0].name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s].x = %s;\n", sdataID->name, out->data.c[0].name);
		PfAppendLine(sc);
	}
	return;
}
static inline void appendRegistersToShared_x_y(VkFFTSpecializationConstantsLayout* sc, PfContainer* sdataID, PfContainer* out)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s] = %s;\n", sdataID->name, out->data.c[1].name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s].x = %s;\n", sdataID->name, out->data.c[1].name);
		PfAppendLine(sc);
	}
	return;
}
static inline void appendRegistersToShared_y_y(VkFFTSpecializationConstantsLayout* sc, PfContainer* sdataID, PfContainer* out)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s + %" PRIi64 "] = %s;\n", sdataID->name, sc->offsetImaginaryShared.data.i, out->data.c[1].name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s].y = %s;\n", sdataID->name, out->data.c[1].name);
		PfAppendLine(sc);
	}
	return;
}
static inline void appendRegistersToShared_y_x(VkFFTSpecializationConstantsLayout* sc, PfContainer* sdataID, PfContainer* out)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s + %" PRIi64 "] = %s;\n", sdataID->name, sc->offsetImaginaryShared.data.i, out->data.c[0].name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "\
sdata[%s].y = %s;\n", sdataID->name, out->data.c[0].name);
		PfAppendLine(sc);
	}
	return;
}

static inline void appendConstantToRegisters(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* constantBufferName, PfContainer* inoutID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	sc->tempLen = sprintf(sc->tempStr, "\
%s = %s[%s];\n", out->name, constantBufferName->name, inoutID->name);
	PfAppendLine(sc);
	return;
}
static inline void appendConstantToRegisters_x(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* constantBufferName, PfContainer* inoutID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	sc->tempLen = sprintf(sc->tempStr, "\
%s = %s[%s];\n", out->data.c[0].name, constantBufferName->name, inoutID->name);
	PfAppendLine(sc);
	return;
}
static inline void appendConstantToRegisters_y(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* constantBufferName, PfContainer* inoutID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	sc->tempLen = sprintf(sc->tempStr, "\
%s = %s[%s];\n", out->data.c[1].name, constantBufferName->name, inoutID->name);
	PfAppendLine(sc);
	return;
}

static inline void appendGlobalToRegisters(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* bufferName, PfContainer* inoutID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferSeparateComplexComponents)) ||
	 ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferSeparateComplexComponents)) ||
	 ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelSeparateComplexComponents))) {
		PfContainer temp_int = VKFFT_ZERO_INIT;
		temp_int.type = 31;
		out->type -= 1;
		bufferName->type -= 1;
		sc->tempLen = sprintf(sc->tempStr, "%s.x", out->name);
		PfAppendLine(sc);
		sc->tempLen = sprintf(sc->tempStr, " = ");
		PfAppendLine(sc);
		PfAppendConversionStart(sc, out, bufferName);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
		if(sc->precision != 3){
			sc->tempLen = sprintf(sc->tempStr, "__ldg(&");
			PfAppendLine(sc);
		}
#endif
#if(VKFFT_BACKEND==0)
		sc->tempLen = sprintf(sc->tempStr, "%sBlocks[0].%s[%s +", bufferName->name, bufferName->name, inoutID->name);
#else
		sc->tempLen = sprintf(sc->tempStr, "%s[%s + ", bufferName->name, inoutID->name);
#endif
		PfAppendLine(sc);
		if (!(strcmp(bufferName->name, sc->inputsStruct.name))) {
			if (sc->inputOffset.type < 100) {
				temp_int.data.i = sc->inputOffset.data.i / (sc->inputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->inputOffset.type == 101) {
					if (sc->performPostCompilationInputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->inputOffset.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->outputsStruct.name))) {
			if (sc->outputOffset.type < 100) {
				temp_int.data.i = sc->outputOffset.data.i / (sc->outputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->outputOffset.type == 101) {
					if (sc->performPostCompilationOutputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->outputOffset.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->kernelStruct.name))) {
			if (sc->kernelOffset.type < 100) {
				temp_int.data.i = sc->kernelOffset.data.i / (sc->kernelNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->kernelOffset.type == 101) {
					if (sc->performPostCompilationKernelOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->kernelOffset.name);
						PfAppendLine(sc);
					}
				}
			}
		}
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
		if(sc->precision != 3){
			sc->tempLen = sprintf(sc->tempStr, ")");
			PfAppendLine(sc);
		}
#endif
		PfAppendConversionEnd(sc, out, bufferName);
		sc->tempLen = sprintf(sc->tempStr, ";\n");
		PfAppendLine(sc);

		sc->tempLen = sprintf(sc->tempStr, "%s.y", out->name);
		PfAppendLine(sc);
		sc->tempLen = sprintf(sc->tempStr, " = ");
		PfAppendLine(sc);
		PfAppendConversionStart(sc, out, bufferName);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
		if(sc->precision != 3){
			sc->tempLen = sprintf(sc->tempStr, "__ldg(&");
			PfAppendLine(sc);
		}
#endif
#if(VKFFT_BACKEND==0)
		sc->tempLen = sprintf(sc->tempStr, "%sBlocks[1].%s[%s +", bufferName->name, bufferName->name, inoutID->name);
#else
		sc->tempLen = sprintf(sc->tempStr, "%s_imag[%s + ", bufferName->name, inoutID->name);
#endif
		PfAppendLine(sc);
		if (!(strcmp(bufferName->name, sc->inputsStruct.name))) {
			if (sc->inputOffsetImaginary.type < 100) {
				temp_int.data.i = sc->inputOffsetImaginary.data.i / (sc->inputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->inputOffsetImaginary.type == 101) {
					if (sc->performPostCompilationInputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->inputOffsetImaginary.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->outputsStruct.name))) {
			if (sc->outputOffsetImaginary.type < 100) {
				temp_int.data.i = sc->outputOffsetImaginary.data.i / (sc->outputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->outputOffsetImaginary.type == 101) {
					if (sc->performPostCompilationOutputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->outputOffsetImaginary.name);
						PfAppendLine(sc);
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->kernelStruct.name))) {
			if (sc->kernelOffsetImaginary.type < 100) {
				temp_int.data.i = sc->kernelOffsetImaginary.data.i / (sc->kernelNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->kernelOffsetImaginary.type == 101) {
					if (sc->performPostCompilationKernelOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->kernelOffsetImaginary.name);
						PfAppendLine(sc);
					}
				}
			}
		}
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
		if(sc->precision != 3){
			sc->tempLen = sprintf(sc->tempStr, ")");
			PfAppendLine(sc);
		}
#endif
		PfAppendConversionEnd(sc, out, bufferName);
		sc->tempLen = sprintf(sc->tempStr, ";\n");
		PfAppendLine(sc);
		out->type += 1;
		bufferName->type += 1;
		return;
	}
	sc->tempLen = sprintf(sc->tempStr, "%s", out->name);
	PfAppendLine(sc);
	sc->tempLen = sprintf(sc->tempStr, " = ");
	PfAppendLine(sc);
	PfAppendConversionStart(sc, out, bufferName);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
	if(sc->precision != 3){
		sc->tempLen = sprintf(sc->tempStr, "__ldg(&");
		PfAppendLine(sc);
	}
#endif
	int dataSize = ((out->type % 10) == 3) ? sc->complexSize : sc->complexSize / 2;
	if ((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "inputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->inputBufferBlockSize / dataSize, bufferName->name, inoutID->name, sc->inputBufferBlockSize / dataSize);
	}
	else if ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "outputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->outputBufferBlockSize / dataSize, bufferName->name, inoutID->name, sc->outputBufferBlockSize / dataSize);
	}
	else if ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "kernelBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->kernelBlockSize / dataSize, bufferName->name, inoutID->name, sc->kernelBlockSize / dataSize);
	}
	else {
		if (sc->tempBufferInput && sc->optimizePow2StridesTempBuffer && (!(strcmp(bufferName->name, sc->inputsStruct.name))))
			sc->tempLen = sprintf(sc->tempStr, "%s[(%s & %" PRIu64 ") + (%s >> %" PRIu64 ") * %" PRIu64 "]", bufferName->name, inoutID->name, sc->inStridePadTempBuffer-1, inoutID->name, (uint64_t)log2(sc->inStridePadTempBuffer), sc->outStridePadTempBuffer);
		else
			sc->tempLen = sprintf(sc->tempStr, "%s[%s]", bufferName->name, inoutID->name);
	}
	PfAppendLine(sc);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
	if(sc->precision != 3){
		sc->tempLen = sprintf(sc->tempStr, ")");
		PfAppendLine(sc);
	}
#endif
	PfAppendConversionEnd(sc, out, bufferName);
	sc->tempLen = sprintf(sc->tempStr, ";\n");
	PfAppendLine(sc);
	return;
}
static inline void appendGlobalToRegisters_x(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* bufferName, PfContainer* inoutID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	
	sc->tempLen = sprintf(sc->tempStr, "%s", out->data.c[0].name);
	PfAppendLine(sc);
	sc->tempLen = sprintf(sc->tempStr, " = ");
	PfAppendLine(sc);
	PfAppendConversionStart(sc, &out->data.c[0], bufferName);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
	if(sc->precision != 3){
		sc->tempLen = sprintf(sc->tempStr, "__ldg(&");
		PfAppendLine(sc);
	}
#endif
	if ((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "inputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->inputBufferBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->inputBufferBlockSize / (sc->complexSize / 2));
	}
	else if ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "outputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->outputBufferBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->outputBufferBlockSize / (sc->complexSize / 2));
	}
	else if ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "kernelBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->kernelBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->kernelBlockSize / (sc->complexSize / 2));
	}
	else {
		sc->tempLen = sprintf(sc->tempStr, "%s[%s]", bufferName->name, inoutID->name);
	}
	PfAppendLine(sc);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
	if(sc->precision != 3){
		sc->tempLen = sprintf(sc->tempStr, ")");
		PfAppendLine(sc);
	}
#endif
	PfAppendConversionEnd(sc, &out->data.c[0], bufferName);
	sc->tempLen = sprintf(sc->tempStr, ";\n");
	PfAppendLine(sc);
	return;
}
static inline void appendGlobalToRegisters_y(VkFFTSpecializationConstantsLayout* sc, PfContainer* out, PfContainer* bufferName, PfContainer* inoutID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	
	sc->tempLen = sprintf(sc->tempStr, "%s", out->data.c[1].name);
	PfAppendLine(sc);
	sc->tempLen = sprintf(sc->tempStr, " = ");
	PfAppendLine(sc);
	PfAppendConversionStart(sc, &out->data.c[1], bufferName);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
	if(sc->precision != 3){
		sc->tempLen = sprintf(sc->tempStr, "__ldg(&");
		PfAppendLine(sc);
	}
#endif
	if ((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "inputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->inputBufferBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->inputBufferBlockSize / (sc->complexSize / 2));
	}
	else if ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "outputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->outputBufferBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->outputBufferBlockSize / (sc->complexSize / 2));
	}
	else if ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "kernelBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->kernelBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->kernelBlockSize / (sc->complexSize / 2));
	}
	else {
		sc->tempLen = sprintf(sc->tempStr, "%s[%s]", bufferName->name, inoutID->name);
	}
	PfAppendLine(sc);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
	if(sc->precision != 3){
		sc->tempLen = sprintf(sc->tempStr, ")");
		PfAppendLine(sc);
	}
#endif
	PfAppendConversionEnd(sc, &out->data.c[1], bufferName);
	sc->tempLen = sprintf(sc->tempStr, ";\n");
	PfAppendLine(sc);
	
	return;
}

static inline void appendRegistersToGlobal(VkFFTSpecializationConstantsLayout* sc, PfContainer* bufferName, PfContainer* inoutID, PfContainer* in)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferSeparateComplexComponents)) ||
	 ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferSeparateComplexComponents)) ||
	 ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelSeparateComplexComponents))) {
		PfContainer temp_int = VKFFT_ZERO_INIT;
		temp_int.type = 31;
		in->type -= 1;
		bufferName->type -= 1;
#if(VKFFT_BACKEND==0)
		sc->tempLen = sprintf(sc->tempStr, "%sBlocks[0].%s[%s +", bufferName->name, bufferName->name, inoutID->name);
#else
		sc->tempLen = sprintf(sc->tempStr, "%s[%s + ", bufferName->name, inoutID->name);
#endif
		PfAppendLine(sc);
		if (!(strcmp(bufferName->name, sc->inputsStruct.name))) {
			if (sc->inputOffset.type < 100) {
				temp_int.data.i = sc->inputOffset.data.i / (sc->inputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->inputOffset.type == 101) {
					if (sc->performPostCompilationInputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->inputOffset.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->outputsStruct.name))) {
			if (sc->outputOffset.type < 100) {
				temp_int.data.i = sc->outputOffset.data.i / (sc->outputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->outputOffset.type == 101) {
					if (sc->performPostCompilationOutputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->outputOffset.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->kernelStruct.name))) {
			if (sc->kernelOffset.type < 100) {
				temp_int.data.i = sc->kernelOffset.data.i / (sc->kernelNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->kernelOffset.type == 101) {
					if (sc->performPostCompilationKernelOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->kernelOffset.name);
						PfAppendLine(sc);
					}
				}
			}
		}
		sc->tempLen = sprintf(sc->tempStr, " = ");
		PfAppendLine(sc);
		PfAppendConversionStart(sc, bufferName, in);
		sc->tempLen = sprintf(sc->tempStr, "%s.x", in->name);
		PfAppendLine(sc);
		PfAppendConversionEnd(sc, bufferName, in);
		sc->tempLen = sprintf(sc->tempStr, ";\n");
		PfAppendLine(sc);

#if(VKFFT_BACKEND==0)
		sc->tempLen = sprintf(sc->tempStr, "%sBlocks[1].%s[%s +", bufferName->name, bufferName->name, inoutID->name);
#else
		sc->tempLen = sprintf(sc->tempStr, "%s_imag[%s + ", bufferName->name, inoutID->name);
#endif
		PfAppendLine(sc);
		if (!(strcmp(bufferName->name, sc->inputsStruct.name))) {
			if (sc->inputOffsetImaginary.type < 100) {
				temp_int.data.i = sc->inputOffsetImaginary.data.i / (sc->inputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->inputOffsetImaginary.type == 101) {
					if (sc->performPostCompilationInputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->inputOffsetImaginary.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->outputsStruct.name))) {
			if (sc->outputOffsetImaginary.type < 100) {
				temp_int.data.i = sc->outputOffsetImaginary.data.i / (sc->outputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->outputOffsetImaginary.type == 101) {
					if (sc->performPostCompilationOutputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->outputOffsetImaginary.name);
						PfAppendLine(sc);
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->kernelStruct.name))) {
			if (sc->kernelOffsetImaginary.type < 100) {
				temp_int.data.i = sc->kernelOffsetImaginary.data.i / (sc->kernelNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->kernelOffsetImaginary.type == 101) {
					if (sc->performPostCompilationKernelOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->kernelOffsetImaginary.name);
						PfAppendLine(sc);
					}
				}
			}
		}
		sc->tempLen = sprintf(sc->tempStr, " = ");
		PfAppendLine(sc);
		PfAppendConversionStart(sc, bufferName, in);
		sc->tempLen = sprintf(sc->tempStr, "%s.y", in->name);
		PfAppendLine(sc);
		PfAppendConversionEnd(sc, bufferName, in);
		sc->tempLen = sprintf(sc->tempStr, ";\n");
		PfAppendLine(sc);
		in->type += 1;
		bufferName->type += 1;
		return;
	}
	int dataSize = ((in->type % 10) == 3) ? sc->complexSize : sc->complexSize / 2;
	if ((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "inputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->inputBufferBlockSize / dataSize, bufferName->name, inoutID->name, sc->inputBufferBlockSize / dataSize);
	}
	else if ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "outputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->outputBufferBlockSize / dataSize, bufferName->name, inoutID->name, sc->outputBufferBlockSize / dataSize);
	}
	else if ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "kernelBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->kernelBlockSize / dataSize, bufferName->name, inoutID->name, sc->kernelBlockSize / dataSize);
	}
	else {
		if (sc->tempBufferOutput && sc->optimizePow2StridesTempBuffer && (!(strcmp(bufferName->name, sc->outputsStruct.name))))
			sc->tempLen = sprintf(sc->tempStr, "%s[(%s & %" PRIu64 ") + (%s >> %" PRIu64 ") * %" PRIu64 "]", bufferName->name, inoutID->name, sc->inStridePadTempBuffer-1, inoutID->name, (uint64_t)log2(sc->inStridePadTempBuffer), sc->outStridePadTempBuffer);
		else
			sc->tempLen = sprintf(sc->tempStr, "%s[%s]", bufferName->name, inoutID->name);
	}
	PfAppendLine(sc);
	sc->tempLen = sprintf(sc->tempStr, " = ");
	PfAppendLine(sc);
	PfAppendConversionStart(sc, bufferName, in);
	sc->tempLen = sprintf(sc->tempStr, "%s", in->name);
	PfAppendLine(sc);
	PfAppendConversionEnd(sc, bufferName, in);
	sc->tempLen = sprintf(sc->tempStr, ";\n");
	PfAppendLine(sc);
	return;
}
static inline void appendRegistersToGlobal_x(VkFFTSpecializationConstantsLayout* sc, PfContainer* bufferName, PfContainer* inoutID, PfContainer* in)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if ((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "inputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->inputBufferBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->inputBufferBlockSize / (sc->complexSize / 2));
	}
	else if ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "outputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->outputBufferBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->outputBufferBlockSize / (sc->complexSize / 2));
	}
	else if ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "kernelBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->kernelBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->kernelBlockSize / (sc->complexSize / 2));
	}
	else {
		sc->tempLen = sprintf(sc->tempStr, "%s[%s]", bufferName->name, inoutID->name);
	}
	PfAppendLine(sc);
	sc->tempLen = sprintf(sc->tempStr, " = ");
	PfAppendLine(sc);
	PfAppendConversionStart(sc, bufferName, &in->data.c[0]);
	sc->tempLen = sprintf(sc->tempStr, "%s", in->data.c[0].name);
	PfAppendLine(sc);
	PfAppendConversionEnd(sc, bufferName, &in->data.c[0]);
	sc->tempLen = sprintf(sc->tempStr, ";\n");
	PfAppendLine(sc);
	return;
}
static inline void appendRegistersToGlobal_y(VkFFTSpecializationConstantsLayout* sc, PfContainer* bufferName, PfContainer* inoutID, PfContainer* in)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if ((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "inputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->inputBufferBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->inputBufferBlockSize / (sc->complexSize / 2));
	}
	else if ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "outputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->outputBufferBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->outputBufferBlockSize / (sc->complexSize / 2));
	}
	else if ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "kernelBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->kernelBlockSize / (sc->complexSize / 2), bufferName->name, inoutID->name, sc->kernelBlockSize / (sc->complexSize / 2));
	}
	else {
		sc->tempLen = sprintf(sc->tempStr, "%s[%s]", bufferName->name, inoutID->name);
	}
	PfAppendLine(sc);
	sc->tempLen = sprintf(sc->tempStr, " = ");
	PfAppendLine(sc);
	PfAppendConversionStart(sc, bufferName, &in->data.c[1]);
	sc->tempLen = sprintf(sc->tempStr, "%s", in->data.c[1].name);
	PfAppendLine(sc);
	PfAppendConversionEnd(sc, bufferName, &in->data.c[1]);
	sc->tempLen = sprintf(sc->tempStr, ";\n");
	PfAppendLine(sc);
	return;
}

static inline void appendGlobalToShared(VkFFTSpecializationConstantsLayout* sc, PfContainer* sdataID, PfContainer* bufferName, PfContainer* inoutID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferSeparateComplexComponents)) ||
	 ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferSeparateComplexComponents)) ||
	 ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelSeparateComplexComponents))) {
		PfContainer temp_int = VKFFT_ZERO_INIT;
		temp_int.type = 31;
		sc->sdataStruct.type -= 1;
		bufferName->type -= 1;
		if (sc->storeSharedComplexComponentsSeparately){
			sc->tempLen = sprintf(sc->tempStr, "%s.x", sc->temp.name);
			PfAppendLine(sc);
		}else{
			sc->tempLen = sprintf(sc->tempStr, "sdata[%s].x", sdataID->name);
			PfAppendLine(sc);
		}
		sc->tempLen = sprintf(sc->tempStr, " = ");
		PfAppendLine(sc);
		PfAppendConversionStart(sc, &sc->sdataStruct, bufferName);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
		if(sc->precision != 3){
			sc->tempLen = sprintf(sc->tempStr, "__ldg(&");
			PfAppendLine(sc);
		}
#endif
#if(VKFFT_BACKEND==0)
		sc->tempLen = sprintf(sc->tempStr, "%sBlocks[0].%s[%s +", bufferName->name, bufferName->name, inoutID->name);
#else
		sc->tempLen = sprintf(sc->tempStr, "%s[%s + ", bufferName->name, inoutID->name);
#endif
		PfAppendLine(sc);
		if (!(strcmp(bufferName->name, sc->inputsStruct.name))) {
			if (sc->inputOffset.type < 100) {
				temp_int.data.i = sc->inputOffset.data.i / (sc->inputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->inputOffset.type == 101) {
					if (sc->performPostCompilationInputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->inputOffset.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->outputsStruct.name))) {
			if (sc->outputOffset.type < 100) {
				temp_int.data.i = sc->outputOffset.data.i / (sc->outputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->outputOffset.type == 101) {
					if (sc->performPostCompilationOutputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->outputOffset.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->kernelStruct.name))) {
			if (sc->kernelOffset.type < 100) {
				temp_int.data.i = sc->kernelOffset.data.i / (sc->kernelNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->kernelOffset.type == 101) {
					if (sc->performPostCompilationKernelOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->kernelOffset.name);
						PfAppendLine(sc);
					}
				}
			}
		}
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
		if(sc->precision != 3){
			sc->tempLen = sprintf(sc->tempStr, ")");
			PfAppendLine(sc);
		}
#endif
		PfAppendConversionEnd(sc, &sc->sdataStruct, bufferName);
		sc->tempLen = sprintf(sc->tempStr, ";\n");
		PfAppendLine(sc);

		if (sc->storeSharedComplexComponentsSeparately){
			sc->tempLen = sprintf(sc->tempStr, "%s.y", sc->temp.name);
			PfAppendLine(sc);
		}else{
			sc->tempLen = sprintf(sc->tempStr, "sdata[%s].y", sdataID->name);
			PfAppendLine(sc);
		}
		sc->tempLen = sprintf(sc->tempStr, " = ");
		PfAppendLine(sc);
		PfAppendConversionStart(sc, &sc->sdataStruct, bufferName);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
		if(sc->precision != 3){
			sc->tempLen = sprintf(sc->tempStr, "__ldg(&");
			PfAppendLine(sc);
		}
#endif
#if(VKFFT_BACKEND==0)
		sc->tempLen = sprintf(sc->tempStr, "%sBlocks[1].%s[%s +", bufferName->name, bufferName->name, inoutID->name);
#else
		sc->tempLen = sprintf(sc->tempStr, "%s_imag[%s + ", bufferName->name, inoutID->name);
#endif
		PfAppendLine(sc);
		if (!(strcmp(bufferName->name, sc->inputsStruct.name))) {
			if (sc->inputOffsetImaginary.type < 100) {
				temp_int.data.i = sc->inputOffsetImaginary.data.i / (sc->inputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->inputOffsetImaginary.type == 101) {
					if (sc->performPostCompilationInputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->inputOffsetImaginary.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->outputsStruct.name))) {
			if (sc->outputOffsetImaginary.type < 100) {
				temp_int.data.i = sc->outputOffsetImaginary.data.i / (sc->outputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->outputOffsetImaginary.type == 101) {
					if (sc->performPostCompilationOutputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->outputOffsetImaginary.name);
						PfAppendLine(sc);
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->kernelStruct.name))) {
			if (sc->kernelOffsetImaginary.type < 100) {
				temp_int.data.i = sc->kernelOffsetImaginary.data.i / (sc->kernelNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->kernelOffsetImaginary.type == 101) {
					if (sc->performPostCompilationKernelOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->kernelOffsetImaginary.name);
						PfAppendLine(sc);
					}
				}
			}
		}
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
		if(sc->precision != 3){
			sc->tempLen = sprintf(sc->tempStr, ")");
			PfAppendLine(sc);
		}
#endif
		PfAppendConversionEnd(sc, &sc->sdataStruct, bufferName);
		sc->tempLen = sprintf(sc->tempStr, ";\n");
		PfAppendLine(sc);
		sc->sdataStruct.type += 1;
		bufferName->type += 1;
		if (sc->storeSharedComplexComponentsSeparately){
			appendRegistersToShared(sc, sdataID, &sc->temp);
		}
		return;
	}
	
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "%s", sc->temp.name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "sdata[%s]", sdataID->name);
		PfAppendLine(sc);
	}
	sc->tempLen = sprintf(sc->tempStr, " = ");
	PfAppendLine(sc);
	PfAppendConversionStart(sc, &sc->sdataStruct, bufferName);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
	if(sc->precision != 3){
		sc->tempLen = sprintf(sc->tempStr, "__ldg(&");
		PfAppendLine(sc);
	}
#endif
	if ((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "inputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->inputBufferBlockSize / sc->complexSize, bufferName->name, inoutID->name, sc->inputBufferBlockSize / sc->complexSize);
	}
	else if ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "outputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->outputBufferBlockSize / sc->complexSize, bufferName->name, inoutID->name, sc->outputBufferBlockSize / sc->complexSize);
	}
	else if ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "kernelBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->kernelBlockSize / sc->complexSize, bufferName->name, inoutID->name, sc->kernelBlockSize / sc->complexSize);
	}
	else {
		if (sc->tempBufferInput && sc->optimizePow2StridesTempBuffer && (!(strcmp(bufferName->name, sc->inputsStruct.name))))
			sc->tempLen = sprintf(sc->tempStr, "%s[(%s & %" PRIu64 ") + (%s >> %" PRIu64 ") * %" PRIu64 "]", bufferName->name, inoutID->name, sc->inStridePadTempBuffer-1, inoutID->name, (uint64_t)log2(sc->inStridePadTempBuffer), sc->outStridePadTempBuffer);
		else
			sc->tempLen = sprintf(sc->tempStr, "%s[%s]", bufferName->name, inoutID->name);
	}
	PfAppendLine(sc);
#if ((VKFFT_BACKEND==1) || (VKFFT_BACKEND==2))
	if(sc->precision != 3){
		sc->tempLen = sprintf(sc->tempStr, ")");
		PfAppendLine(sc);
	}
#endif
	PfAppendConversionEnd(sc, &sc->sdataStruct, bufferName);
	sc->tempLen = sprintf(sc->tempStr, ";\n");
	PfAppendLine(sc);

	if (sc->storeSharedComplexComponentsSeparately){
		appendRegistersToShared(sc, sdataID, &sc->temp);
	}
	return;
}
static inline void appendSharedToGlobal(VkFFTSpecializationConstantsLayout* sc, PfContainer* bufferName, PfContainer* inoutID, PfContainer* sdataID)
{
	if (sc->res != VKFFT_SUCCESS) return;
	if (((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferSeparateComplexComponents)) ||
	 ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferSeparateComplexComponents)) ||
	 ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelSeparateComplexComponents))) {
		PfContainer temp_int = VKFFT_ZERO_INIT;
		temp_int.type = 31;
		if (sc->storeSharedComplexComponentsSeparately){
			appendSharedToRegisters(sc, &sc->temp, sdataID);
		}
		sc->sdataStruct.type -= 1;
		bufferName->type -= 1;
#if(VKFFT_BACKEND==0)
		sc->tempLen = sprintf(sc->tempStr, "%sBlocks[0].%s[%s +", bufferName->name, bufferName->name, inoutID->name);
#else
		sc->tempLen = sprintf(sc->tempStr, "%s[%s + ", bufferName->name, inoutID->name);
#endif
		PfAppendLine(sc);
		if (!(strcmp(bufferName->name, sc->inputsStruct.name))) {
			if (sc->inputOffset.type < 100) {
				temp_int.data.i = sc->inputOffset.data.i / (sc->inputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->inputOffset.type == 101) {
					if (sc->performPostCompilationInputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->inputOffset.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->outputsStruct.name))) {
			if (sc->outputOffset.type < 100) {
				temp_int.data.i = sc->outputOffset.data.i / (sc->outputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->outputOffset.type == 101) {
					if (sc->performPostCompilationOutputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->outputOffset.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->kernelStruct.name))) {
			if (sc->kernelOffset.type < 100) {
				temp_int.data.i = sc->kernelOffset.data.i / (sc->kernelNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->kernelOffset.type == 101) {
					if (sc->performPostCompilationKernelOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->kernelOffset.name);
						PfAppendLine(sc);
					}
				}
			}
		}
		sc->tempLen = sprintf(sc->tempStr, " = ");
		PfAppendLine(sc);
		PfAppendConversionStart(sc, bufferName, &sc->sdataStruct);
		if (sc->storeSharedComplexComponentsSeparately){
			sc->tempLen = sprintf(sc->tempStr, "%s.x", sc->temp.name);
			PfAppendLine(sc);
		}else{
			sc->tempLen = sprintf(sc->tempStr, "sdata[%s].x", sdataID->name);
			PfAppendLine(sc);
		}
		PfAppendConversionEnd(sc, bufferName, &sc->sdataStruct);
		sc->tempLen = sprintf(sc->tempStr, ";\n");
		PfAppendLine(sc);
#if(VKFFT_BACKEND==0)
		sc->tempLen = sprintf(sc->tempStr, "%sBlocks[1].%s[%s +", bufferName->name, bufferName->name, inoutID->name);
#else
		sc->tempLen = sprintf(sc->tempStr, "%s_imag[%s + ", bufferName->name, inoutID->name);
#endif
		PfAppendLine(sc);
		if (!(strcmp(bufferName->name, sc->inputsStruct.name))) {
			if (sc->inputOffsetImaginary.type < 100) {
				temp_int.data.i = sc->inputOffsetImaginary.data.i / (sc->inputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->inputOffsetImaginary.type == 101) {
					if (sc->performPostCompilationInputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->inputOffsetImaginary.name);
						PfAppendLine(sc);	
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->outputsStruct.name))) {
			if (sc->outputOffsetImaginary.type < 100) {
				temp_int.data.i = sc->outputOffsetImaginary.data.i / (sc->outputNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->outputOffsetImaginary.type == 101) {
					if (sc->performPostCompilationOutputOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->outputOffsetImaginary.name);
						PfAppendLine(sc);
					}
				}
			}
		}
		else if (!(strcmp(bufferName->name, sc->kernelStruct.name))) {
			if (sc->kernelOffsetImaginary.type < 100) {
				temp_int.data.i = sc->kernelOffsetImaginary.data.i / (sc->kernelNumberByteSize/2);
				sc->tempLen = sprintf(sc->tempStr, "%" PRIi64 "]", temp_int.data.i);
				PfAppendLine(sc);
			}
			else {
				if (sc->kernelOffsetImaginary.type == 101) {
					if (sc->performPostCompilationKernelOffset) {
						sc->tempLen = sprintf(sc->tempStr, "%s]", sc->kernelOffsetImaginary.name);
						PfAppendLine(sc);
					}
				}
			}
		}
		sc->tempLen = sprintf(sc->tempStr, " = ");
		PfAppendLine(sc);
		PfAppendConversionStart(sc, bufferName, &sc->sdataStruct);
		if (sc->storeSharedComplexComponentsSeparately){
			sc->tempLen = sprintf(sc->tempStr, "%s.y", sc->temp.name);
			PfAppendLine(sc);
		}else{
			sc->tempLen = sprintf(sc->tempStr, "sdata[%s].y", sdataID->name);
			PfAppendLine(sc);
		}
		PfAppendConversionEnd(sc, bufferName, &sc->sdataStruct);
		sc->tempLen = sprintf(sc->tempStr, ";\n");
		PfAppendLine(sc);
		sc->sdataStruct.type += 1;
		bufferName->type += 1;
		return;
	}
	if (sc->storeSharedComplexComponentsSeparately){
		appendSharedToRegisters(sc, &sc->temp, sdataID);
	}
	if ((!(strcmp(bufferName->name, sc->inputsStruct.name))) && (sc->inputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "inputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->inputBufferBlockSize / sc->complexSize, bufferName->name, inoutID->name, sc->inputBufferBlockSize / sc->complexSize);
	}
	else if ((!(strcmp(bufferName->name, sc->outputsStruct.name))) && (sc->outputBufferBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "outputBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->outputBufferBlockSize / sc->complexSize, bufferName->name, inoutID->name, sc->outputBufferBlockSize / sc->complexSize);
	}
	else if ((!(strcmp(bufferName->name, sc->kernelStruct.name))) && (sc->kernelBlockNum != 1)) {
		sc->tempLen = sprintf(sc->tempStr, "kernelBlocks[%s / %" PRIu64 "].%s[%s %% %" PRIu64 "]", inoutID->name, sc->kernelBlockSize / sc->complexSize, bufferName->name, inoutID->name, sc->kernelBlockSize / sc->complexSize);
	}
	else {
		if (sc->tempBufferOutput && sc->optimizePow2StridesTempBuffer && (!(strcmp(bufferName->name, sc->outputsStruct.name))))
			sc->tempLen = sprintf(sc->tempStr, "%s[(%s & %" PRIu64 ") + (%s >> %" PRIu64 ") * %" PRIu64 "]", bufferName->name, inoutID->name, sc->inStridePadTempBuffer-1, inoutID->name, (uint64_t)log2(sc->inStridePadTempBuffer), sc->outStridePadTempBuffer);
		else
			sc->tempLen = sprintf(sc->tempStr, "%s[%s]", bufferName->name, inoutID->name);
	}
	PfAppendLine(sc);
	sc->tempLen = sprintf(sc->tempStr, " = ");
	PfAppendLine(sc);
	PfAppendConversionStart(sc, bufferName, &sc->sdataStruct);
	if (sc->storeSharedComplexComponentsSeparately){
		sc->tempLen = sprintf(sc->tempStr, "%s", sc->temp.name);
		PfAppendLine(sc);
	}else{
		sc->tempLen = sprintf(sc->tempStr, "sdata[%s]", sdataID->name);
		PfAppendLine(sc);
	}
	PfAppendConversionEnd(sc, bufferName, &sc->sdataStruct);
	sc->tempLen = sprintf(sc->tempStr, ";\n");
	PfAppendLine(sc);
	return;
}
static inline void appendSetSMToZero(VkFFTSpecializationConstantsLayout* sc) {
	VkFFTResult res = VKFFT_SUCCESS;

	PfContainer temp_int = VKFFT_ZERO_INIT;
	temp_int.type = 31;

	PfContainer temp_int1 = VKFFT_ZERO_INIT;
	temp_int1.type = 31;

	PfContainer used_registers = VKFFT_ZERO_INIT;
	used_registers.type = 31;
	temp_int.data.i = sc->localSize[0].data.i * sc->localSize[1].data.i;
	temp_int1.data.i = sc->usedSharedMemory.data.i / sc->complexSize;
	PfDivCeil(sc, &used_registers, &temp_int1, &temp_int);
	for (pfINT i = 0; i < used_registers.data.i; i++) {
		if (sc->localSize[1].data.i == 1) {
			temp_int.data.i = (i)*sc->localSize[0].data.i;

			PfAdd(sc, &sc->combinedID, &sc->gl_LocalInvocationID_x, &temp_int);
		}
		else {
			PfMul(sc, &sc->combinedID, &sc->localSize[0], &sc->gl_LocalInvocationID_y, 0);

			temp_int.data.i = (i)*sc->localSize[0].data.i * sc->localSize[1].data.i;

			PfAdd(sc, &sc->combinedID, &sc->combinedID, &temp_int);
			PfAdd(sc, &sc->combinedID, &sc->combinedID, &sc->gl_LocalInvocationID_x);
		}

		temp_int.data.i = (i + 1) * sc->localSize[0].data.i * sc->localSize[1].data.i;
		temp_int1.data.i = sc->usedSharedMemory.data.i / sc->complexSize;
		if (temp_int.data.i > temp_int1.data.i) {
			//check that we only read fftDim * local batch data
			//&sc->tempIntLen = sprintf(&sc->tempIntStr, "		if(combinedID < %" PRIu64 "){\n", &sc->fftDim * &sc->localSize[0]);
			PfIf_lt_start(sc, &sc->combinedID, &temp_int1);
		}
	
		PfSetToZeroShared(sc, &sc->combinedID);

		temp_int.data.i = (i + 1) * sc->localSize[0].data.i * sc->localSize[1].data.i;
		temp_int1.data.i = sc->usedSharedMemory.data.i / sc->complexSize;
		if (temp_int.data.i > temp_int1.data.i) {
			//check that we only read fftDim * local batch data
			//&sc->tempIntLen = sprintf(&sc->tempIntStr, "		if(combinedID < %" PRIu64 "){\n", &sc->fftDim * &sc->localSize[0]);
			PfIf_end(sc);
		}
	}


	//res = appendZeropadEnd(sc);
	//if (res != VKFFT_SUCCESS) return res;
	return;
}

#endif
