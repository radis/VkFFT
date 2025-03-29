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
#ifndef VKFFT_RADIXKERNELS_H
#define VKFFT_RADIXKERNELS_H

#include "vkFFT/vkFFT_Structs/vkFFT_Structs.h"
#include "vkFFT/vkFFT_CodeGen/vkFFT_StringManagement/vkFFT_StringManager.h"
#include "vkFFT/vkFFT_CodeGen/vkFFT_MathUtils/vkFFT_MathUtils.h"
#include "vkFFT/vkFFT_CodeGen/vkFFT_KernelsLevel0/vkFFT_MemoryManagement/vkFFT_MemoryTransfers/vkFFT_Transfers.h"
static inline void inlineGeneratedRadixKernelVkFFT(VkFFTSpecializationConstantsLayout* sc, pfINT radix, pfINT stageSize, pfINT stageSizeSum, pfLD stageAngle, PfContainer* regID);

static inline void inlineRadixKernelVkFFT(VkFFTSpecializationConstantsLayout* sc, pfINT radix, pfINT stageSize, pfINT stageSizeSum, pfLD stageAngle, PfContainer* regID) {
	if (sc->res != VKFFT_SUCCESS) return;

	PfContainer temp_complex = VKFFT_ZERO_INIT;
	temp_complex.type = 23;
	PfAllocateContainerFlexible(sc, &temp_complex, 50);
	PfContainer temp_double = VKFFT_ZERO_INIT;
	temp_double.type = 22;
	PfContainer temp_int = VKFFT_ZERO_INIT;
	temp_int.type = 31;
	//sprintf(temp, "loc_0");

	switch (radix) {
	case 2: {
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);
			PfMov(sc, &sc->temp, &regID[1]);
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
				}
				else {
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
				}
			}
			else {
				PfSinCos(sc, &sc->w, &sc->angle);
			}
			PfMul(sc, &sc->temp, &regID[1], &sc->w, 0);
		}

		PfSub(sc, &regID[1], &regID[0], &sc->temp);

		PfAdd(sc, &regID[0], &regID[0], &sc->temp);
		
		break;
	}
	case 3: {

		PfContainer tf[2] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < 2; i++){
			tf[i].type = 22;
		}
		
		tf[0].data.d = pfFPinit("-0.5");
		tf[1].data.d = pfFPinit("-0.8660254037844386467637231707529361834714");

		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);
			PfMov(sc, &sc->locID[2], &regID[2]);
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
				}
				else {
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
				}
			}
			else { 
				temp_double.data.d = pfFPinit("4.0") / 3.0;
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			PfMul(sc, &sc->locID[2], &regID[2], &sc->w, 0);
		}
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			PfMov(sc, &sc->locID[1], &regID[1]);
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_double.data.d = pfFPinit("4.0") / 3.0;
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
					
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("2.0") / 3.0;
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			PfMul(sc, &sc->locID[1], &regID[1], &sc->w, 0);
		}
		
		PfAdd(sc, &regID[1], &sc->locID[1], &sc->locID[2]);
		
		PfSub(sc, &regID[2], &sc->locID[1], &sc->locID[2]);
		
		PfAdd(sc, &sc->locID[0], &regID[0], &regID[1]);
		
		PfFMA(sc, &sc->locID[1], &regID[1], &tf[0], &regID[0]);
		
		PfMul(sc, &sc->locID[2], &regID[2], &tf[1], 0);
		
		PfMov(sc, &regID[0], &sc->locID[0]);
		
		if (stageAngle < 0)
		{
			PfShuffleComplex(sc, &regID[1], &sc->locID[1], &sc->locID[2], &sc->locID[0]);
			
			PfShuffleComplexInv(sc, &regID[2], &sc->locID[1], &sc->locID[2], &sc->locID[0]);
			
		}
		else {
			PfShuffleComplexInv(sc, &regID[1], &sc->locID[1], &sc->locID[2], &sc->locID[0]);
			
			PfShuffleComplex(sc, &regID[2], &sc->locID[1], &sc->locID[2], &sc->locID[0]);
			
		}

		break;
	}
	case 4: {	
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			PfMov(sc, &sc->temp, &regID[2]);
		
			PfSub(sc, &regID[2], &regID[0], &sc->temp);
		
			PfAdd(sc, &regID[0], &regID[0], &sc->temp);
		
			PfMov(sc, &sc->temp, &regID[3]);
		
			PfSub(sc, &regID[3], &regID[1], &sc->temp);
		
			PfAdd(sc, &regID[1], &regID[1], &sc->temp);
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
										
				}
				else {
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
										
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				PfSinCos(sc, &sc->w, &sc->angle);
			}
			PfMul(sc, &sc->temp, &regID[2], &sc->w, 0);
		
			PfSub(sc, &regID[2], &regID[0], &sc->temp);
		
			PfAdd(sc, &regID[0], &regID[0], &sc->temp);
		
			PfMul(sc, &sc->temp, &regID[3], &sc->w, 0);
		
			PfSub(sc, &regID[3], &regID[1], &sc->temp);
		
			PfAdd(sc, &regID[1], &regID[1], &sc->temp);
		}
		
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			PfMov(sc, &sc->temp, &regID[1]);
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
				
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.5");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
				
			}
			PfMul(sc, &sc->temp, &regID[1], &sc->w, 0);
		}
		
		PfSub(sc, &regID[1], &regID[0], &sc->temp);
		
		PfAdd(sc, &regID[0], &regID[0], &sc->temp);
		if (stageSize == 1) {
			if (stageAngle < 0) {
				PfMov(sc, &sc->temp.data.c[0], &regID[3].data.c[1]);

				PfMovNeg(sc, &sc->temp.data.c[1], &regID[3].data.c[0]);
			}
			else {
				PfMovNeg(sc, &sc->temp.data.c[0], &regID[3].data.c[1]);

				PfMov(sc, &sc->temp.data.c[1], &regID[3].data.c[0]);
			}
			//PfMul(sc, &sc->temp, &regID[3], &sc->w, 0);

			PfSub(sc, &regID[3], &regID[2], &sc->temp);

			PfAdd(sc, &regID[2], &regID[2], &sc->temp);
		}
		else {
			if (stageAngle < 0) {
				PfMov(sc, &sc->temp.data.c[0], &sc->w.data.c[0]);

				PfMov(sc, &sc->w.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->w.data.c[1], &sc->temp.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMov(sc, &sc->temp.data.c[0], &sc->w.data.c[0]);

				PfMovNeg(sc, &sc->w.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->w.data.c[1], &sc->temp.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(-w.y, w.x);\n\n", vecType);
			}
			PfMul(sc, &sc->temp, &regID[3], &sc->w, 0);

			PfSub(sc, &regID[3], &regID[2], &sc->temp);

			PfAdd(sc, &regID[2], &regID[2], &sc->temp);
		}
		//PfMov(sc, &sc->temp, &regID[1]);
		//

		pfUINT permute2[4] = { 0,2,1,3 };
		PfPermute(sc, permute2, 4, 1, regID, &sc->temp);
		break;
	}
	case 5: {
		PfContainer tf[5] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < 5; i++){
			tf[i].type = 22;
		}
		tf[0].data.d = pfFPinit("-0.5");
		tf[1].data.d = pfFPinit("1.538841768587626701285145288018455");
		tf[2].data.d = pfFPinit("-0.363271264002680442947733378740309");
		tf[3].data.d = pfFPinit("-0.809016994374947424102293417182819");
		tf[4].data.d = pfFPinit("-0.587785252292473129168705954639073");

		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
					
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
					
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &sc->locID[i], &regID[i], &sc->w, 0);
			}
			
		}
		PfAdd(sc, &regID[1], &sc->locID[1], &sc->locID[4]);
		
		PfAdd(sc, &regID[2], &sc->locID[2], &sc->locID[3]);
		
		PfSub(sc, &regID[3], &sc->locID[2], &sc->locID[3]);
		
		PfSub(sc, &regID[4], &sc->locID[1], &sc->locID[4]);
		
		PfSub(sc, &sc->locID[3], &regID[1], &regID[2]);
		
		PfAdd(sc, &sc->locID[4], &regID[3], &regID[4]);
		
		PfAdd(sc, &sc->locID[0], &regID[0], &regID[1]);
		
		PfAdd(sc, &sc->locID[0], &sc->locID[0], &regID[2]);
		
		PfFMA(sc, &sc->locID[1], &regID[1], &tf[0], &regID[0]);
		
		PfFMA(sc, &sc->locID[2], &regID[2], &tf[0], &regID[0]);
		
		PfMul(sc, &regID[3], &regID[3], &tf[1], &regID[0]);
		
		PfMul(sc, &regID[4], &regID[4], &tf[2], &regID[0]);
		
		PfMul(sc, &sc->locID[3], &sc->locID[3], &tf[3], &regID[0]);
		
		PfMul(sc, &sc->locID[4], &sc->locID[4], &tf[4], &regID[0]);
		
		PfSub(sc, &sc->locID[1], &sc->locID[1], &sc->locID[3]);
		
		PfAdd(sc, &sc->locID[2], &sc->locID[2], &sc->locID[3]);
		
		PfAdd(sc, &sc->locID[3], &regID[3], &sc->locID[4]);
		
		PfAdd(sc, &sc->locID[4], &sc->locID[4], &regID[4]);
		
		PfMov(sc, &regID[0], &sc->locID[0]);
		
		if (stageAngle < 0)
		{
			PfShuffleComplex(sc, &regID[1], &sc->locID[1], &sc->locID[4], &sc->locID[0]);
			
			PfShuffleComplex(sc, &regID[2], &sc->locID[2], &sc->locID[3], &sc->locID[0]);
			
			PfShuffleComplexInv(sc, &regID[3], &sc->locID[2], &sc->locID[3], &sc->locID[0]);
			
			PfShuffleComplexInv(sc, &regID[4], &sc->locID[1], &sc->locID[4], &sc->locID[0]);
			
		}
		else {
			PfShuffleComplexInv(sc, &regID[1], &sc->locID[1], &sc->locID[4], &sc->locID[0]);
			
			PfShuffleComplexInv(sc, &regID[2], &sc->locID[2], &sc->locID[3], &sc->locID[0]);
			
			PfShuffleComplex(sc, &regID[3], &sc->locID[2], &sc->locID[3], &sc->locID[0]);
			
			PfShuffleComplex(sc, &regID[4], &sc->locID[1], &sc->locID[4], &sc->locID[0]);
			
		}

		break;
	}
	case 7: {
		PfContainer tf_x[6] = VKFFT_ZERO_INIT;
		PfContainer tf_y[6] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < 6; i++){
			tf_x[i].type = 22;
			tf_y[i].type = 22;
		}
		
		tf_x[0].data.d = pfFPinit("0.6234898018587335305250048840042398106322747308964021053655");
		tf_x[1].data.d = pfFPinit("-0.222520933956314404288902564496794759466355568764544955311");
		tf_x[2].data.d = pfFPinit("-0.900968867902419126236102319507445051165919162131857150053");
		tf_x[3].data.d = tf_x[0].data.d;
		tf_x[4].data.d = tf_x[1].data.d;
		tf_x[5].data.d = tf_x[2].data.d;
		if (stageAngle < 0) {
			tf_y[0].data.d = pfFPinit("-0.7818314824680298087084445266740577502323345187086875289806");
			tf_y[1].data.d = pfFPinit("0.9749279121818236070181316829939312172327858006199974376480");
			tf_y[2].data.d = pfFPinit("0.4338837391175581204757683328483587546099907277874598764445");
			tf_y[3].data.d = -tf_y[0].data.d;
			tf_y[4].data.d = -tf_y[1].data.d;
			tf_y[5].data.d = -tf_y[2].data.d;
		}
		else {
			tf_y[0].data.d = pfFPinit("0.7818314824680298087084445266740577502323345187086875289806");
			tf_y[1].data.d = pfFPinit("-0.9749279121818236070181316829939312172327858006199974376480");
			tf_y[2].data.d = pfFPinit("-0.4338837391175581204757683328483587546099907277874598764445");
			tf_y[3].data.d = -tf_y[0].data.d;
			tf_y[4].data.d = -tf_y[1].data.d;
			tf_y[5].data.d = -tf_y[2].data.d;
		}
		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
					
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &sc->locID[i], &regID[i], &sc->w, 0);
			}
			
		}
		PfMov(sc, &sc->locID[0], &regID[0]);
		
		pfUINT permute[7] = { 0, 1, 3, 2, 6, 4, 5 };
		PfPermute(sc, permute, 7, 0, 0, &sc->w);
		
		for (pfUINT i = 0; i < 3; i++) {
			PfSub(sc, &regID[i + 4].data.c[0], &sc->locID[i + 1].data.c[0], &sc->locID[i + 4].data.c[0]);
			
			PfAdd(sc, &regID[i + 1].data.c[0], &sc->locID[i + 1].data.c[0], &sc->locID[i + 4].data.c[0]);
			
			PfAdd(sc, &regID[i + 4].data.c[1], &sc->locID[i + 1].data.c[1], &sc->locID[i + 4].data.c[1]);
			
			PfSub(sc, &regID[i + 1].data.c[1], &sc->locID[i + 1].data.c[1], &sc->locID[i + 4].data.c[1]);
			
		}
		for (pfUINT i = 0; i < 3; i++) {
			PfAdd(sc, &regID[0].data.c[0], &regID[0].data.c[0], &regID[i + 1].data.c[0]);
			
			PfAdd(sc, &regID[0].data.c[1], &regID[0].data.c[1], &regID[i + 4].data.c[1]);
			
		}
		for (pfUINT i = 1; i < 4; i++) {
			PfMov(sc, &sc->locID[i], &sc->locID[0]);
			
			
		}
		for (pfUINT i = 4; i < 7; i++) {
			PfSetToZero(sc, &sc->locID[i]);
		}
		for (pfUINT i = 0; i < 3; i++) {
			for (pfUINT j = 0; j < 3; j++) {
				pfUINT id = ((6 - i) + j) % 6;
				PfFMA3_const_w(sc, &sc->locID[j + 1], &sc->locID[j + 4], &regID[i + 1], &tf_x[id], &tf_y[id], &regID[i + 4], &sc->w, &sc->locID[0]);
				
			}
		}
		for (pfUINT i = 1; i < 4; i++) {
			PfSub(sc, &regID[i].data.c[0], &sc->locID[i].data.c[0], &sc->locID[i + 3].data.c[0]);
			
			PfAdd(sc, &regID[i].data.c[1], &sc->locID[i].data.c[1], &sc->locID[i + 3].data.c[1]);
			
		}
		for (pfUINT i = 1; i < 4; i++) {
			PfAdd(sc, &regID[i + 3].data.c[0], &sc->locID[i].data.c[0], &sc->locID[i + 3].data.c[0]);
			
			PfSub(sc, &regID[i + 3].data.c[1], &sc->locID[i].data.c[1], &sc->locID[i + 3].data.c[1]);
			
		}
		pfUINT permute2[7] = { 0, 1, 5, 6, 3, 2, 4 };
		PfPermute(sc, permute2, 7, 1, regID, &sc->w);
		break;
	}
	case 8: {
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 4; i++) {
				PfMov(sc, &sc->temp, &regID[i + 4]);
			
				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
										
				}
				else {
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
										
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				PfSinCos(sc, &sc->w, &sc->angle);
			}
			for (pfUINT i = 0; i < 4; i++) {
				PfMul(sc, &sc->temp, &regID[i + 4], &sc->w, 0);
			
				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 2; i++) {
				PfMov(sc, &sc->temp, &regID[i + 2]);
			
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
					
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.5");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			for (pfUINT i = 0; i < 2; i++) {
				PfMul(sc, &sc->temp, &regID[i + 2], &sc->w, 0);
			
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			for (pfUINT i = 4; i < 6; i++) {
				if (stageAngle < 0) {

					PfMov(sc, &sc->temp.data.c[0], &regID[i + 2].data.c[1]);
					PfMovNeg(sc, &sc->temp.data.c[1], &regID[i + 2].data.c[0]);
				}
				else {

					PfMovNeg(sc, &sc->temp.data.c[0], &regID[i + 2].data.c[1]);
					PfMov(sc, &sc->temp.data.c[1], &regID[i + 2].data.c[0]);
				}
				
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		else {
			if (stageAngle < 0) {

				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {

				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}

			for (pfUINT i = 4; i < 6; i++) {
				PfMul(sc, &sc->temp, &regID[i + 2], &sc->iw, 0);

				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			PfMov(sc, &sc->temp, &regID[1]);
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = 2 * stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = 2 * stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
					
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.25");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			PfMul(sc, &sc->temp, &regID[1], &sc->w, 0);
		}
		
		PfSub(sc, &regID[1], &regID[0], &sc->temp);
		
		PfAdd(sc, &regID[0], &regID[0], &sc->temp);
		
		if (stageSize == 1) {
			if (stageAngle < 0) {
				PfMov(sc, &sc->temp.data.c[0], &regID[3].data.c[1]);

				PfMovNeg(sc, &sc->temp.data.c[1], &regID[3].data.c[0]);
			}
			else {
				PfMovNeg(sc, &sc->temp.data.c[0], &regID[3].data.c[1]);

				PfMov(sc, &sc->temp.data.c[1], &regID[3].data.c[0]);
			}
			
			PfSub(sc, &regID[3], &regID[2], &sc->temp);

			PfAdd(sc, &regID[2], &regID[2], &sc->temp);
		}
		else {
			if (stageAngle < 0) {

				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);


				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}
			PfMul(sc, &sc->temp, &regID[3], &sc->iw, 0);

			PfSub(sc, &regID[3], &regID[2], &sc->temp);

			PfAdd(sc, &regID[2], &regID[2], &sc->temp);
		}
		if (stageAngle < 0) {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("-0.70710678118654752440084436210485");
		}
		else {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("0.70710678118654752440084436210485");
		}
		if (stageSize == 1)
			PfMov(sc, &sc->iw, &temp_complex);
		else
			PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
		PfMul(sc, &sc->temp, &regID[5], &sc->iw, 0);
		
		PfSub(sc, &regID[5], &regID[4], &sc->temp);
		
		PfAdd(sc, &regID[4], &regID[4], &sc->temp);
		
		if (stageAngle < 0) {
			PfMov(sc, &sc->w.data.c[0], &sc->iw.data.c[1]);
			PfMovNeg(sc, &sc->w.data.c[1], &sc->iw.data.c[0]);

			//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(iw.y, -iw.x);\n\n", vecType);
		}
		else {
			PfMovNeg(sc, &sc->w.data.c[0], &sc->iw.data.c[1]);
			PfMov(sc, &sc->w.data.c[1], &sc->iw.data.c[0]);

			//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(-iw.y, iw.x);\n\n", vecType);
		}
		PfMul(sc, &sc->temp, &regID[7], &sc->w, 0);

		PfSub(sc, &regID[7], &regID[6], &sc->temp);

		PfAdd(sc, &regID[6], &regID[6], &sc->temp);

		pfUINT permute2[8] = { 0,4,2,6,1,5,3,7 };
		PfPermute(sc, permute2, 8, 1, regID, &sc->temp);
		
		break;
	}
	case 11: {
		PfContainer tf_x[10] = VKFFT_ZERO_INIT;
		PfContainer tf_y[10] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < 10; i++){
			tf_x[i].type = 22;
			tf_y[i].type = 22;
		}
		
		tf_x[0].data.d = pfFPinit("0.8412535328311811688618116489193677175132924984205378986426");
		tf_x[1].data.d = pfFPinit("-0.959492973614497389890368057066327699062454848422161955044");
		tf_x[2].data.d = pfFPinit("-0.142314838273285140443792668616369668791051361125984328418");
		tf_x[3].data.d = pfFPinit("-0.654860733945285064056925072466293553183791199336928427606");
		tf_x[4].data.d = pfFPinit("0.4154150130018864255292741492296232035240049104645368124262");
		tf_x[5].data.d = tf_x[0].data.d;
		tf_x[6].data.d = tf_x[1].data.d;
		tf_x[7].data.d = tf_x[2].data.d;
		tf_x[8].data.d = tf_x[3].data.d;
		tf_x[9].data.d = tf_x[4].data.d;
		if (stageAngle < 0) {
			tf_y[0].data.d = pfFPinit("-0.5406408174555975821076359543186916954317706078981138400357");
			tf_y[1].data.d = pfFPinit("0.2817325568414296977114179153466168990357778989732668718310");
			tf_y[2].data.d = pfFPinit("-0.9898214418809327323760920377767187873765193719487166878386");
			tf_y[3].data.d = pfFPinit("0.7557495743542582837740358439723444201797174451692235695799");
			tf_y[4].data.d = pfFPinit("0.9096319953545183714117153830790284600602410511946441707561");
			tf_y[5].data.d = -tf_y[0].data.d;
			tf_y[6].data.d = -tf_y[1].data.d;
			tf_y[7].data.d = -tf_y[2].data.d;
			tf_y[8].data.d = -tf_y[3].data.d;
			tf_y[9].data.d = -tf_y[4].data.d;
		}
		else {
			tf_y[0].data.d = pfFPinit("0.5406408174555975821076359543186916954317706078981138400357");
			tf_y[1].data.d = pfFPinit("-0.2817325568414296977114179153466168990357778989732668718310");
			tf_y[2].data.d = pfFPinit("0.9898214418809327323760920377767187873765193719487166878386");
			tf_y[3].data.d = pfFPinit("-0.7557495743542582837740358439723444201797174451692235695799");
			tf_y[4].data.d = pfFPinit("-0.9096319953545183714117153830790284600602410511946441707561");
			tf_y[5].data.d = -tf_y[0].data.d;
			tf_y[6].data.d = -tf_y[1].data.d;
			tf_y[7].data.d = -tf_y[2].data.d;
			tf_y[8].data.d = -tf_y[3].data.d;
			tf_y[9].data.d = -tf_y[4].data.d;
		}
		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
					
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &sc->locID[i], &regID[i], &sc->w, 0);
			}
			
		}
		PfMov(sc, &sc->locID[0], &regID[0]);
		
		pfUINT permute[11] = { 0,1,2,4,8,5,10,9,7,3,6 };
		PfPermute(sc, permute, 11, 0, 0, &sc->w);
		
		for (pfUINT i = 0; i < 5; i++) {
			PfSub(sc, &regID[i + 6].data.c[0], &sc->locID[i + 1].data.c[0], &sc->locID[i + 6].data.c[0]);
			
			PfAdd(sc, &regID[i + 1].data.c[0], &sc->locID[i + 1].data.c[0], &sc->locID[i + 6].data.c[0]);
			
			PfAdd(sc, &regID[i + 6].data.c[1], &sc->locID[i + 1].data.c[1], &sc->locID[i + 6].data.c[1]);
			
			PfSub(sc, &regID[i + 1].data.c[1], &sc->locID[i + 1].data.c[1], &sc->locID[i + 6].data.c[1]);
			
		}
		for (pfUINT i = 0; i < 5; i++) {
			PfAdd(sc, &regID[0].data.c[0], &regID[0].data.c[0], &regID[i + 1].data.c[0]);
			
			PfAdd(sc, &regID[0].data.c[1], &regID[0].data.c[1], &regID[i + 6].data.c[1]);
			
		}
		for (pfUINT i = 1; i < 6; i++) {
			PfMov(sc, &sc->locID[i], &sc->locID[0]);
			
			
		}
		for (pfUINT i = 6; i < 11; i++) {
			PfSetToZero(sc, &sc->locID[i]);
		}
		for (pfUINT i = 0; i < 5; i++) {
			for (pfUINT j = 0; j < 5; j++) {
				pfUINT id = ((10 - i) + j) % 10;
				PfFMA3_const_w(sc, &sc->locID[j + 1], &sc->locID[j + 6], &regID[i + 1], &tf_x[id], &tf_y[id], &regID[i + 6], &sc->w, &sc->locID[0]);
				
			}
		}
		for (pfUINT i = 1; i < 6; i++) {
			PfSub(sc, &regID[i].data.c[0], &sc->locID[i].data.c[0], &sc->locID[i + 5].data.c[0]);
			
			PfAdd(sc, &regID[i].data.c[1], &sc->locID[i].data.c[1], &sc->locID[i + 5].data.c[1]);
			
		}
		for (pfUINT i = 1; i < 6; i++) {
			PfAdd(sc, &regID[i + 5].data.c[0], &sc->locID[i].data.c[0], &sc->locID[i + 5].data.c[0]);
			
			PfSub(sc, &regID[i + 5].data.c[1], &sc->locID[i].data.c[1], &sc->locID[i + 5].data.c[1]);
			
		}

		pfUINT permute2[11] = { 0,1,10,3,9,7,2,4,8,5,6 };
		PfPermute(sc, permute2, 11, 1, regID, &sc->w);
		break;
	}
	case 13: {
		PfContainer tf_x[12] = VKFFT_ZERO_INIT;
		PfContainer tf_y[12] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < 12; i++){
			tf_x[i].type = 22;
			tf_y[i].type = 22;
		}
		
		tf_x[0].data.d = pfFPinit("0.8854560256532098959003755220150988786054984163475349018024");
		tf_x[1].data.d = pfFPinit("-0.970941817426052027156982276293789227249865105739003588587");
		tf_x[2].data.d = pfFPinit("0.1205366802553230533490676874525435822736811592275714047969");
		tf_x[3].data.d = pfFPinit("-0.748510748171101098634630599701351383846451590175826134069");
		tf_x[4].data.d = pfFPinit("-0.354604887042535625969637892600018474316355432113794753421");
		tf_x[5].data.d = pfFPinit("0.5680647467311558025118075591275166245334925524535181694796");
		tf_x[6].data.d = tf_x[0].data.d;
		tf_x[7].data.d = tf_x[1].data.d;
		tf_x[8].data.d = tf_x[2].data.d;
		tf_x[9].data.d = tf_x[3].data.d;
		tf_x[10].data.d = tf_x[4].data.d;
		tf_x[11].data.d = tf_x[5].data.d;
		if (stageAngle < 0) {
			tf_y[0].data.d = pfFPinit("-0.4647231720437685456560153351331047775577358653324689769540");
			tf_y[1].data.d = pfFPinit("0.2393156642875577671487537262602118952031730227383060133551");
			tf_y[2].data.d = pfFPinit("0.9927088740980539928007516494925201793436756329701668557709");
			tf_y[3].data.d = pfFPinit("-0.6631226582407952023767854926667662795247641070441061881807");
			tf_y[4].data.d = pfFPinit("0.9350162426854148234397845998378307290505174695784318706963");
			tf_y[5].data.d = pfFPinit("0.8229838658936563945796174234393819906550676930875738058270");
			tf_y[6].data.d = -tf_y[0].data.d;
			tf_y[7].data.d = -tf_y[1].data.d;
			tf_y[8].data.d = -tf_y[2].data.d;
			tf_y[9].data.d = -tf_y[3].data.d;
			tf_y[10].data.d = -tf_y[4].data.d;
			tf_y[11].data.d = -tf_y[5].data.d;
		}
		else {
			tf_y[0].data.d = pfFPinit("0.4647231720437685456560153351331047775577358653324689769540");
			tf_y[1].data.d = pfFPinit("-0.2393156642875577671487537262602118952031730227383060133551");
			tf_y[2].data.d = pfFPinit("-0.9927088740980539928007516494925201793436756329701668557709");
			tf_y[3].data.d = pfFPinit("0.6631226582407952023767854926667662795247641070441061881807");
			tf_y[4].data.d = pfFPinit("-0.9350162426854148234397845998378307290505174695784318706963");
			tf_y[5].data.d = pfFPinit("-0.8229838658936563945796174234393819906550676930875738058270");
			tf_y[6].data.d = -tf_y[0].data.d;
			tf_y[7].data.d = -tf_y[1].data.d;
			tf_y[8].data.d = -tf_y[2].data.d;
			tf_y[9].data.d = -tf_y[3].data.d;
			tf_y[10].data.d = -tf_y[4].data.d;
			tf_y[11].data.d = -tf_y[5].data.d;
		}
		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &sc->locID[i], &regID[i], &sc->w, 0);
			}
			
		}
		PfMov(sc, &sc->locID[0], &regID[0]);
		
		pfUINT permute[13] = { 0, 1, 2, 4, 8, 3, 6, 12, 11, 9, 5, 10, 7 };
		PfPermute(sc, permute, 13, 0, 0, &sc->w);
		
		for (pfUINT i = 0; i < 6; i++) {
			PfSub(sc, &regID[i + 7].data.c[0], &sc->locID[i + 1].data.c[0], &sc->locID[i + 7].data.c[0]);
			
			PfAdd(sc, &regID[i + 1].data.c[0], &sc->locID[i + 1].data.c[0], &sc->locID[i + 7].data.c[0]);
			
			PfAdd(sc, &regID[i + 7].data.c[1], &sc->locID[i + 1].data.c[1], &sc->locID[i + 7].data.c[1]);
			
			PfSub(sc, &regID[i + 1].data.c[1], &sc->locID[i + 1].data.c[1], &sc->locID[i + 7].data.c[1]);
			
		}
		for (pfUINT i = 0; i < 6; i++) {
			PfAdd(sc, &regID[0].data.c[0], &regID[0].data.c[0], &regID[i + 1].data.c[0]);
			
			PfAdd(sc, &regID[0].data.c[1], &regID[0].data.c[1], &regID[i + 7].data.c[1]);
			
		}
		for (pfUINT i = 1; i < 7; i++) {
			PfMov(sc, &sc->locID[i], &sc->locID[0]);
			
		}
		for (pfUINT i = 7; i < 13; i++) {
			PfSetToZero(sc, &sc->locID[i]);
		}
		for (pfUINT i = 0; i < 6; i++) {
			for (pfUINT j = 0; j < 6; j++) {
				pfUINT id = ((12 - i) + j) % 12;
				PfFMA3_const_w(sc, &sc->locID[j + 1], &sc->locID[j + 7], &regID[i + 1], &tf_x[id], &tf_y[id], &regID[i + 7], &sc->w, &sc->locID[0]);
				
			}
		}
		for (pfUINT i = 1; i < 7; i++) {
			PfSub(sc, &regID[i].data.c[0], &sc->locID[i].data.c[0], &sc->locID[i + 6].data.c[0]);
			
			PfAdd(sc, &regID[i].data.c[1], &sc->locID[i].data.c[1], &sc->locID[i + 6].data.c[1]);
			
		}
		for (pfUINT i = 1; i < 7; i++) {
			PfAdd(sc, &regID[i + 6].data.c[0], &sc->locID[i].data.c[0], &sc->locID[i + 6].data.c[0]);
			
			PfSub(sc, &regID[i + 6].data.c[1], &sc->locID[i].data.c[1], &sc->locID[i + 6].data.c[1]);
			
		}

		pfUINT permute2[13] = { 0,1,12,9,11,4,8,2,10,5,3,6,7 };
		PfPermute(sc, permute2, 13, 1, regID, &sc->w);
		//
		break;
	}
	/*//FFT-Rader kernel for radix-13
	case 13: {
		PfContainer tf[12] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.08333333333333333333333333333333333333333333333333333333333333333");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("0.2562476715829366009586846540617250591441251745816534759684972107");
		tf[1].data.c[1].data.d = pfFPinit("-0.1568913910515846110468327267560032696602126356507376422652278132");
		tf[2].data.c[0].data.d = pfFPinit("0.2582603903117448614204506442845085678525168110539550001050732180");
		tf[2].data.c[1].data.d = pfFPinit("0.1535556855795414001208799594893377343016146669747805618605872236");
		tf[3].data.c[0].data.d = pfFPinit("0.2875703647370015606841927737277266942305008038116643345604914622");
		tf[3].data.c[1].data.d = pfFPinit("-0.08706930057606795250283039746463237130848233812070066033810699591");
		tf[4].data.c[0].data.d = pfFPinit("0.07590298603719386598310289724510354035642837320959385859985980906");
		tf[4].data.c[1].data.d = pfFPinit("0.2907172414708410974096654697695786233019937403926989448899890856");
		tf[5].data.c[0].data.d = pfFPinit("-0.3002386359663326414628846266673815046760064242920422354983989134");
		tf[5].data.c[1].data.d = pfFPinit("0.01159910560576829072165545665408325218982704110520381412632246659");
		tf[6].data.c[0].data.d = pfFPinit("0.3004626062886657744266017722892079955209413811537705177258710880");
		tf[6].data.c[1].data.d = pfFPinit("0.0");
		tf[7].data.c[0].data.d = pfFPinit("0.3002386359663326414628846266673815046760064242920422354983989134");
		tf[7].data.c[1].data.d = pfFPinit("0.01159910560576829072165545665408325218982704110520381412632246659");
		tf[8].data.c[0].data.d = pfFPinit("0.07590298603719386598310289724510354035642837320959385859985980906");
		tf[8].data.c[1].data.d = pfFPinit("-0.2907172414708410974096654697695786233019937403926989448899890856");
		tf[9].data.c[0].data.d = pfFPinit("-0.2875703647370015606841927737277266942305008038116643345604914622");
		tf[9].data.c[1].data.d = pfFPinit("-0.08706930057606795250283039746463237130848233812070066033810699591");
		tf[10].data.c[0].data.d = pfFPinit("0.2582603903117448614204506442845085678525168110539550001050732180");
		tf[10].data.c[1].data.d = pfFPinit("-0.1535556855795414001208799594893377343016146669747805618605872236");
		tf[11].data.c[0].data.d = pfFPinit("-0.2562476715829366009586846540617250591441251745816534759684972107");
		tf[11].data.c[1].data.d = pfFPinit("-0.1568913910515846110468327267560032696602126356507376422652278132");

		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[13] = { 0, 1, 2, 4, 8, 3, 6, 12, 11, 9, 5, 10, 7 };
		PfPermute(sc, permute, radix, 1, regID, &sc->w);
		
		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[13];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}*/
	/*case 16: {
		
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 8; i++) {
				PfMov(sc, &sc->temp, &regID[i + 8]);
			
				PfSub(sc, &regID[i + 8], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
										
				}
				else {
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
										
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				PfSinCos(sc, &sc->w, &sc->angle);
			}
			for (pfUINT i = 0; i < 8; i++) {
				PfMul(sc, &sc->temp, &regID[i + 8], &sc->w, 0);
			
				PfSub(sc, &regID[i + 8], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 4; i++) {
				PfMov(sc, &sc->temp, &regID[i + 4]);
			
				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
					
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.5");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			for (pfUINT i = 0; i < 4; i++) {
				PfMul(sc, &sc->temp, &regID[i + 4], &sc->w, 0);
			
				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			for (pfUINT i = 8; i < 12; i++) {
				if (stageAngle < 0) {
					PfMov(sc, &sc->temp.data.c[0], &regID[i + 4].data.c[1]);
					PfMovNeg(sc, &sc->temp.data.c[1], &regID[i + 4].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
				}
				else {
					PfMovNeg(sc, &sc->temp.data.c[0], &regID[i + 4].data.c[1]);
					PfMov(sc, &sc->temp.data.c[1], &regID[i + 4].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
				}
				//PfMul(sc, &sc->temp, &regID[i + 4], &sc->iw, 0);

				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		else {
			if (stageAngle < 0) {
				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}

			for (pfUINT i = 8; i < 12; i++) {
				PfMul(sc, &sc->temp, &regID[i + 4], &sc->iw, 0);

				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 2; i++) {
				PfMov(sc, &sc->temp, &regID[i + 2]);
			
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = 2 * stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = 2 * stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
					
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.25");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			for (pfUINT i = 0; i < 2; i++) {
				PfMul(sc, &sc->temp, &regID[i + 2], &sc->w, 0);
			
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			for (pfUINT i = 4; i < 6; i++) {
				if (stageAngle < 0) {
					PfMov(sc, &sc->temp.data.c[0], &regID[i + 2].data.c[1]);
					PfMovNeg(sc, &sc->temp.data.c[1], &regID[i + 2].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
				}
				else {
					PfMovNeg(sc, &sc->temp.data.c[0], &regID[i + 2].data.c[1]);
					PfMov(sc, &sc->temp.data.c[1], &regID[i + 2].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
				}

				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		else {
			if (stageAngle < 0) {
				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}
			for (pfUINT i = 4; i < 6; i++) {
				PfMul(sc, &sc->temp, &regID[i + 2], &sc->iw, 0);

				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		if (stageAngle < 0) {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("-0.70710678118654752440084436210485");
		}
		else {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("0.70710678118654752440084436210485");
		}
		if (stageSize == 1)
			PfMov(sc, &sc->iw, &temp_complex);
		else
			PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
		for (pfUINT i = 8; i < 10; i++) {
			PfMul(sc, &sc->temp, &regID[i + 2], &sc->iw, 0);
			
			PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}
		if (stageAngle < 0) {
			PfMov(sc, &sc->w.data.c[0], &sc->iw.data.c[1]);
			PfMovNeg(sc, &sc->w.data.c[1], &sc->iw.data.c[0]);
			
			//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(iw.y, -iw.x);\n\n", vecType);
		}
		else {
			PfMovNeg(sc, &sc->w.data.c[0], &sc->iw.data.c[1]);
			PfMov(sc, &sc->w.data.c[1], &sc->iw.data.c[0]);
			
			//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(-iw.y, iw.x);\n\n", vecType);
		}
		for (pfUINT i = 12; i < 14; i++) {
			PfMul(sc, &sc->temp, &regID[i + 2], &sc->w, 0);
			
			PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}

		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 1; i++) {
				PfMov(sc, &sc->temp, &regID[i + 1]);
			
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = 3 * stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = 3 * stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
					
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.125");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			for (pfUINT i = 0; i < 1; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->w, 0);
			
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			for (pfUINT i = 2; i < 3; i++) {
				if (stageAngle < 0) {
					PfMov(sc, &sc->temp.data.c[0], &regID[i + 1].data.c[1]);
					PfMovNeg(sc, &sc->temp.data.c[1], &regID[i + 1].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
				}
				else {
					PfMovNeg(sc, &sc->temp.data.c[0], &regID[i + 1].data.c[1]);
					PfMov(sc, &sc->temp.data.c[1], &regID[i + 1].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
				}
			
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		else {
			if (stageAngle < 0) {
				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}
			for (pfUINT i = 2; i < 3; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);

				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}

		if (stageAngle < 0) {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("-0.70710678118654752440084436210485");
		}
		else {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("0.70710678118654752440084436210485");
		}
		if (stageSize == 1)
			PfMov(sc, &sc->iw, &temp_complex);
		else
			PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
		for (pfUINT i = 4; i < 5; i++) {
			PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
			
			PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}
		if (stageAngle < 0) {
			PfMov(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
			PfMovNeg(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
			
			PfMov(sc, &sc->iw, &sc->temp);
			
		}
		else {
			PfMovNeg(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
			PfMov(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
			
			PfMov(sc, &sc->iw, &sc->temp);
			
		}
		for (pfUINT i = 6; i < 7; i++) {
			PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
			
			PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}


		for (pfUINT j = 0; j < 2; j++) {
			if (stageAngle < 0) {
				temp_complex.data.c[0].data.d = pfcos((2 * j + 1) * sc->double_PI / 8);
				temp_complex.data.c[1].data.d = -pfsin((2 * j + 1) * sc->double_PI / 8);
			}
			else {
				temp_complex.data.c[0].data.d = pfcos((2 * j + 1) * sc->double_PI / 8);
				temp_complex.data.c[1].data.d = pfsin((2 * j + 1) * sc->double_PI / 8);
			}
			if (stageSize == 1)
				PfMov(sc, &sc->iw, &temp_complex);
			else
				PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
			for (pfUINT i = 8 + 4 * j; i < 9 + 4 * j; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
				
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
				
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
				
			}
			if (stageAngle < 0) {
				PfMov(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
				PfMovNeg(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
				
				PfMov(sc, &sc->iw, &sc->temp);
				
			}
			else {
				PfMovNeg(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
				PfMov(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
				
				PfMov(sc, &sc->iw, &sc->temp);
				
			}
			for (pfUINT i = 10 + 4 * j; i < 11 + 4 * j; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
				
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
				
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
				
			}
		}

		pfUINT permute2[16] = { 0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15 };
		PfPermute(sc, permute2, 16, 1, regID, &sc->temp);
		
		break;
	}*/
	case 17: {
		PfContainer tf[16] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.06250000000000000000000000000000000000000000000000000000000000000");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("-0.1453265187733074097516468827171935684903322586200248349180842300");
		tf[1].data.c[1].data.d = pfFPinit("-0.2128061393410244849188438549870676234306688021082809910346777106");
		tf[2].data.c[0].data.d = pfFPinit("0.1904955880223868152421517134468575441006004620321375708593403789");
		tf[2].data.c[1].data.d = pfFPinit("-0.1735444638817530155255035248184830719965516372631882094421105202");
		tf[3].data.c[0].data.d = pfFPinit("0.03907028461534646656269975604806228244698233263430913024306695534");
		tf[3].data.c[1].data.d = pfFPinit("-0.2547150620989575897116632917769922071835250599156714504853334436");
		tf[4].data.c[0].data.d = pfFPinit("0.1585880964163625577712726917448275596205512896323094267850158363");
		tf[4].data.c[1].data.d = pfFPinit("0.2031158922266657431462639614848582629865562518336561458556844885");
		tf[5].data.c[0].data.d = pfFPinit("-0.05828693741686925402844586595700494410157544475626369021931309982");
		tf[5].data.c[1].data.d = pfFPinit("0.2510157025497846557630303472796305554160440162905917750883230970");
		tf[6].data.c[0].data.d = pfFPinit("0.01955537946287663709121448271990668237429216151518582790486149158");
		tf[6].data.c[1].data.d = pfFPinit("0.2569510403443872080261172638634786334681084864302444403465211723");
		tf[7].data.c[0].data.d = pfFPinit("0.2551833473419941305928030847304800969124722727975519438054872230");
		tf[7].data.c[1].data.d = pfFPinit("0.03588466579662094449511950139257919333589915370033439569888077932");
		tf[8].data.c[0].data.d = pfFPinit("0.2576941016011037843638381159983798140716999515858512771499145983");
		tf[8].data.c[1].data.d = pfFPinit("0.0");
		tf[9].data.c[0].data.d = pfFPinit("-0.2551833473419941305928030847304800969124722727975519438054872230");
		tf[9].data.c[1].data.d = pfFPinit("0.03588466579662094449511950139257919333589915370033439569888077932");
		tf[10].data.c[0].data.d = pfFPinit("0.01955537946287663709121448271990668237429216151518582790486149158");
		tf[10].data.c[1].data.d = pfFPinit("-0.2569510403443872080261172638634786334681084864302444403465211723");
		tf[11].data.c[0].data.d = pfFPinit("0.05828693741686925402844586595700494410157544475626369021931309982");
		tf[11].data.c[1].data.d = pfFPinit("0.2510157025497846557630303472796305554160440162905917750883230970");
		tf[12].data.c[0].data.d = pfFPinit("0.1585880964163625577712726917448275596205512896323094267850158363");
		tf[12].data.c[1].data.d = pfFPinit("-0.2031158922266657431462639614848582629865562518336561458556844885");
		tf[13].data.c[0].data.d = pfFPinit("-0.03907028461534646656269975604806228244698233263430913024306695534");
		tf[13].data.c[1].data.d = pfFPinit("-0.2547150620989575897116632917769922071835250599156714504853334436");
		tf[14].data.c[0].data.d = pfFPinit("0.1904955880223868152421517134468575441006004620321375708593403789");
		tf[14].data.c[1].data.d = pfFPinit("0.1735444638817530155255035248184830719965516372631882094421105202");
		tf[15].data.c[0].data.d = pfFPinit("0.1453265187733074097516468827171935684903322586200248349180842300");
		tf[15].data.c[1].data.d = pfFPinit("-0.2128061393410244849188438549870676234306688021082809910346777106");

		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[17] = { 0, 1, 3, 9, 10, 13, 5, 15, 11, 16, 14, 8, 7, 4, 12, 2, 6};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);
		
		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[17];// = { 0, 1, 3, 16, 5, 12, 2, 6, 7, 15, 14, 10, 4, 13, 8, 11, 9 };
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	case 19: {
		PfContainer tf[18] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.05555555555555555555555555555555555555555555555555555555555555556");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("-0.04756405326107505924263276220002909516743014687702272348157171219");
		tf[1].data.c[1].data.d = pfFPinit("-0.2374439642231816983165810632376976432045578525809827705390750259");
		tf[2].data.c[0].data.d = pfFPinit("0.1627678269602152806683898338050862666397645544926266036853808068");
		tf[2].data.c[1].data.d = pfFPinit("0.1793003341192965845776105320857180618758234030840482978770608970");
		tf[3].data.c[0].data.d = pfFPinit("0.2410016755740243139812786008232119697898944821406635709369962997");
		tf[3].data.c[1].data.d = pfFPinit("0.02366786173600622627040048693178417950505466826374507203500881038");
		tf[4].data.c[0].data.d = pfFPinit("0.1831262468616759429373690457158219231657208343655686930850549854");
		tf[4].data.c[1].data.d = pfFPinit("0.1584511060832283908512710751001771281788450170082280585916664346");
		tf[5].data.c[0].data.d = pfFPinit("-0.1555370104301625640757265795852935850381719351140487032873806545");
		tf[5].data.c[1].data.d = pfFPinit("0.1856076875969567082280945097534371846559161109169882846221647563");
		tf[6].data.c[0].data.d = pfFPinit("-0.07404523574415480007777889307097644018882184675620161668896514793");
		tf[6].data.c[1].data.d = pfFPinit("0.2305629596709638780451476821077284273137115529469193586209962951");
		tf[7].data.c[0].data.d = pfFPinit("0.2418064195141283700598389553995115674601353862224511913187838033");
		tf[7].data.c[1].data.d = pfFPinit("-0.01310079350265982565689523661360569383704381594738471179662354815");
		tf[8].data.c[0].data.d = pfFPinit("0.2288375605503586937596306484703167920730201798961358373762137727");
		tf[8].data.c[1].data.d = pfFPinit("-0.07921708269055919857912128456881778227649224910656449577382617880");
		tf[9].data.c[0].data.d = pfFPinit("0.0");
		tf[9].data.c[1].data.d = pfFPinit("-0.2421610524189263084576101102144230921742779958462469409383524521");
		tf[10].data.c[0].data.d = pfFPinit("0.2288375605503586937596306484703167920730201798961358373762137727");
		tf[10].data.c[1].data.d = pfFPinit("0.07921708269055919857912128456881778227649224910656449577382617880");
		tf[11].data.c[0].data.d = pfFPinit("-0.2418064195141283700598389553995115674601353862224511913187838033");
		tf[11].data.c[1].data.d = pfFPinit("-0.01310079350265982565689523661360569383704381594738471179662354815");
		tf[12].data.c[0].data.d = pfFPinit("-0.07404523574415480007777889307097644018882184675620161668896514793");
		tf[12].data.c[1].data.d = pfFPinit("-0.2305629596709638780451476821077284273137115529469193586209962951");
		tf[13].data.c[0].data.d = pfFPinit("0.1555370104301625640757265795852935850381719351140487032873806545");
		tf[13].data.c[1].data.d = pfFPinit("0.1856076875969567082280945097534371846559161109169882846221647563");
		tf[14].data.c[0].data.d = pfFPinit("0.1831262468616759429373690457158219231657208343655686930850549854");
		tf[14].data.c[1].data.d = pfFPinit("-0.1584511060832283908512710751001771281788450170082280585916664346");
		tf[15].data.c[0].data.d = pfFPinit("-0.2410016755740243139812786008232119697898944821406635709369962997");
		tf[15].data.c[1].data.d = pfFPinit("0.02366786173600622627040048693178417950505466826374507203500881038");
		tf[16].data.c[0].data.d = pfFPinit("0.1627678269602152806683898338050862666397645544926266036853808068");
		tf[16].data.c[1].data.d = pfFPinit("-0.1793003341192965845776105320857180618758234030840482978770608970");
		tf[17].data.c[0].data.d = pfFPinit("0.04756405326107505924263276220002909516743014687702272348157171219");
		tf[17].data.c[1].data.d = pfFPinit("-0.2374439642231816983165810632376976432045578525809827705390750259");
		
		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[19] = { 0, 1, 2, 4, 8, 16, 13, 7, 14, 9, 18, 17, 15, 11, 3, 6, 12, 5, 10};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);

		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[19];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	case 23: {
		PfContainer tf[22] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.04545454545454545454545454545454545454545454545454545454545454546");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("0.03477601508341821046577844996735710475292435268485853197248860110");
		tf[1].data.c[1].data.d = pfFPinit("-0.2152005806961093274470747749483316963420884760509974112616046517");
		tf[2].data.c[0].data.d = pfFPinit("0.02226508934597303421881671776722466331837131146193140736967283787");
		tf[2].data.c[1].data.d = pfFPinit("0.2168523159974101624807219144701563659300868600557722591894303494");
		tf[3].data.c[0].data.d = pfFPinit("0.1385917663704848248899400320272177851214830913738915090815561524");
		tf[3].data.c[1].data.d = pfFPinit("0.1682646232912127757567042969066774235603976936902703699730252108");
		tf[4].data.c[0].data.d = pfFPinit("0.1862823278617129308263396561679882663140057646155177257829816644");
		tf[4].data.c[1].data.d = pfFPinit("0.1132234758495166660604651240019012744103870350745541350112070456");
		tf[5].data.c[0].data.d = pfFPinit("0.2179718680349117775499149986225512970180565005145131542963753776");
		tf[5].data.c[1].data.d = pfFPinit("0.002987624875348083538253394399369549234547523744465758486448735564");
		tf[6].data.c[0].data.d = pfFPinit("0.08320521311002368954247135599129347364244945799247426020782756985");
		tf[6].data.c[1].data.d = pfFPinit("0.2014883462345659715585956056958274396039667884108273033504380655");
		tf[7].data.c[0].data.d = pfFPinit("0.08791546846332311182457602616213547964853762310524274943483995799");
		tf[7].data.c[1].data.d = pfFPinit("-0.1994781480811851550391939327442532608828566526283399844917218592");
		tf[8].data.c[0].data.d = pfFPinit("-0.002567113717680323116391696713903397846719878177058261450566125673");
		tf[8].data.c[1].data.d = pfFPinit("-0.2179772260677369270084752691587679180715808504763753020659465875");
		tf[9].data.c[0].data.d = pfFPinit("-0.01427777472029829740996470838697625001560129949690240872599967969");
		tf[9].data.c[1].data.d = pfFPinit("0.2175242660166014771504067110889503052831957879797544329814045320");
		tf[10].data.c[0].data.d = pfFPinit("0.2150003998011430433091030382012891215221298458710808149503076524");
		tf[10].data.c[1].data.d = pfFPinit("-0.03599290544501018483368659083024343530864346924135005534261908113");
		tf[11].data.c[0].data.d = pfFPinit("0.0");
		tf[11].data.c[1].data.d = pfFPinit("-0.2179923419687599791635199120073951781816685019047331521129685962");
		tf[12].data.c[0].data.d = pfFPinit("0.2150003998011430433091030382012891215221298458710808149503076524");
		tf[12].data.c[1].data.d = pfFPinit("0.03599290544501018483368659083024343530864346924135005534261908113");
		tf[13].data.c[0].data.d = pfFPinit("0.01427777472029829740996470838697625001560129949690240872599967969");
		tf[13].data.c[1].data.d = pfFPinit("0.2175242660166014771504067110889503052831957879797544329814045320");
		tf[14].data.c[0].data.d = pfFPinit("-0.002567113717680323116391696713903397846719878177058261450566125673");
		tf[14].data.c[1].data.d = pfFPinit("0.2179772260677369270084752691587679180715808504763753020659465875");
		tf[15].data.c[0].data.d = pfFPinit("-0.08791546846332311182457602616213547964853762310524274943483995799");
		tf[15].data.c[1].data.d = pfFPinit("-0.1994781480811851550391939327442532608828566526283399844917218592");
		tf[16].data.c[0].data.d = pfFPinit("0.08320521311002368954247135599129347364244945799247426020782756985");
		tf[16].data.c[1].data.d = pfFPinit("-0.2014883462345659715585956056958274396039667884108273033504380655");
		tf[17].data.c[0].data.d = pfFPinit("-0.2179718680349117775499149986225512970180565005145131542963753776");
		tf[17].data.c[1].data.d = pfFPinit("0.002987624875348083538253394399369549234547523744465758486448735564");
		tf[18].data.c[0].data.d = pfFPinit("0.1862823278617129308263396561679882663140057646155177257829816644");
		tf[18].data.c[1].data.d = pfFPinit("-0.1132234758495166660604651240019012744103870350745541350112070456");
		tf[19].data.c[0].data.d = pfFPinit("-0.1385917663704848248899400320272177851214830913738915090815561524");
		tf[19].data.c[1].data.d = pfFPinit("0.1682646232912127757567042969066774235603976936902703699730252108");
		tf[20].data.c[0].data.d = pfFPinit("0.02226508934597303421881671776722466331837131146193140736967283787");
		tf[20].data.c[1].data.d = pfFPinit("-0.2168523159974101624807219144701563659300868600557722591894303494");
		tf[21].data.c[0].data.d = pfFPinit("-0.03477601508341821046577844996735710475292435268485853197248860110");
		tf[21].data.c[1].data.d = pfFPinit("-0.2152005806961093274470747749483316963420884760509974112616046517");
		
		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[23] = { 0, 1, 5, 2, 10, 4, 20, 8, 17, 16, 11, 9, 22, 18, 21, 13, 19, 3, 15, 6, 7, 12, 14};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);

		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[23];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	case 29: {
		PfContainer tf[28] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.03571428571428571428571428571428571428571428571428571428571428571");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("0.1384072942324543430116485311603034003040923058806500953697133869");
		tf[1].data.c[1].data.d = pfFPinit("-0.1335410679215130348103855121200774590322281130899418252279190537");
		tf[2].data.c[0].data.d = pfFPinit("0.1056464887647930866181502795506199897694554664964432187913992312");
		tf[2].data.c[1].data.d = pfFPinit("0.1607128349884905585106498477246229323175664184793369795663470373");
		tf[3].data.c[0].data.d = pfFPinit("0.1465125222211019255691167006935276059006381172269925506340114588");
		tf[3].data.c[1].data.d = pfFPinit("0.1245948504183798979109993851103652610840131086540473765482706762");
		tf[4].data.c[0].data.d = pfFPinit("0.1645296350986088396380704602932000897169966037615406781370608804");
		tf[4].data.c[1].data.d = pfFPinit("0.09959816811912741772524337708092130388537626744990341116271898143");
		tf[5].data.c[0].data.d = pfFPinit("0.1343108109087458370265338422300511503683078497222098576213540549");
		tf[5].data.c[1].data.d = pfFPinit("-0.1376604590701428221945453294651213997338064728890576268542325900");
		tf[6].data.c[0].data.d = pfFPinit("0.06870970049883672020620193147812303167407535339255272894173366514");
		tf[6].data.c[1].data.d = pfFPinit("0.1796351106430129485482198860644797881595938866143928544521191786");
		tf[7].data.c[0].data.d = pfFPinit("-0.03637054170314597490011401877484223686796823033101410746119584590");
		tf[7].data.c[1].data.d = pfFPinit("0.1888570348559647020971073317306513977278814118230234262911395354");
		tf[8].data.c[0].data.d = pfFPinit("-0.1602557856523615289468252117668044942729970144292790464388222930");
		tf[8].data.c[1].data.d = pfFPinit("0.1063385117599061739984075822463936973170490725790492145862232174");
		tf[9].data.c[0].data.d = pfFPinit("0.01761010332544107243868643388836180629002714310469402878439740388");
		tf[9].data.c[1].data.d = pfFPinit("-0.1915193989632241828916758956597939567469993950888568027882428650");
		tf[10].data.c[0].data.d = pfFPinit("0.1878495864355301745504255160324176859363660370395945673934250618");
		tf[10].data.c[1].data.d = pfFPinit("-0.04125928737105895276127641535359424335159164137146429740149495654");
		tf[11].data.c[0].data.d = pfFPinit("-0.1275593521945562702350092312846152101910899650423404027683994768");
		tf[11].data.c[1].data.d = pfFPinit("-0.1439389022678459201799128648554375955992077651791021755375094386");
		tf[12].data.c[0].data.d = pfFPinit("0.04352413829651990612289974707386948814175294190254847677020654037");
		tf[12].data.c[1].data.d = pfFPinit("0.1873377839729955709907456500799008924859417069121819064441095698");
		tf[13].data.c[0].data.d = pfFPinit("-0.04996865155473070218794210832478948285560827698009254300719814809");
		tf[13].data.c[1].data.d = pfFPinit("0.1857227228428693264774361467183758754014459468311819837063632725");
		tf[14].data.c[0].data.d = pfFPinit("0.1923273145405180011160968032692974841533971486301710299171567382");
		tf[14].data.c[1].data.d = pfFPinit("0.0");
		tf[15].data.c[0].data.d = pfFPinit("0.04996865155473070218794210832478948285560827698009254300719814809");
		tf[15].data.c[1].data.d = pfFPinit("0.1857227228428693264774361467183758754014459468311819837063632725");
		tf[16].data.c[0].data.d = pfFPinit("0.04352413829651990612289974707386948814175294190254847677020654037");
		tf[16].data.c[1].data.d = pfFPinit("-0.1873377839729955709907456500799008924859417069121819064441095698");
		tf[17].data.c[0].data.d = pfFPinit("0.1275593521945562702350092312846152101910899650423404027683994768");
		tf[17].data.c[1].data.d = pfFPinit("-0.1439389022678459201799128648554375955992077651791021755375094386");
		tf[18].data.c[0].data.d = pfFPinit("0.1878495864355301745504255160324176859363660370395945673934250618");
		tf[18].data.c[1].data.d = pfFPinit("0.04125928737105895276127641535359424335159164137146429740149495654");
		tf[19].data.c[0].data.d = pfFPinit("-0.01761010332544107243868643388836180629002714310469402878439740388");
		tf[19].data.c[1].data.d = pfFPinit("-0.1915193989632241828916758956597939567469993950888568027882428650");
		tf[20].data.c[0].data.d = pfFPinit("-0.1602557856523615289468252117668044942729970144292790464388222930");
		tf[20].data.c[1].data.d = pfFPinit("-0.1063385117599061739984075822463936973170490725790492145862232174");
		tf[21].data.c[0].data.d = pfFPinit("0.03637054170314597490011401877484223686796823033101410746119584590");
		tf[21].data.c[1].data.d = pfFPinit("0.1888570348559647020971073317306513977278814118230234262911395354");
		tf[22].data.c[0].data.d = pfFPinit("0.06870970049883672020620193147812303167407535339255272894173366514");
		tf[22].data.c[1].data.d = pfFPinit("-0.1796351106430129485482198860644797881595938866143928544521191786");
		tf[23].data.c[0].data.d = pfFPinit("-0.1343108109087458370265338422300511503683078497222098576213540549");
		tf[23].data.c[1].data.d = pfFPinit("-0.1376604590701428221945453294651213997338064728890576268542325900");
		tf[24].data.c[0].data.d = pfFPinit("0.1645296350986088396380704602932000897169966037615406781370608804");
		tf[24].data.c[1].data.d = pfFPinit("-0.09959816811912741772524337708092130388537626744990341116271898143");
		tf[25].data.c[0].data.d = pfFPinit("-0.1465125222211019255691167006935276059006381172269925506340114588");
		tf[25].data.c[1].data.d = pfFPinit("0.1245948504183798979109993851103652610840131086540473765482706762");
		tf[26].data.c[0].data.d = pfFPinit("0.1056464887647930866181502795506199897694554664964432187913992312");
		tf[26].data.c[1].data.d = pfFPinit("-0.1607128349884905585106498477246229323175664184793369795663470373");
		tf[27].data.c[0].data.d = pfFPinit("-0.1384072942324543430116485311603034003040923058806500953697133869");
		tf[27].data.c[1].data.d = pfFPinit("-0.1335410679215130348103855121200774590322281130899418252279190537");

		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[29] = { 0, 1, 2, 4, 8, 16, 3, 6, 12, 24, 19, 9, 18, 7, 14, 28, 27, 25, 21, 13, 26, 23, 17, 5, 10, 20, 11, 22, 15};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);

		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[29];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	case 31: {
		PfContainer tf[30] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.03333333333333333333333333333333333333333333333333333333333333333");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("-0.1845177128303933441540957346049756020013862853395510081989618268");
		tf[1].data.c[1].data.d = pfFPinit("0.01994136645982265449542985383398166278929705823730960940606551001");
		tf[2].data.c[0].data.d = pfFPinit("0.1855916875471966030132064975137330701976620982229427207020003734");
		tf[2].data.c[1].data.d = pfFPinit("-0.0004122594185628719383809984453346998210461704979867086866915629309");
		tf[3].data.c[0].data.d = pfFPinit("0.1556703144637517229352336907226856552685414899204257584911238816");
		tf[3].data.c[1].data.d = pfFPinit("-0.1010504707520014256694450758804488352748055537087081952069289059");
		tf[4].data.c[0].data.d = pfFPinit("0.02886648384729574195497065853456265509236097043717314749645623483");
		tf[4].data.c[1].data.d = pfFPinit("0.1833334954522447828199040550703092129017105582870305088723641863");
		tf[5].data.c[0].data.d = pfFPinit("0.1334262014154941318567496807344701013904382755269775957712392368");
		tf[5].data.c[1].data.d = pfFPinit("-0.1290034620476382260448998571822006943696109698856655214958181321");
		tf[6].data.c[0].data.d = pfFPinit("0.1517472223157759706269839616936809031216861822333577296827060240");
		tf[6].data.c[1].data.d = pfFPinit("-0.1068514153574528357109759482832850670327464742097042099061751544");
		tf[7].data.c[0].data.d = pfFPinit("0.01570800481054565260279272050915134363734104438651311053702537223");
		tf[7].data.c[1].data.d = pfFPinit("0.1849262096873137101094347758378151159852255667004778858503224238");
		tf[8].data.c[0].data.d = pfFPinit("-0.1121720639063589038910721066542293023173780577742953742541776556");
		tf[8].data.c[1].data.d = pfFPinit("0.1478576089466895798523138904375698594473786185122657390904609008");
		tf[9].data.c[0].data.d = pfFPinit("0.06138066971085620890378637704648500194592523253912256834625433580");
		tf[9].data.c[1].data.d = pfFPinit("0.1751481025597800291753600753590093193318963487460103335918876578");
		tf[10].data.c[0].data.d = pfFPinit("0.1708602846384470498932622919205814609614013100543435747047157488");
		tf[10].data.c[1].data.d = pfFPinit("0.07246521632972125137020067361574363353090385646506966292438549105");
		tf[11].data.c[0].data.d = pfFPinit("0.1838457475855493579371667665768212692067383223647942062780147743");
		tf[11].data.c[1].data.d = pfFPinit("0.02540050273429478542845246575478062494755021038089910446401637581");
		tf[12].data.c[0].data.d = pfFPinit("0.1742193117549369021805749036738220462818514019652003337539929477");
		tf[12].data.c[1].data.d = pfFPinit("0.06396933527933949839978677439985701367398356049415888744836891579");
		tf[13].data.c[0].data.d = pfFPinit("-0.02960656119865229799448054219267429518815358277099924621239697471");
		tf[13].data.c[1].data.d = pfFPinit("-0.1832154359720678683631055335771347756616443248976421926555923559");
		tf[14].data.c[0].data.d = pfFPinit("-0.09268128890437945014225631860959880452520606719829448450554383351");
		tf[14].data.c[1].data.d = pfFPinit("0.1607937285203231894591492879813720862751425412959064114590519291");
		tf[15].data.c[0].data.d = pfFPinit("0.0");
		tf[15].data.c[1].data.d = pfFPinit("-0.1855921454276673974039823766306183173492131125856804767989477529");
		tf[16].data.c[0].data.d = pfFPinit("-0.09268128890437945014225631860959880452520606719829448450554383351");
		tf[16].data.c[1].data.d = pfFPinit("-0.1607937285203231894591492879813720862751425412959064114590519291");
		tf[17].data.c[0].data.d = pfFPinit("0.02960656119865229799448054219267429518815358277099924621239697471");
		tf[17].data.c[1].data.d = pfFPinit("-0.1832154359720678683631055335771347756616443248976421926555923559");
		tf[18].data.c[0].data.d = pfFPinit("0.1742193117549369021805749036738220462818514019652003337539929477");
		tf[18].data.c[1].data.d = pfFPinit("-0.06396933527933949839978677439985701367398356049415888744836891579");
		tf[19].data.c[0].data.d = pfFPinit("-0.1838457475855493579371667665768212692067383223647942062780147743");
		tf[19].data.c[1].data.d = pfFPinit("0.02540050273429478542845246575478062494755021038089910446401637581");
		tf[20].data.c[0].data.d = pfFPinit("0.1708602846384470498932622919205814609614013100543435747047157488");
		tf[20].data.c[1].data.d = pfFPinit("-0.07246521632972125137020067361574363353090385646506966292438549105");
		tf[21].data.c[0].data.d = pfFPinit("-0.06138066971085620890378637704648500194592523253912256834625433580");
		tf[21].data.c[1].data.d = pfFPinit("0.1751481025597800291753600753590093193318963487460103335918876578");
		tf[22].data.c[0].data.d = pfFPinit("-0.1121720639063589038910721066542293023173780577742953742541776556");
		tf[22].data.c[1].data.d = pfFPinit("-0.1478576089466895798523138904375698594473786185122657390904609008");
		tf[23].data.c[0].data.d = pfFPinit("-0.01570800481054565260279272050915134363734104438651311053702537223");
		tf[23].data.c[1].data.d = pfFPinit("0.1849262096873137101094347758378151159852255667004778858503224238");
		tf[24].data.c[0].data.d = pfFPinit("0.1517472223157759706269839616936809031216861822333577296827060240");
		tf[24].data.c[1].data.d = pfFPinit("0.1068514153574528357109759482832850670327464742097042099061751544");
		tf[25].data.c[0].data.d = pfFPinit("-0.1334262014154941318567496807344701013904382755269775957712392368");
		tf[25].data.c[1].data.d = pfFPinit("-0.1290034620476382260448998571822006943696109698856655214958181321");
		tf[26].data.c[0].data.d = pfFPinit("0.02886648384729574195497065853456265509236097043717314749645623483");
		tf[26].data.c[1].data.d = pfFPinit("-0.1833334954522447828199040550703092129017105582870305088723641863");
		tf[27].data.c[0].data.d = pfFPinit("-0.1556703144637517229352336907226856552685414899204257584911238816");
		tf[27].data.c[1].data.d = pfFPinit("-0.1010504707520014256694450758804488352748055537087081952069289059");
		tf[28].data.c[0].data.d = pfFPinit("0.1855916875471966030132064975137330701976620982229427207020003734");
		tf[28].data.c[1].data.d = pfFPinit("0.0004122594185628719383809984453346998210461704979867086866915629309");
		tf[29].data.c[0].data.d = pfFPinit("0.1845177128303933441540957346049756020013862853395510081989618268");
		tf[29].data.c[1].data.d = pfFPinit("0.01994136645982265449542985383398166278929705823730960940606551001");

		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[31] = { 0, 1, 3, 9, 27, 19, 26, 16, 17, 20, 29, 25, 13, 8, 24, 10, 30, 28, 22, 4, 12, 5, 15, 14, 11, 2, 6, 18, 23, 7, 21};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);

		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[31];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	case 37: {
		PfContainer tf[36] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.02777777777777777777777777777777777777777777777777777777777777777");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("-0.1450973629381411700107215602557214578716211365724166523553557605");
		tf[1].data.c[1].data.d = pfFPinit("-0.08658024015008690152115557556484192490426002669965430179475038973");
		tf[2].data.c[0].data.d = pfFPinit("0.09605594158960272896288068403363804125700254822749811058475532295");
		tf[2].data.c[1].data.d = pfFPinit("0.1390058948440108833178998769842165305149414250016169836409001778");
		tf[3].data.c[0].data.d = pfFPinit("0.1600486520623147985199591207774845148473942102230037606594533617");
		tf[3].data.c[1].data.d = pfFPinit("-0.05416467196508698239144903390025251479720806490764413421845491626");
		tf[4].data.c[0].data.d = pfFPinit("0.1551758027845641103804948218083780690372852486578833499539675346");
		tf[4].data.c[1].data.d = pfFPinit("0.06685695884659612606337787976734594153578550160174347777618477756");
		tf[5].data.c[0].data.d = pfFPinit("0.1214431769518508235172368513142300729057344808474548634089632008");
		tf[5].data.c[1].data.d = pfFPinit("0.1174773913903898005588485028985285741788683895938650470934380580");
		tf[6].data.c[0].data.d = pfFPinit("-0.03842074127698812436395841740831084432932829848554516970611314547");
		tf[6].data.c[1].data.d = pfFPinit("0.1645394461999192571331265456086904367507052326289297194079833195");
		tf[7].data.c[0].data.d = pfFPinit("0.1268218535813665309350269362045150408687454733204135557415045495");
		tf[7].data.c[1].data.d = pfFPinit("0.1116494521716779939854300784970338055045022102212872876386925532");
		tf[8].data.c[0].data.d = pfFPinit("0.1219872426099098865740155059200152247859537230090945788182549221");
		tf[8].data.c[1].data.d = pfFPinit("0.1169123404798670878129414317787712567039148118808181707720293691");
		tf[9].data.c[0].data.d = pfFPinit("-0.1092151388529488466946826833078955634342012413968572321344222760");
		tf[9].data.c[1].data.d = pfFPinit("-0.1289241488681638960197660993605193944991490239791056180949620430");
		tf[10].data.c[0].data.d = pfFPinit("-0.1124824176398409752154540553377313340634517739700744389471877183");
		tf[10].data.c[1].data.d = pfFPinit("0.1260836565060903305135074395306055733851883502013756713778742673");
		tf[11].data.c[0].data.d = pfFPinit("0.08912255368298480372959918011240469157490971174887662034471275993");
		tf[11].data.c[1].data.d = pfFPinit("-0.1435498280774758521703245500382056157271716853471453958293502419");
		tf[12].data.c[0].data.d = pfFPinit("0.06212270379911496228398561056073161266413258551932888598757602660");
		tf[12].data.c[1].data.d = pfFPinit("-0.1571310039067300369923076870564047261227125553489874280431629353");
		tf[13].data.c[0].data.d = pfFPinit("-0.1650795184197204653203236081753026553900121217074864686108436761");
		tf[13].data.c[1].data.d = pfFPinit("-0.03602964493805835883781082375742749990864828440272824935936940047");
		tf[14].data.c[0].data.d = pfFPinit("0.08569156110403695744162271693626766979275833388407835950793764829");
		tf[14].data.c[1].data.d = pfFPinit("0.1456239646198471109854172738722842977319048051836509490723685116");
		tf[15].data.c[0].data.d = pfFPinit("-0.07973953963135509617844848052786324592687634133726185693460600157");
		tf[15].data.c[1].data.d = pfFPinit("0.1489664006930050407610167513623487537727902617777192287000794677");
		tf[16].data.c[0].data.d = pfFPinit("0.05208393817153496716602488818064763589987786150620051019360683664");
		tf[16].data.c[1].data.d = pfFPinit("0.1607378178917242409172399795346751834517093943277175807123654408");
		tf[17].data.c[0].data.d = pfFPinit("0.1684377940876150455711266302907379313540909140386488000456082797");
		tf[17].data.c[1].data.d = pfFPinit("-0.01334512041712538141130702782887948675008615771618260308916253146");
		tf[18].data.c[0].data.d = pfFPinit("0.1689656258416172135833245623667240850579158359662891996227542513");
		tf[18].data.c[1].data.d = pfFPinit("0.0");
		tf[19].data.c[0].data.d = pfFPinit("-0.1684377940876150455711266302907379313540909140386488000456082797");
		tf[19].data.c[1].data.d = pfFPinit("-0.01334512041712538141130702782887948675008615771618260308916253146");
		tf[20].data.c[0].data.d = pfFPinit("0.05208393817153496716602488818064763589987786150620051019360683664");
		tf[20].data.c[1].data.d = pfFPinit("-0.1607378178917242409172399795346751834517093943277175807123654408");
		tf[21].data.c[0].data.d = pfFPinit("0.07973953963135509617844848052786324592687634133726185693460600157");
		tf[21].data.c[1].data.d = pfFPinit("0.1489664006930050407610167513623487537727902617777192287000794677");
		tf[22].data.c[0].data.d = pfFPinit("0.08569156110403695744162271693626766979275833388407835950793764829");
		tf[22].data.c[1].data.d = pfFPinit("-0.1456239646198471109854172738722842977319048051836509490723685116");
		tf[23].data.c[0].data.d = pfFPinit("0.1650795184197204653203236081753026553900121217074864686108436761");
		tf[23].data.c[1].data.d = pfFPinit("-0.03602964493805835883781082375742749990864828440272824935936940047");
		tf[24].data.c[0].data.d = pfFPinit("0.06212270379911496228398561056073161266413258551932888598757602660");
		tf[24].data.c[1].data.d = pfFPinit("0.1571310039067300369923076870564047261227125553489874280431629353");
		tf[25].data.c[0].data.d = pfFPinit("-0.08912255368298480372959918011240469157490971174887662034471275993");
		tf[25].data.c[1].data.d = pfFPinit("-0.1435498280774758521703245500382056157271716853471453958293502419");
		tf[26].data.c[0].data.d = pfFPinit("-0.1124824176398409752154540553377313340634517739700744389471877183");
		tf[26].data.c[1].data.d = pfFPinit("-0.1260836565060903305135074395306055733851883502013756713778742673");
		tf[27].data.c[0].data.d = pfFPinit("0.1092151388529488466946826833078955634342012413968572321344222760");
		tf[27].data.c[1].data.d = pfFPinit("-0.1289241488681638960197660993605193944991490239791056180949620430");
		tf[28].data.c[0].data.d = pfFPinit("0.1219872426099098865740155059200152247859537230090945788182549221");
		tf[28].data.c[1].data.d = pfFPinit("-0.1169123404798670878129414317787712567039148118808181707720293691");
		tf[29].data.c[0].data.d = pfFPinit("-0.1268218535813665309350269362045150408687454733204135557415045495");
		tf[29].data.c[1].data.d = pfFPinit("0.1116494521716779939854300784970338055045022102212872876386925532");
		tf[30].data.c[0].data.d = pfFPinit("-0.03842074127698812436395841740831084432932829848554516970611314547");
		tf[30].data.c[1].data.d = pfFPinit("-0.1645394461999192571331265456086904367507052326289297194079833195");
		tf[31].data.c[0].data.d = pfFPinit("-0.1214431769518508235172368513142300729057344808474548634089632008");
		tf[31].data.c[1].data.d = pfFPinit("0.1174773913903898005588485028985285741788683895938650470934380580");
		tf[32].data.c[0].data.d = pfFPinit("0.1551758027845641103804948218083780690372852486578833499539675346");
		tf[32].data.c[1].data.d = pfFPinit("-0.06685695884659612606337787976734594153578550160174347777618477756");
		tf[33].data.c[0].data.d = pfFPinit("-0.1600486520623147985199591207774845148473942102230037606594533617");
		tf[33].data.c[1].data.d = pfFPinit("-0.05416467196508698239144903390025251479720806490764413421845491626");
		tf[34].data.c[0].data.d = pfFPinit("0.09605594158960272896288068403363804125700254822749811058475532295");
		tf[34].data.c[1].data.d = pfFPinit("-0.1390058948440108833178998769842165305149414250016169836409001778");
		tf[35].data.c[0].data.d = pfFPinit("0.1450973629381411700107215602557214578716211365724166523553557605");
		tf[35].data.c[1].data.d = pfFPinit("-0.08658024015008690152115557556484192490426002669965430179475038973");

		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[37] = { 0, 1, 2, 4, 8, 16, 32, 27, 17, 34, 31, 25, 13, 26, 15, 30, 23, 9, 18, 36, 35, 33, 29, 21, 5, 10, 20, 3, 6, 12, 24, 11, 22, 7, 14, 28, 19};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);

		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[37];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	case 41: {
		PfContainer tf[40] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.02500000000000000000000000000000000000000000000000000000000000000");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("-0.1310494710592016474934518721118969217686374455643945418132177109");
		tf[1].data.c[1].data.d = pfFPinit("-0.09192951721347975089562840454798657467990918278276257891647905515");
		tf[2].data.c[0].data.d = pfFPinit("0.1499266277877367100106979013929323652926261823369915976839258367");
		tf[2].data.c[1].data.d = pfFPinit("0.05609818428610193964259534043583720264085541051169276888622891228");
		tf[3].data.c[0].data.d = pfFPinit("-0.04328015904849246340773561976026676424623896084721033395151657562");
		tf[3].data.c[1].data.d = pfFPinit("-0.1541162802326126606818010147169622551649847812864905392553193976");
		tf[4].data.c[0].data.d = pfFPinit("0.1587202267279127962227014913521257086802219358359219559971582064");
		tf[4].data.c[1].data.d = pfFPinit("0.02080599979428906032653360372042791402364782065518294217543402791");
		tf[5].data.c[0].data.d = pfFPinit("-0.06417447964190369012561871626309962412114914761120715674131412395");
		tf[5].data.c[1].data.d = pfFPinit("-0.1466514103672067956562462321000493541185722580609800052276151596");
		tf[6].data.c[0].data.d = pfFPinit("0.1598386729243292753393411998578919736231759029674190531662934116");
		tf[6].data.c[1].data.d = pfFPinit("-0.008752064772914299203798290129371465205496944449147848976532262764");
		tf[7].data.c[0].data.d = pfFPinit("-0.01308834202591810366023673379614674611512505473641154429039831917");
		tf[7].data.c[1].data.d = pfFPinit("0.1595421427178806132444380014801136231634388099477567900586778604");
		tf[8].data.c[0].data.d = pfFPinit("0.04460786170103486693680500669832425720149139878408201026245213097");
		tf[8].data.c[1].data.d = pfFPinit("0.1537372390621782524133368663525745762986713904368238282524837017");
		tf[9].data.c[0].data.d = pfFPinit("-0.1577680464151754396299136427072427556103470284279650028420093981");
		tf[9].data.c[1].data.d = pfFPinit("0.02709692842997242948625370175668698257788313276067265728566745243");
		tf[10].data.c[0].data.d = pfFPinit("-0.05298696423660421336256032806944218833666800962749896281466897683");
		tf[10].data.c[1].data.d = pfFPinit("-0.1510542340386022614604358111760599021020558008943597040596189147");
		tf[11].data.c[0].data.d = pfFPinit("0.1558434701372208079085711968434810145136516036487953255666156327");
		tf[11].data.c[1].data.d = pfFPinit("0.03657612357247780105192205668425679570123964864329147543841688509");
		tf[12].data.c[0].data.d = pfFPinit("-0.1355000889476756004767712814055563735406972169579013412983157117");
		tf[12].data.c[1].data.d = pfFPinit("-0.08523336139782356713315965870328687964246558420526838249681752566");
		tf[13].data.c[0].data.d = pfFPinit("-0.08484020640319338644387893417423435549333881947070039914932872229");
		tf[13].data.c[1].data.d = pfFPinit("-0.1357465998744113808161309359888242247134212020258788234311969470");
		tf[14].data.c[0].data.d = pfFPinit("0.04761702852370543425572843031301016686993290096723061142966670753");
		tf[14].data.c[1].data.d = pfFPinit("0.1528319946692204337282358220046174534028048209811400495021043883");
		tf[15].data.c[0].data.d = pfFPinit("0.1596266220917933376355843984356661489070829365760858649928507942");
		tf[15].data.c[1].data.d = pfFPinit("-0.01201422155463245874240243452746219567220430547118122409890061493");
		tf[16].data.c[0].data.d = pfFPinit("0.1593008307870715524939194409722544579156335605128923451208717691");
		tf[16].data.c[1].data.d = pfFPinit("0.01575580244065011806463329840029293481535037230674452052359405611");
		tf[17].data.c[0].data.d = pfFPinit("0.1199877979388597485737650983715399888153152393988328647682661227");
		tf[17].data.c[1].data.d = pfFPinit("0.1059619193190806839297142087091385327765017158712595640632952179");
		tf[18].data.c[0].data.d = pfFPinit("-0.1049230363452471668384898931280203543761335721504111694906825790");
		tf[18].data.c[1].data.d = pfFPinit("-0.1208972970917627341254711092970058713592087198341135374085685137");
		tf[19].data.c[0].data.d = pfFPinit("-0.08609183673679052475643835866733744868286983235654767485366700914");
		tf[19].data.c[1].data.d = pfFPinit("0.1349562730934942698107411498563439943717178367125833127279085088");
		tf[20].data.c[0].data.d = pfFPinit("0.1600781059358212171622054418655453316130105033155254721382318157");
		tf[20].data.c[1].data.d = pfFPinit("0.0");
		tf[21].data.c[0].data.d = pfFPinit("0.08609183673679052475643835866733744868286983235654767485366700914");
		tf[21].data.c[1].data.d = pfFPinit("0.1349562730934942698107411498563439943717178367125833127279085088");
		tf[22].data.c[0].data.d = pfFPinit("-0.1049230363452471668384898931280203543761335721504111694906825790");
		tf[22].data.c[1].data.d = pfFPinit("0.1208972970917627341254711092970058713592087198341135374085685137");
		tf[23].data.c[0].data.d = pfFPinit("-0.1199877979388597485737650983715399888153152393988328647682661227");
		tf[23].data.c[1].data.d = pfFPinit("0.1059619193190806839297142087091385327765017158712595640632952179");
		tf[24].data.c[0].data.d = pfFPinit("0.1593008307870715524939194409722544579156335605128923451208717691");
		tf[24].data.c[1].data.d = pfFPinit("-0.01575580244065011806463329840029293481535037230674452052359405611");
		tf[25].data.c[0].data.d = pfFPinit("-0.1596266220917933376355843984356661489070829365760858649928507942");
		tf[25].data.c[1].data.d = pfFPinit("-0.01201422155463245874240243452746219567220430547118122409890061493");
		tf[26].data.c[0].data.d = pfFPinit("0.04761702852370543425572843031301016686993290096723061142966670753");
		tf[26].data.c[1].data.d = pfFPinit("-0.1528319946692204337282358220046174534028048209811400495021043883");
		tf[27].data.c[0].data.d = pfFPinit("0.08484020640319338644387893417423435549333881947070039914932872229");
		tf[27].data.c[1].data.d = pfFPinit("-0.1357465998744113808161309359888242247134212020258788234311969470");
		tf[28].data.c[0].data.d = pfFPinit("-0.1355000889476756004767712814055563735406972169579013412983157117");
		tf[28].data.c[1].data.d = pfFPinit("0.08523336139782356713315965870328687964246558420526838249681752566");
		tf[29].data.c[0].data.d = pfFPinit("-0.1558434701372208079085711968434810145136516036487953255666156327");
		tf[29].data.c[1].data.d = pfFPinit("0.03657612357247780105192205668425679570123964864329147543841688509");
		tf[30].data.c[0].data.d = pfFPinit("-0.05298696423660421336256032806944218833666800962749896281466897683");
		tf[30].data.c[1].data.d = pfFPinit("0.1510542340386022614604358111760599021020558008943597040596189147");
		tf[31].data.c[0].data.d = pfFPinit("0.1577680464151754396299136427072427556103470284279650028420093981");
		tf[31].data.c[1].data.d = pfFPinit("0.02709692842997242948625370175668698257788313276067265728566745243");
		tf[32].data.c[0].data.d = pfFPinit("0.04460786170103486693680500669832425720149139878408201026245213097");
		tf[32].data.c[1].data.d = pfFPinit("-0.1537372390621782524133368663525745762986713904368238282524837017");
		tf[33].data.c[0].data.d = pfFPinit("0.01308834202591810366023673379614674611512505473641154429039831917");
		tf[33].data.c[1].data.d = pfFPinit("0.1595421427178806132444380014801136231634388099477567900586778604");
		tf[34].data.c[0].data.d = pfFPinit("0.1598386729243292753393411998578919736231759029674190531662934116");
		tf[34].data.c[1].data.d = pfFPinit("0.008752064772914299203798290129371465205496944449147848976532262764");
		tf[35].data.c[0].data.d = pfFPinit("0.06417447964190369012561871626309962412114914761120715674131412395");
		tf[35].data.c[1].data.d = pfFPinit("-0.1466514103672067956562462321000493541185722580609800052276151596");
		tf[36].data.c[0].data.d = pfFPinit("0.1587202267279127962227014913521257086802219358359219559971582064");
		tf[36].data.c[1].data.d = pfFPinit("-0.02080599979428906032653360372042791402364782065518294217543402791");
		tf[37].data.c[0].data.d = pfFPinit("0.04328015904849246340773561976026676424623896084721033395151657562");
		tf[37].data.c[1].data.d = pfFPinit("-0.1541162802326126606818010147169622551649847812864905392553193976");
		tf[38].data.c[0].data.d = pfFPinit("0.1499266277877367100106979013929323652926261823369915976839258367");
		tf[38].data.c[1].data.d = pfFPinit("-0.05609818428610193964259534043583720264085541051169276888622891228");
		tf[39].data.c[0].data.d = pfFPinit("0.1310494710592016474934518721118969217686374455643945418132177109");
		tf[39].data.c[1].data.d = pfFPinit("-0.09192951721347975089562840454798657467990918278276257891647905515");
		
		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[41] = { 0, 1, 6, 36, 11, 25, 27, 39, 29, 10, 19, 32, 28, 4, 24, 21, 3, 18, 26, 33, 34, 40, 35, 5, 30, 16, 14, 2, 12, 31, 22, 9, 13, 37, 17, 20, 38, 23, 15, 8, 7};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);

		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[41];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	case 43: {
		PfContainer tf[42] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.02380952380952380952380952380952380952380952380952380952380952381");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("-0.1506188741091256895138811027678574658336276077339937522744077869");
		tf[1].data.c[1].data.d = pfFPinit("0.04111413377002411968645339859976159309499826044157289311839002061");
		tf[2].data.c[0].data.d = pfFPinit("0.01076529731603380954569643670075016044848281901641918093741407827");
		tf[2].data.c[1].data.d = pfFPinit("0.1557579070457018113288467180174521750584535241782063758913223371");
		tf[3].data.c[0].data.d = pfFPinit("-0.1006242930519696868138600623314118167884321303258978845786456709");
		tf[3].data.c[1].data.d = pfFPinit("-0.1193782596679622219483083798418784323982559065093302144699670206");
		tf[4].data.c[0].data.d = pfFPinit("0.1153802881446594195795112196835607845281768685636942164684833455");
		tf[4].data.c[1].data.d = pfFPinit("0.1051846297764813910837887757338138848650749307026993820249966288");
		tf[5].data.c[0].data.d = pfFPinit("0.1325584942835216726377146244892920219585855389733849228741146863");
		tf[5].data.c[1].data.d = pfFPinit("0.08249038020791056300688388325832462121559115266468321151390440133");
		tf[6].data.c[0].data.d = pfFPinit("0.1226607223577406895862515892575482248602725243460984381130792080");
		tf[6].data.c[1].data.d = pfFPinit("-0.09659588202525687392903836581106992496243907381129066159224991279");
		tf[7].data.c[0].data.d = pfFPinit("0.1555493909256335667461559669587461437679085898325185434568200020");
		tf[7].data.c[1].data.d = pfFPinit("-0.01344634583165688567114258272296776616828212174697336546506106169");
		tf[8].data.c[0].data.d = pfFPinit("0.02457563870799938304727279248806198874313804084249719892830421665");
		tf[8].data.c[1].data.d = pfFPinit("0.1541831872016335415256476968225569291580188148146333364612838401");
		tf[9].data.c[0].data.d = pfFPinit("0.1470549867088295790698165518244410383609872179958125262029957286");
		tf[9].data.c[1].data.d = pfFPinit("0.05245234139317373263595000939602431492019864715413018046486230491");
		tf[10].data.c[0].data.d = pfFPinit("0.01850141626763959202305904172968074263193594279889303367921888366");
		tf[10].data.c[1].data.d = pfFPinit("0.1550293998880586900246283732650470552332895158191653540053490289");
		tf[11].data.c[0].data.d = pfFPinit("0.1349073404183188054589055082740592398188343707110174736453220241");
		tf[11].data.c[1].data.d = pfFPinit("0.07859024579943706916794032376024646413883727797944795117593883026");
		tf[12].data.c[0].data.d = pfFPinit("0.1543410865045664206819373114302782176248279748236429385544041051");
		tf[12].data.c[1].data.d = pfFPinit("0.02356366376754760507449789129709667824852494336133095846303300295");
		tf[13].data.c[0].data.d = pfFPinit("-0.02491976552660817575435962289057265313051696993885104245572175567");
		tf[13].data.c[1].data.d = pfFPinit("-0.1541279420470505129097981636994013772863828405936702731879276187");
		tf[14].data.c[0].data.d = pfFPinit("0.1150560479348837421367359576019533605044270289859068442520318726");
		tf[14].data.c[1].data.d = pfFPinit("0.1055392015658912651758040358968075893272594161463670635145885774");
		tf[15].data.c[0].data.d = pfFPinit("-0.07863465589663748430872648900601833379334931342807811389627410222");
		tf[15].data.c[1].data.d = pfFPinit("0.1348814595323519859694770526880256365681505510160472938268905150");
		tf[16].data.c[0].data.d = pfFPinit("-0.1521370613015365700268202828330244823746930532309789793290098964");
		tf[16].data.c[1].data.d = pfFPinit("-0.03508178747003342924477963626869561032513788768311548066521323652");
		tf[17].data.c[0].data.d = pfFPinit("0.03469196535152666676047283123245900199837526345025388267677752306");
		tf[17].data.c[1].data.d = pfFPinit("-0.1522264260028742970676018271902822380999608755863322015540599404");
		tf[18].data.c[0].data.d = pfFPinit("0.09322907472944342076142859308144661998186788118776434312906492436");
		tf[18].data.c[1].data.d = pfFPinit("-0.1252387993341198750766658401578698096438821569877714137972325422");
		tf[19].data.c[0].data.d = pfFPinit("-0.1461533676350763357791196551116614129533272289007201451589449740");
		tf[19].data.c[1].data.d = pfFPinit("0.05491457331607259012504931549865598818651979836294172930315500694");
		tf[20].data.c[0].data.d = pfFPinit("0.004203935280887096323008884320673831973704512589594395107810938712");
		tf[20].data.c[1].data.d = pfFPinit("-0.1560728809297573113568490893308539784491429003686565664718817859");
		tf[21].data.c[0].data.d = pfFPinit("0.0");
		tf[21].data.c[1].data.d = pfFPinit("-0.1561294886738571583891454761341905149506420552353283278539395263");
		tf[22].data.c[0].data.d = pfFPinit("0.004203935280887096323008884320673831973704512589594395107810938712");
		tf[22].data.c[1].data.d = pfFPinit("0.1560728809297573113568490893308539784491429003686565664718817859");
		tf[23].data.c[0].data.d = pfFPinit("0.1461533676350763357791196551116614129533272289007201451589449740");
		tf[23].data.c[1].data.d = pfFPinit("0.05491457331607259012504931549865598818651979836294172930315500694");
		tf[24].data.c[0].data.d = pfFPinit("0.09322907472944342076142859308144661998186788118776434312906492436");
		tf[24].data.c[1].data.d = pfFPinit("0.1252387993341198750766658401578698096438821569877714137972325422");
		tf[25].data.c[0].data.d = pfFPinit("-0.03469196535152666676047283123245900199837526345025388267677752306");
		tf[25].data.c[1].data.d = pfFPinit("-0.1522264260028742970676018271902822380999608755863322015540599404");
		tf[26].data.c[0].data.d = pfFPinit("-0.1521370613015365700268202828330244823746930532309789793290098964");
		tf[26].data.c[1].data.d = pfFPinit("0.03508178747003342924477963626869561032513788768311548066521323652");
		tf[27].data.c[0].data.d = pfFPinit("0.07863465589663748430872648900601833379334931342807811389627410222");
		tf[27].data.c[1].data.d = pfFPinit("0.1348814595323519859694770526880256365681505510160472938268905150");
		tf[28].data.c[0].data.d = pfFPinit("0.1150560479348837421367359576019533605044270289859068442520318726");
		tf[28].data.c[1].data.d = pfFPinit("-0.1055392015658912651758040358968075893272594161463670635145885774");
		tf[29].data.c[0].data.d = pfFPinit("0.02491976552660817575435962289057265313051696993885104245572175567");
		tf[29].data.c[1].data.d = pfFPinit("-0.1541279420470505129097981636994013772863828405936702731879276187");
		tf[30].data.c[0].data.d = pfFPinit("0.1543410865045664206819373114302782176248279748236429385544041051");
		tf[30].data.c[1].data.d = pfFPinit("-0.02356366376754760507449789129709667824852494336133095846303300295");
		tf[31].data.c[0].data.d = pfFPinit("-0.1349073404183188054589055082740592398188343707110174736453220241");
		tf[31].data.c[1].data.d = pfFPinit("0.07859024579943706916794032376024646413883727797944795117593883026");
		tf[32].data.c[0].data.d = pfFPinit("0.01850141626763959202305904172968074263193594279889303367921888366");
		tf[32].data.c[1].data.d = pfFPinit("-0.1550293998880586900246283732650470552332895158191653540053490289");
		tf[33].data.c[0].data.d = pfFPinit("-0.1470549867088295790698165518244410383609872179958125262029957286");
		tf[33].data.c[1].data.d = pfFPinit("0.05245234139317373263595000939602431492019864715413018046486230491");
		tf[34].data.c[0].data.d = pfFPinit("0.02457563870799938304727279248806198874313804084249719892830421665");
		tf[34].data.c[1].data.d = pfFPinit("-0.1541831872016335415256476968225569291580188148146333364612838401");
		tf[35].data.c[0].data.d = pfFPinit("-0.1555493909256335667461559669587461437679085898325185434568200020");
		tf[35].data.c[1].data.d = pfFPinit("-0.01344634583165688567114258272296776616828212174697336546506106169");
		tf[36].data.c[0].data.d = pfFPinit("0.1226607223577406895862515892575482248602725243460984381130792080");
		tf[36].data.c[1].data.d = pfFPinit("0.09659588202525687392903836581106992496243907381129066159224991279");
		tf[37].data.c[0].data.d = pfFPinit("-0.1325584942835216726377146244892920219585855389733849228741146863");
		tf[37].data.c[1].data.d = pfFPinit("0.08249038020791056300688388325832462121559115266468321151390440133");
		tf[38].data.c[0].data.d = pfFPinit("0.1153802881446594195795112196835607845281768685636942164684833455");
		tf[38].data.c[1].data.d = pfFPinit("-0.1051846297764813910837887757338138848650749307026993820249966288");
		tf[39].data.c[0].data.d = pfFPinit("0.1006242930519696868138600623314118167884321303258978845786456709");
		tf[39].data.c[1].data.d = pfFPinit("-0.1193782596679622219483083798418784323982559065093302144699670206");
		tf[40].data.c[0].data.d = pfFPinit("0.01076529731603380954569643670075016044848281901641918093741407827");
		tf[40].data.c[1].data.d = pfFPinit("-0.1557579070457018113288467180174521750584535241782063758913223371");
		tf[41].data.c[0].data.d = pfFPinit("0.1506188741091256895138811027678574658336276077339937522744077869");
		tf[41].data.c[1].data.d = pfFPinit("0.04111413377002411968645339859976159309499826044157289311839002061");

		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[43] = { 0, 1, 3, 9, 27, 38, 28, 41, 37, 25, 32, 10, 30, 4, 12, 36, 22, 23, 26, 35, 19, 14, 42, 40, 34, 16, 5, 15, 2, 6, 18, 11, 33, 13, 39, 31, 7, 21, 20, 17, 8, 24, 29};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);

		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[43];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	case 47: {
		PfContainer tf[46] = VKFFT_ZERO_INIT;
		for (pfINT i = 0; i < radix-1; i++){
			tf[i].type = 23;
			PfAllocateContainerFlexible(sc, &tf[i], 50);
		}
		
		tf[0].data.c[0].data.d = pfFPinit("-0.02173913043478260869565217391304347826086956521739130434782608696");
		tf[0].data.c[1].data.d = pfFPinit("0.0");
		tf[1].data.c[0].data.d = pfFPinit("0.1440217537342382408054304926674723662184275053647204090602552722");
		tf[1].data.c[1].data.d = pfFPinit("-0.03833346681631513391837907054525246096404414686166602001126863432");
		tf[2].data.c[0].data.d = pfFPinit("-0.004416011885642990456100351068043850683619134358764061250364805648");
		tf[2].data.c[1].data.d = pfFPinit("-0.1489705308638891842652161446123306383600799092822704576292561655");
		tf[3].data.c[0].data.d = pfFPinit("0.04482474129524076051134662350013059188160098171893019382040360940");
		tf[3].data.c[1].data.d = pfFPinit("0.1421353678528248571990892779732841300188956948706879764668083717");
		tf[4].data.c[0].data.d = pfFPinit("0.02577385104501349096762173438928586633892063973860283760530717183");
		tf[4].data.c[1].data.d = pfFPinit("-0.1467904248551401842035905250956003772891191124189771645455941252");
		tf[5].data.c[0].data.d = pfFPinit("0.07022065456636491404759541228086267646589270324078022221749184196");
		tf[5].data.c[1].data.d = pfFPinit("-0.1314563802145576901177402900333653308445350192745368738733199002");
		tf[6].data.c[0].data.d = pfFPinit("0.07874152000761948864688655131831991551391776381434960280298420014");
		tf[6].data.c[1].data.d = pfFPinit("0.1265365293254590571537531335983682133007515952402222618515115748");
		tf[7].data.c[0].data.d = pfFPinit("-0.09755908731550658785219769352681082179431803532212897643415251821");
		tf[7].data.c[1].data.d = pfFPinit("-0.1126674074833022053539365306335596774305253173354969427588185005");
		tf[8].data.c[0].data.d = pfFPinit("-0.03233585554119885420518793372549388375473766757255267740938330891");
		tf[8].data.c[1].data.d = pfFPinit("0.1454857816876337216480605322462116769243448926384000293355163381");
		tf[9].data.c[0].data.d = pfFPinit("0.08519630757797469832137337620635968175755346296937346604193016518");
		tf[9].data.c[1].data.d = pfFPinit("-0.1222837250083682688848042344088660237905799201253720542051993260");
		tf[10].data.c[0].data.d = pfFPinit("0.1490274895206284836591621817667028930766750604547090209489099238");
		tf[10].data.c[1].data.d = pfFPinit("-0.001589840879480608517465431678713987204348120811937896861283369944");
		tf[11].data.c[0].data.d = pfFPinit("-0.1490350950131748690343833248023213110809000650500831289745974611");
		tf[11].data.c[1].data.d = pfFPinit("0.0005105695418250804339547702683307201527363318466451417481683222836");
		tf[12].data.c[0].data.d = pfFPinit("0.1134938767536597973969775033641377376100170914945565997987116567");
		tf[12].data.c[1].data.d = pfFPinit("0.09659637760427758626995943264154250783896765616223889179845718443");
		tf[13].data.c[0].data.d = pfFPinit("0.1373634688090753298265136942124946720490817828283350933686924688");
		tf[13].data.c[1].data.d = pfFPinit("-0.05781866189718756521410952030221747952336821446659601843182775763");
		tf[14].data.c[0].data.d = pfFPinit("0.08948290984489029073182241235269929003634002152196719354251872989");
		tf[14].data.c[1].data.d = pfFPinit("0.1191827549292863781724020412260252122342094361739301228725974927");
		tf[15].data.c[0].data.d = pfFPinit("0.1312719749928518421913007210270854144216616985897705109708773591");
		tf[15].data.c[1].data.d = pfFPinit("0.07056478447723893371194244623017389868736829092849338539445159580");
		tf[16].data.c[0].data.d = pfFPinit("0.03051828801759847739756390654082321377456808392472560482477928338");
		tf[16].data.c[1].data.d = pfFPinit("0.1458778746874179181999256602777488042805098051050455661577874733");
		tf[17].data.c[0].data.d = pfFPinit("-0.1489574342827943255749534481689318952533682726694365508751702560");
		tf[17].data.c[1].data.d = pfFPinit("0.004837664594627239579934817606022251898633648810815399073307764803");
		tf[18].data.c[0].data.d = pfFPinit("0.1488845310498856172845690943852182201511941921462813143371570306");
		tf[18].data.c[1].data.d = pfFPinit("-0.006716892205383805355788674332839797063640466958080276785786431514");
		tf[19].data.c[0].data.d = pfFPinit("-0.1042456647497833398521164472790054171349383537714711664249665664");
		tf[19].data.c[1].data.d = pfFPinit("0.1065108520655002524274592007418152314426178947107235560783622104");
		tf[20].data.c[0].data.d = pfFPinit("-0.09657899737856242868554674166627152402550244235578920208596441241");
		tf[20].data.c[1].data.d = pfFPinit("-0.1135086670355824930786651964854386545008375482475203724573706630");
		tf[21].data.c[0].data.d = pfFPinit("-0.03053245589362907650070066445315420527109157169453345998548562093");
		tf[21].data.c[1].data.d = pfFPinit("0.1458749099877929496594567817279642530195693341811219950538930989");
		tf[22].data.c[0].data.d = pfFPinit("0.003816707860889981063909169740892950589347453706230633120111451355");
		tf[22].data.c[1].data.d = pfFPinit("0.1489870899371744921323194302879471191586795591260007664353192194");
		tf[23].data.c[0].data.d = pfFPinit("0.0");
		tf[23].data.c[1].data.d = pfFPinit("-0.1490359695739357418464319880235836730534922491522027451192414823");
		tf[24].data.c[0].data.d = pfFPinit("0.003816707860889981063909169740892950589347453706230633120111451355");
		tf[24].data.c[1].data.d = pfFPinit("-0.1489870899371744921323194302879471191586795591260007664353192194");
		tf[25].data.c[0].data.d = pfFPinit("0.03053245589362907650070066445315420527109157169453345998548562093");
		tf[25].data.c[1].data.d = pfFPinit("0.1458749099877929496594567817279642530195693341811219950538930989");
		tf[26].data.c[0].data.d = pfFPinit("-0.09657899737856242868554674166627152402550244235578920208596441241");
		tf[26].data.c[1].data.d = pfFPinit("0.1135086670355824930786651964854386545008375482475203724573706630");
		tf[27].data.c[0].data.d = pfFPinit("0.1042456647497833398521164472790054171349383537714711664249665664");
		tf[27].data.c[1].data.d = pfFPinit("0.1065108520655002524274592007418152314426178947107235560783622104");
		tf[28].data.c[0].data.d = pfFPinit("0.1488845310498856172845690943852182201511941921462813143371570306");
		tf[28].data.c[1].data.d = pfFPinit("0.006716892205383805355788674332839797063640466958080276785786431514");
		tf[29].data.c[0].data.d = pfFPinit("0.1489574342827943255749534481689318952533682726694365508751702560");
		tf[29].data.c[1].data.d = pfFPinit("0.004837664594627239579934817606022251898633648810815399073307764803");
		tf[30].data.c[0].data.d = pfFPinit("0.03051828801759847739756390654082321377456808392472560482477928338");
		tf[30].data.c[1].data.d = pfFPinit("-0.1458778746874179181999256602777488042805098051050455661577874733");
		tf[31].data.c[0].data.d = pfFPinit("-0.1312719749928518421913007210270854144216616985897705109708773591");
		tf[31].data.c[1].data.d = pfFPinit("0.07056478447723893371194244623017389868736829092849338539445159580");
		tf[32].data.c[0].data.d = pfFPinit("0.08948290984489029073182241235269929003634002152196719354251872989");
		tf[32].data.c[1].data.d = pfFPinit("-0.1191827549292863781724020412260252122342094361739301228725974927");
		tf[33].data.c[0].data.d = pfFPinit("-0.1373634688090753298265136942124946720490817828283350933686924688");
		tf[33].data.c[1].data.d = pfFPinit("-0.05781866189718756521410952030221747952336821446659601843182775763");
		tf[34].data.c[0].data.d = pfFPinit("0.1134938767536597973969775033641377376100170914945565997987116567");
		tf[34].data.c[1].data.d = pfFPinit("-0.09659637760427758626995943264154250783896765616223889179845718443");
		tf[35].data.c[0].data.d = pfFPinit("0.1490350950131748690343833248023213110809000650500831289745974611");
		tf[35].data.c[1].data.d = pfFPinit("0.0005105695418250804339547702683307201527363318466451417481683222836");
		tf[36].data.c[0].data.d = pfFPinit("0.1490274895206284836591621817667028930766750604547090209489099238");
		tf[36].data.c[1].data.d = pfFPinit("0.001589840879480608517465431678713987204348120811937896861283369944");
		tf[37].data.c[0].data.d = pfFPinit("-0.08519630757797469832137337620635968175755346296937346604193016518");
		tf[37].data.c[1].data.d = pfFPinit("-0.1222837250083682688848042344088660237905799201253720542051993260");
		tf[38].data.c[0].data.d = pfFPinit("-0.03233585554119885420518793372549388375473766757255267740938330891");
		tf[38].data.c[1].data.d = pfFPinit("-0.1454857816876337216480605322462116769243448926384000293355163381");
		tf[39].data.c[0].data.d = pfFPinit("0.09755908731550658785219769352681082179431803532212897643415251821");
		tf[39].data.c[1].data.d = pfFPinit("-0.1126674074833022053539365306335596774305253173354969427588185005");
		tf[40].data.c[0].data.d = pfFPinit("0.07874152000761948864688655131831991551391776381434960280298420014");
		tf[40].data.c[1].data.d = pfFPinit("-0.1265365293254590571537531335983682133007515952402222618515115748");
		tf[41].data.c[0].data.d = pfFPinit("-0.07022065456636491404759541228086267646589270324078022221749184196");
		tf[41].data.c[1].data.d = pfFPinit("-0.1314563802145576901177402900333653308445350192745368738733199002");
		tf[42].data.c[0].data.d = pfFPinit("0.02577385104501349096762173438928586633892063973860283760530717183");
		tf[42].data.c[1].data.d = pfFPinit("0.1467904248551401842035905250956003772891191124189771645455941252");
		tf[43].data.c[0].data.d = pfFPinit("-0.04482474129524076051134662350013059188160098171893019382040360940");
		tf[43].data.c[1].data.d = pfFPinit("0.1421353678528248571990892779732841300188956948706879764668083717");
		tf[44].data.c[0].data.d = pfFPinit("-0.004416011885642990456100351068043850683619134358764061250364805648");
		tf[44].data.c[1].data.d = pfFPinit("0.1489705308638891842652161446123306383600799092822704576292561655");
		tf[45].data.c[0].data.d = pfFPinit("-0.1440217537342382408054304926674723662184275053647204090602552722");
		tf[45].data.c[1].data.d = pfFPinit("-0.03833346681631513391837907054525246096404414686166602001126863432");

		for (pfUINT i = radix - 1; i > 0; i--) {
			if (stageSize == 1) {
				temp_complex.data.c[0].data.d = pfFPinit("1.0");
				temp_complex.data.c[1].data.d = pfFPinit("0.0");
				//PfMov(sc, &sc->w, &temp_complex);	
				//PfMov(sc, &sc->locID[i], &regID[i]);
			}
			else {
				if ((int64_t)i == radix - 1) {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
						}
						else {
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				else {
					if (sc->LUT) {
						if (sc->useCoalescedLUTUploadToSM) {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
							appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
						}
						else {
							temp_int.data.i = (radix - 1 - i) * stageSize;
							PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
							appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
						}
						if (stageAngle < 0) {
							PfConjugate(sc, &sc->w, &sc->w);
							
						}
					}
					else {
						temp_double.data.d = pfFPinit("2.0") * i / radix;
						PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
						PfSinCos(sc, &sc->w, &sc->tempFloat);
					}
				}
				PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
			}
			
		}
		pfUINT permute[47] = { 0, 1, 5, 25, 31, 14, 23, 21, 11, 8, 40, 12, 13, 18, 43, 27, 41, 17, 38, 2, 10, 3, 15, 28, 46, 42, 22, 16, 33, 24, 26, 36, 39, 7, 35, 34, 29, 4, 20, 6, 30, 9, 45, 37, 44, 32, 19};
		PfPermute(sc, permute, radix, 1, regID, &sc->w);

		PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
		for (int t = 0; t < radix-1; t++) {
			tempID[t].type = 100 + sc->vecTypeCode;
			PfAllocateContainerFlexible(sc, &tempID[t], 50);
			PfCopyContainer(sc, &tempID[t], &regID[t+1]);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -1.0, tempID);

		PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

		for (int t = 0; t < radix - 1; t++) {
			PfMul(sc, &tempID[t], &tempID[t], &tf[t], &sc->w);
		}

		inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, 1.0, tempID);

		for (int t = 0; t < radix-1; t++) {
			PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
			PfCopyContainer(sc, &regID[t+1], &tempID[t]);
			PfDeallocateContainer(sc, &tempID[t]);
		}
		free(tempID);

		PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);

		pfUINT permute2[47];
		permute2[0] = 0;
		permute2[1] = 1;
		for (pfUINT t = 2; t < radix; t++) {
			permute2[permute[radix + 1 - t]] = t;
		}
		if (stageAngle > 0) {
			for (pfUINT t = 0; t < (radix/2); t++) {
				pfUINT temp_permute = permute2[radix-1-t];
				permute2[radix-1 - t] = permute2[t + 1];
				permute2[t+1] = temp_permute;
			}
		}
		PfPermute(sc, permute2, radix, 1, regID, &sc->w);

		for (pfINT i = 0; i < radix-1; i++){
			PfDeallocateContainer(sc, &tf[i]);
		}
		break;
	}
	/*case 32: {
		
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 16; i++) {
				PfMov(sc, &sc->temp, &regID[i + 16]);
			
				PfSub(sc, &regID[i + 16], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
										
				}
				else {
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
										
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				PfSinCos(sc, &sc->w, &sc->angle);
			}
			for (pfUINT i = 0; i < 16; i++) {
				PfMul(sc, &sc->temp, &regID[i + 16], &sc->w, 0);
			
				PfSub(sc, &regID[i + 16], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 8; i++) {
				PfMov(sc, &sc->temp, &regID[i + 8]);
			
				PfSub(sc, &regID[i + 8], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.5");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			for (pfUINT i = 0; i < 8; i++) {
				PfMul(sc, &sc->temp, &regID[i + 8], &sc->w, 0);
			
				PfSub(sc, &regID[i + 8], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			for (pfUINT i = 16; i < 24; i++) {
				if (stageAngle < 0) {
					PfMov(sc, &sc->temp.data.c[0], &regID[i + 8].data.c[1]);
					PfMovNeg(sc, &sc->temp.data.c[1], &regID[i + 8].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
				}
				else {
					PfMovNeg(sc, &sc->temp.data.c[0], &regID[i + 8].data.c[1]);
					PfMov(sc, &sc->temp.data.c[1], &regID[i + 8].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
				}

				PfSub(sc, &regID[i + 8], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		else {
			if (stageAngle < 0) {
				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}

			for (pfUINT i = 16; i < 24; i++) {
				PfMul(sc, &sc->temp, &regID[i + 8], &sc->iw, 0);

				PfSub(sc, &regID[i + 8], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 4; i++) {
				PfMov(sc, &sc->temp, &regID[i + 4]);
			
				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = 2 * stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = 2 * stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.25");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			for (pfUINT i = 0; i < 4; i++) {
				PfMul(sc, &sc->temp, &regID[i + 4], &sc->w, 0);
			
				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			for (pfUINT i = 8; i < 12; i++) {
				if (stageAngle < 0) {
					PfMov(sc, &sc->temp.data.c[0], &regID[i + 4].data.c[1]);
					PfMovNeg(sc, &sc->temp.data.c[1], &regID[i + 4].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
				}
				else {
					PfMovNeg(sc, &sc->temp.data.c[0], &regID[i + 4].data.c[1]);
					PfMov(sc, &sc->temp.data.c[1], &regID[i + 4].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
				}

				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		else {
			if (stageAngle < 0) {
				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}
			for (pfUINT i = 8; i < 12; i++) {
				PfMul(sc, &sc->temp, &regID[i + 4], &sc->iw, 0);

				PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		if (stageAngle < 0) {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("-0.70710678118654752440084436210485");
		}
		else {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("0.70710678118654752440084436210485");
		}
		if (stageSize == 1)
			PfMov(sc, &sc->iw, &temp_complex);
		else
			PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
		for (pfUINT i = 16; i < 20; i++) {
			PfMul(sc, &sc->temp, &regID[i + 4], &sc->iw, 0);
			
			PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}
		if (stageAngle < 0) {
			PfMov(sc, &sc->w.data.c[0], &sc->iw.data.c[1]);
			PfMovNeg(sc, &sc->w.data.c[1], &sc->iw.data.c[0]);
			
			//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(iw.y, -iw.x);\n\n", vecType);
		}
		else {
			PfMovNeg(sc, &sc->w.data.c[0], &sc->iw.data.c[1]);
			PfMov(sc, &sc->w.data.c[1], &sc->iw.data.c[0]);
			
			//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(-iw.y, iw.x);\n\n", vecType);
		}
		for (pfUINT i = 24; i < 28; i++) {
			PfMul(sc, &sc->temp, &regID[i + 4], &sc->w, 0);
			
			PfSub(sc, &regID[i + 4], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}

		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 2; i++) {
				PfMov(sc, &sc->temp, &regID[i + 2]);
			
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = 3 * stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = 3 * stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
					
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.125");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			for (pfUINT i = 0; i < 2; i++) {
				PfMul(sc, &sc->temp, &regID[i + 2], &sc->w, 0);
			
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			for (pfUINT i = 4; i < 6; i++) {
				if (stageAngle < 0) {
					PfMov(sc, &sc->temp.data.c[0], &regID[i + 2].data.c[1]);
					PfMovNeg(sc, &sc->temp.data.c[1], &regID[i + 2].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
				}
				else {
					PfMovNeg(sc, &sc->temp.data.c[0], &regID[i + 2].data.c[1]);
					PfMov(sc, &sc->temp.data.c[1], &regID[i + 2].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
				}

				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		else {
			if (stageAngle < 0) {
				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}
			for (pfUINT i = 4; i < 6; i++) {
				PfMul(sc, &sc->temp, &regID[i + 2], &sc->iw, 0);

				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		if (stageAngle < 0) {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("-0.70710678118654752440084436210485");
		}
		else {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("0.70710678118654752440084436210485");
		}
		if (stageSize == 1)
			PfMov(sc, &sc->iw, &temp_complex);
		else
			PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
		for (pfUINT i = 8; i < 10; i++) {
			PfMul(sc, &sc->temp, &regID[i + 2], &sc->iw, 0);
			
			PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}
		if (stageAngle < 0) {
			PfMov(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
			PfMovNeg(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
			
			PfMov(sc, &sc->iw, &sc->temp);
			
		}
		else {
			PfMovNeg(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
			PfMov(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
			
			PfMov(sc, &sc->iw, &sc->temp);
			
		}
		for (pfUINT i = 12; i < 14; i++) {
			PfMul(sc, &sc->temp, &regID[i + 2], &sc->iw, 0);
			
			PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}


		for (pfUINT j = 0; j < 2; j++) {
			if (stageAngle < 0) {
				temp_complex.data.c[0].data.d = pfcos((2 * j + 1) * sc->double_PI / 8);
				temp_complex.data.c[1].data.d = -pfsin((2 * j + 1) * sc->double_PI / 8);
			}
			else {
				temp_complex.data.c[0].data.d = pfcos((2 * j + 1) * sc->double_PI / 8);
				temp_complex.data.c[1].data.d = pfsin((2 * j + 1) * sc->double_PI / 8);
			}
			if (stageSize == 1)
				PfMov(sc, &sc->iw, &temp_complex);
			else
				PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
			for (pfUINT i = 16 + 8 * j; i < 18 + 8 * j; i++) {
				PfMul(sc, &sc->temp, &regID[i + 2], &sc->iw, 0);
				
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
				
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
				
			}
			if (stageAngle < 0) {
				PfMov(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
				PfMovNeg(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
			
				PfMov(sc, &sc->iw, &sc->temp);
			}
			else {
				PfMovNeg(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
				PfMov(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
			
				PfMov(sc, &sc->iw, &sc->temp);
				
			}
			for (pfUINT i = 20 + 8 * j; i < 22 + 8 * j; i++) {
				PfMul(sc, &sc->temp, &regID[i + 2], &sc->iw, 0);
				
				PfSub(sc, &regID[i + 2], &regID[i], &sc->temp);
				
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
				
			}
		}

		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			for (pfUINT i = 0; i < 1; i++) {
				PfMov(sc, &sc->temp, &regID[i + 1]);
			
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		else {
			if (sc->LUT) {
				if (sc->useCoalescedLUTUploadToSM) {
					temp_int.data.i = 4 * stageSize;
					PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
					appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
				}
				else {
					temp_int.data.i = 4 * stageSize;
					PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
					appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
				}
				if (stageAngle < 0) {
					PfConjugate(sc, &sc->w, &sc->w);
				}
			}
			else {
				temp_double.data.d = pfFPinit("0.0625");
				PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
				PfSinCos(sc, &sc->w, &sc->tempFloat);
			}
			for (pfUINT i = 0; i < 1; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->w, 0);
			
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
			
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
			}
		}
		if (stageSize == 1) {
			for (pfUINT i = 2; i < 3; i++) {
				if (stageAngle < 0) {
					PfMov(sc, &sc->temp.data.c[0], &regID[i + 1].data.c[1]);
					PfMovNeg(sc, &sc->temp.data.c[1], &regID[i + 1].data.c[0]);


					//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
				}
				else {
					PfMovNeg(sc, &sc->temp.data.c[0], &regID[i + 1].data.c[1]);
					PfMov(sc, &sc->temp.data.c[1], &regID[i + 1].data.c[0]);

					//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
				}

				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		else {
			if (stageAngle < 0) {
				PfMov(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMovNeg(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);


				//&sc->tempLen = sprintf(&sc->tempStr, "	w = %s(w.y, -w.x);\n\n", vecType);
			}
			else {
				PfMovNeg(sc, &sc->iw.data.c[0], &sc->w.data.c[1]);
				PfMov(sc, &sc->iw.data.c[1], &sc->w.data.c[0]);

				//&sc->tempLen = sprintf(&sc->tempStr, "	iw = %s(-w.y, w.x);\n\n", vecType);
			}
			for (pfUINT i = 2; i < 3; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);

				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);

				PfAdd(sc, &regID[i], &regID[i], &sc->temp);

			}
		}
		if (stageAngle < 0) {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("-0.70710678118654752440084436210485");
		}
		else {
			temp_complex.data.c[0].data.d = pfFPinit("0.70710678118654752440084436210485");
			temp_complex.data.c[1].data.d = pfFPinit("0.70710678118654752440084436210485");
		}
		if (stageSize == 1)
			PfMov(sc, &sc->iw, &temp_complex);
		else
			PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
		for (pfUINT i = 4; i < 5; i++) {
			PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
			
			PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}
		if (stageAngle < 0) {
			PfMov(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
			PfMovNeg(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
			
			PfMov(sc, &sc->iw, &sc->temp);
			
		}
		else {
			PfMovNeg(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
			PfMov(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
			
			
			PfMov(sc, &sc->iw, &sc->temp);
			
		}
		for (pfUINT i = 6; i < 7; i++) {
			PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
			
			PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
			
			PfAdd(sc, &regID[i], &regID[i], &sc->temp);
			
		}


		for (pfUINT j = 0; j < 2; j++) {
			if (stageAngle < 0) {
				temp_complex.data.c[0].data.d = pfcos((2 * j + 1) * sc->double_PI / 8);
				temp_complex.data.c[1].data.d = -pfsin((2 * j + 1) * sc->double_PI / 8);
			}
			else {
				temp_complex.data.c[0].data.d = pfcos((2 * j + 1) * sc->double_PI / 8);
				temp_complex.data.c[1].data.d = pfsin((2 * j + 1) * sc->double_PI / 8);
			}
			if (stageSize == 1)
				PfMov(sc, &sc->iw, &temp_complex);
			else
				PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
			for (pfUINT i = 8 + 4 * j; i < 9 + 4 * j; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
				
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
				
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
				
			}
			if (stageAngle < 0) {
				PfMov(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
				PfMovNeg(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
				
				PfMov(sc, &sc->iw, &sc->temp);
				
			}
			else {
				PfMovNeg(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
				PfMov(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
				
				PfMov(sc, &sc->iw, &sc->temp);
				
			}
			for (pfUINT i = 10 + 4 * j; i < 11 + 4 * j; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
				
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
				
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
				
			}
		}

		for (pfUINT j = 0; j < 4; j++) {
			if ((j == 1) || (j == 2)) {
				if (stageAngle < 0) {
					temp_complex.data.c[0].data.d = pfcos((7 - 2 * j) * sc->double_PI / 16);
					temp_complex.data.c[1].data.d = -pfsin((7 - 2 * j) * sc->double_PI / 16);
				}
				else {
					temp_complex.data.c[0].data.d = pfcos((7 - 2 * j) * sc->double_PI / 16);
					temp_complex.data.c[1].data.d = pfsin((7 - 2 * j) * sc->double_PI / 16);
				}
			}
			else {
				if (stageAngle < 0) {
					temp_complex.data.c[0].data.d = pfcos((2 * j + 1) * sc->double_PI / 16);
					temp_complex.data.c[1].data.d = -pfsin((2 * j + 1) * sc->double_PI / 16);
				}
				else {
					temp_complex.data.c[0].data.d = pfcos((2 * j + 1) * sc->double_PI / 16);
					temp_complex.data.c[1].data.d = pfsin((2 * j + 1) * sc->double_PI / 16);
				}
			}
			if (stageSize == 1)
				PfMov(sc, &sc->iw, &temp_complex);
			else
				PfMul(sc, &sc->iw, &sc->w, &temp_complex, 0);
			for (pfUINT i = 16 + 4 * j; i < 17 + 4 * j; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
				
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
				
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
				
			}
			if (stageAngle < 0) {
				PfMov(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
				PfMovNeg(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
				
				PfMov(sc, &sc->iw, &sc->temp);
				
			}
			else {
				PfMovNeg(sc, &sc->temp.data.c[0], &sc->iw.data.c[1]);
				PfMov(sc, &sc->temp.data.c[1], &sc->iw.data.c[0]);
				
				PfMov(sc, &sc->iw, &sc->temp);
				
			}
			for (pfUINT i = 18 + 4 * j; i < 19 + 4 * j; i++) {
				PfMul(sc, &sc->temp, &regID[i + 1], &sc->iw, 0);
				
				PfSub(sc, &regID[i + 1], &regID[i], &sc->temp);
				
				PfAdd(sc, &regID[i], &regID[i], &sc->temp);
				
			}
		}

		pfUINT permute2[32] = { 0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31 };
		PfPermute(sc, permute2, 32, 1, regID, &sc->temp);
		
		break;
	}*/
	default:
		inlineGeneratedRadixKernelVkFFT(sc, radix, stageSize, stageSizeSum, stageAngle, regID);
	}
	PfDeallocateContainer(sc, &temp_complex);
	return;
}
static inline void inlineGeneratedPrimeRadixKernelVkFFT(VkFFTSpecializationConstantsLayout* sc, pfINT radix, pfINT stageSize, pfINT stageSizeSum, pfLD stageAngle, PfContainer* regID) {
	//Not implemented properly
	if (sc->res != VKFFT_SUCCESS) return;

	PfContainer temp_complex = VKFFT_ZERO_INIT;
	temp_complex.type = 23;
	PfAllocateContainerFlexible(sc, &temp_complex, 50);
	PfContainer temp_double = VKFFT_ZERO_INIT;
	temp_double.type = 22;
	PfContainer temp_int = VKFFT_ZERO_INIT;
	temp_int.type = 31;

	VkFFTRaderContainer* currentRaderContainer = VKFFT_ZERO_INIT;
	for (pfUINT i = 0; i < sc->numRaderPrimes; i++) {
		if (sc->raderContainer[i].prime == radix) {
			currentRaderContainer = &sc->raderContainer[i];
		}
	}
	for (pfUINT i = radix - 1; i > 0; i--) {
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	
			//PfMov(sc, &sc->locID[i], &regID[i]);
		}
		else {
			if ((int64_t)i == radix - 1) {
				if (sc->LUT) {
					if (sc->useCoalescedLUTUploadToSM) {
						appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
					}
					else {
						appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
					}
					if (stageAngle < 0) {
						PfConjugate(sc, &sc->w, &sc->w);
							
					}
				}
				else {
					temp_double.data.d = pfFPinit("2.0") * i / radix;
					PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
					PfSinCos(sc, &sc->w, &sc->tempFloat);
				}
			}
			else {
				if (sc->LUT) {
					if (sc->useCoalescedLUTUploadToSM) {
						temp_int.data.i = (radix - 1 - i) * stageSize;
						PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
						appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
					}
					else {
						temp_int.data.i = (radix - 1 - i) * stageSize;
						PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
						appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
					}
					if (stageAngle < 0) {
						PfConjugate(sc, &sc->w, &sc->w);
							
					}
				}
				else {
					temp_double.data.d = pfFPinit("2.0") * i / radix;
					PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
					PfSinCos(sc, &sc->w, &sc->tempFloat);
				}
			}
			PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
		}
			
	}
		
	pfUINT* permute = (pfUINT*)calloc(radix, sizeof(pfUINT));// { 0, 1, 2, 4, 8, 3, 6, 12, 11, 9, 5, 10, 7, 1 };

	pfUINT g_pow = 1;
	permute[0] = 0;
	permute[1] = 1;
	for (pfUINT t = 0; t < currentRaderContainer->prime - 2; t++) {
		g_pow = (g_pow * currentRaderContainer->generator) % currentRaderContainer->prime;
		permute[t+2] = g_pow;
	}	
	PfPermute(sc, permute, 13, 1, regID, &sc->w);

	PfContainer* tempID = (PfContainer*)calloc(radix-1, sizeof(PfContainer));
	for (int t = 0; t < radix-1; t++) {
		tempID[t].type = 100 + sc->vecTypeCode;
		PfAllocateContainerFlexible(sc, &tempID[t], 50);
		PfCopyContainer(sc, &tempID[t], &regID[t+1]);
	}

	inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, stageAngle, tempID);

	PfMov(sc, &sc->locID[radix - 1], &tempID[0]);

	PfContainer tempMul = VKFFT_ZERO_INIT;
	tempMul.type = tempID[0].type % 100;
	PfAllocateContainerFlexible(sc, &tempMul, 50);

	for (int t = 0; t < radix-1; t++) {
		switch ((tempID[t].type / 10) % 10) {
		case 1: {
			float* tempRaderFFTkernel = (float*)currentRaderContainer->raderFFTkernel;
			tempMul.data.c[0].data.d = tempRaderFFTkernel[2 * t];
			tempMul.data.c[1].data.d = tempRaderFFTkernel[2 * t + 1];
			break;
		}
		case 2: {
			double* tempRaderFFTkernel = (double*)currentRaderContainer->raderFFTkernel;
			tempMul.data.c[0].data.d = tempRaderFFTkernel[2 * t];
			tempMul.data.c[1].data.d = tempRaderFFTkernel[2 * t + 1];
			break;
		}
		case 3: {
			double* tempRaderFFTkernel = (double*)currentRaderContainer->raderFFTkernel;
			tempMul.data.c[0].data.dd[0].data.d = tempRaderFFTkernel[4 * t];
			tempMul.data.c[0].data.dd[1].data.d = tempRaderFFTkernel[4 * t + 1];
			tempMul.data.c[1].data.dd[0].data.d = tempRaderFFTkernel[4 * t + 2];
			tempMul.data.c[1].data.dd[1].data.d = tempRaderFFTkernel[4 * t + 3];
			break;
		}
		}
		PfMul(sc, &tempID[t], &tempID[t], &tempMul, 0);
	}
	PfDeallocateContainer(sc, &tempMul);

	inlineRadixKernelVkFFT(sc, radix - 1, 1, 1, -stageAngle, tempID);

	for (int t = 0; t < radix-1; t++) {
		PfAdd(sc, &tempID[t], &tempID[t], &regID[0]);
		PfDeallocateContainer(sc, &tempID[t]);
	}
	free(tempID);

	PfAdd(sc, &regID[0], &regID[0], &sc->locID[radix - 1]);
	
	g_pow = 1;
	for (pfUINT t = 0; t < (currentRaderContainer->prime-2)/2; t++) {
		pfUINT temp_permute = permute[currentRaderContainer->prime-1-t];
		permute[currentRaderContainer->prime - 1 - t] = permute[t + 2];
		permute[t+2] = temp_permute;
	}
	PfPermute(sc, permute, 13, 1, regID, &sc->w);
	free(permute);
	PfDeallocateContainer(sc, &temp_complex);
	return;
}
static inline void inlineGeneratedRadixKernelVkFFT(VkFFTSpecializationConstantsLayout* sc, pfINT radix, pfINT stageSize, pfINT stageSizeSum, pfLD stageAngle, PfContainer* regID) {
	if (sc->res != VKFFT_SUCCESS) return;

	PfContainer temp_complex = VKFFT_ZERO_INIT;
	temp_complex.type = 23;
	PfAllocateContainerFlexible(sc, &temp_complex, 50);
	PfContainer temp_double = VKFFT_ZERO_INIT;
	temp_double.type = 22;
	PfContainer temp_int = VKFFT_ZERO_INIT;
	temp_int.type = 31;
	//sprintf(temp, "loc_0");

	for (pfUINT i = radix - 1; i > 0; i--) {
		if (stageSize == 1) {
			temp_complex.data.c[0].data.d = pfFPinit("1.0");
			temp_complex.data.c[1].data.d = pfFPinit("0.0");
			//PfMov(sc, &sc->w, &temp_complex);	

		}
		else {
			if ((int64_t)i == radix - 1) {
				if (sc->LUT) {
					if (sc->useCoalescedLUTUploadToSM) {
						appendSharedToRegisters(sc, &sc->w, &sc->stageInvocationID);
														
					}
					else {
						appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->LUTId);
														
					}
					if (stageAngle < 0) {
						PfConjugate(sc, &sc->w, &sc->w);
							
					}
				}
				else {
					temp_double.data.d = pfFPinit("2.0") * i / radix;
					PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
					PfSinCos(sc, &sc->w, &sc->tempFloat);
				}
			}
			else {
				if (sc->LUT) {
					if (sc->useCoalescedLUTUploadToSM) {
						temp_int.data.i = (radix - 1 - i) * stageSize;
						PfAdd(sc, &sc->sdataID, &sc->stageInvocationID, &temp_int);
						appendSharedToRegisters(sc, &sc->w, &sc->sdataID);
					}
					else {
						temp_int.data.i = (radix - 1 - i) * stageSize;
						PfAdd(sc, &sc->inoutID, &sc->LUTId, &temp_int);
						appendGlobalToRegisters(sc, &sc->w, &sc->LUTStruct, &sc->inoutID);
							
					}
					if (stageAngle < 0) {
						PfConjugate(sc, &sc->w, &sc->w);
							
					}
				}
				else {
					temp_double.data.d = pfFPinit("2.0") * i / radix;
					PfMul(sc, &sc->tempFloat, &sc->angle, &temp_double, 0);
					PfSinCos(sc, &sc->w, &sc->tempFloat);
				}
			}
			PfMul(sc, &regID[i], &regID[i], &sc->w, &sc->temp);
		}
			
	}
	//important
	//PfMov(sc, &regID[1], &sc->locID[1]);
	//
	//PfMov(sc, &regID[2], &sc->locID[2]);
	//
	pfUINT P = 0;

	for (pfUINT i = ((pfUINT)pfceil(pfsqrt((double)radix))); i < radix; i++) {
		if ((radix % i) == 0) {
			P = i;
			i = radix;
		}
	} 
	
	if (P == 0) {
		//inlineGeneratedPrimeRadixKernelVkFFT(sc, radix, stageSize, stageSizeSum, stageAngle, regID);
		PfDeallocateContainer(sc, &temp_complex);
		sc->res = VKFFT_ERROR_MATH_FAILED;
		return;
	}
	pfUINT Q = radix/P;
	pfUINT tempQ = Q;
	pfUINT tempP = P;
	while ((tempQ % 4) == 0) {
		tempQ /= 4;
	}
	while ((tempP % 4) == 0) {
		tempP /= 4;
	}
	if (((tempP % 2) == 0) && ((tempQ % 2) == 0)) {
		P *= 2;
		Q /= 2;
	}
	PfContainer* tempID = (PfContainer*)calloc(P, sizeof(PfContainer));
	for (int t = 0; t < P; t++) {
		tempID[t].type = 100 + sc->vecTypeCode;
		PfAllocateContainerFlexible(sc, &tempID[t], 50);
	}
	for (pfUINT i = 0; i < Q; i++) {
		for (pfUINT j = 0; j < P; j++) {
			PfCopyContainer(sc, &tempID[j], &regID[i + j * Q]);
		}
		inlineRadixKernelVkFFT(sc, P, 1, 1, stageAngle, tempID);
		for (pfUINT j = 0; j < P; j++) {
			PfCopyContainer(sc, &regID[i + j * Q], &tempID[j]);
		}
	}
	for (int t = 0; t < P; t++) {
		PfDeallocateContainer(sc, &tempID[t]);
	}
	free(tempID);

	tempID = (PfContainer*)calloc(Q, sizeof(PfContainer));
	for (int t = 0; t < Q; t++) {
		tempID[t].type = 100 + sc->vecTypeCode;
		PfAllocateContainerFlexible(sc, &tempID[t], 50);
	}
	
	for (pfUINT i = 0; i < P; i++) {
		for (pfUINT j = 0; j < Q; j++) {
			if ((i > 0) && (j > 0)) {
				if (stageAngle < 0) {
					temp_complex.data.c[0].data.d = pfcos(2 * i * j * sc->double_PI / radix);
					temp_complex.data.c[1].data.d = -pfsin(2 * i * j * sc->double_PI / radix);
					PfMov(sc, &sc->w, &temp_complex);	
						
				}
				else {
					temp_complex.data.c[0].data.d = pfcos(2 * i * j * sc->double_PI / radix);
					temp_complex.data.c[1].data.d = pfsin(2 * i * j * sc->double_PI / radix);
					PfMov(sc, &sc->w, &temp_complex);	
						
				}
				PfMul(sc, &regID[Q * i + j], &regID[Q * i + j], &sc->w, &sc->temp);
					
			}
		}

		for (pfUINT j = 0; j < Q; j++) {
			PfCopyContainer(sc, &tempID[j], &regID[Q * i + j]);
		}
		inlineRadixKernelVkFFT(sc, Q, 1, 1, stageAngle, tempID);
		for (pfUINT j = 0; j < Q; j++) {
			PfCopyContainer(sc, &regID[Q * i + j], &tempID[j]);
		}	
	}
	for (int t = 0; t < Q; t++) {
		PfDeallocateContainer(sc, &tempID[t]);
	}
	free(tempID);

	pfUINT* permute2 = (pfUINT*)calloc(radix, sizeof(pfUINT));
	for (int t = 0; t < radix; t++) {
		permute2[t] = (t % P) * Q + t / P;
	}

	PfPermute(sc, permute2, radix, 1, regID, &sc->temp);
	free(permute2);
	PfDeallocateContainer(sc, &temp_complex);
	return;
}
#endif
