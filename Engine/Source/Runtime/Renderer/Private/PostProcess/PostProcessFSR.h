//------------------------------------------------------------------------------
// FidelityFX Super Resolution UE4 Integration
//
// Copyright (c) 2021 Advanced Micro Devices, Inc. All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//------------------------------------------------------------------------------


#pragma once

#include "CoreMinimal.h"
#include "RendererInterface.h"
#include "PostProcess/RenderingCompositionGraph.h"
#include "ScreenPass.h"
#include "PostProcess/PostProcessTonemap.h"

class FViewInfo;

struct FFSRInputs
{
	// [Optional] Render to the specified output. If invalid, a new texture is created and returned.
	FScreenPassRenderTarget OverrideOutput;

	// [Required] Tonemapped scene color [RGB: 0.0f - 1.0f] to filter.
	FScreenPassTexture SceneColor;
};


bool IsFidelityFXFSRPassEnabled(const FViewInfo& View);

FScreenPassTexture AddFidelityFXSuperResolutionPass(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FFSRInputs& Inputs);

// If FidelityFX Super Resolution (FSR) is enabled as the Primary or Secondary Upscale method,
// returns the effective resolution fraction.
// If this function is called without FSR being enabled, it returns -1.0f
float GetFidelityFXFSREffectiveResolutionFraction(const FViewInfo& View);

// Determines whether FidelityFX Super Resolution will need to provide MipBias values for the scene view
bool ShouldFidelityFXFSROverrideMipBias(const FViewInfo& View);

// Returns the MipBias value to be applied to the materials of the BasePass when
// FidelityFX Super Resolution is enabled && ShouldFidelityFXFSROverrideMipBias() returns true
float GetFidelityFXFSRMipBias(const FViewInfo& View);