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

#include "PostProcess/PostProcessFSR.h"

#include "GlobalShader.h"
#include "PostProcessing.h"
#include "ScreenPass.h"
#include "RHI.h"
#include "SceneFilterRendering.h"
#include "BlueNoise.h"

// ===============================================================================================================================================
//
// FIDELITYFX CONFIGURATION
//
// ===============================================================================================================================================
#define A_CPU 1

// colliding ffx_a.h/UE4Engine definitions
// ffx_a.h will override these definitions
#ifdef A_STATIC
#undef A_STATIC
#endif
#ifdef A_RESTRICT
#undef A_RESTRICT
#endif

#include "..\Shaders\Shared\ThirdParty\AMD\FidelityFX\FSR1\ffx_a.h"
#include "..\Shaders\Shared\ThirdParty\AMD\FidelityFX\FSR1\ffx_fsr1.h"


// ===============================================================================================================================================
//
// CVARS
//
// ===============================================================================================================================================
static int32 GFSR_FP16 = 1;
static FAutoConsoleVariableRef CVarFSRUseFP16(
	TEXT("r.FidelityFX.FSR.UseFP16"),
	GFSR_FP16,
	TEXT("Enables half precision shaders for extra performance (if the device supports) without perceivable visual difference."),
	ECVF_RenderThreadSafe);

static int32 GFSR_MipBiasMethod = 1; // 0: disabled, 1: -log2(screen%), 2: r.FidelityFX.FSR.MipBias.Offset
static FAutoConsoleVariableRef CVarFSRMipBiasMethod(
	TEXT("r.FidelityFX.FSR.MipBias.Method"),
	GFSR_MipBiasMethod,
	TEXT("Selects the method for applying negative MipBias to material texture views for improving the upscaled image quality. 0: Disabled, 1: Automatic (-log2(screen%)), 2: Uses the r.FidelityFX.FSR.MipBias.Offset value"),
	ECVF_RenderThreadSafe);

static float GFSR_MipBiasManualOffset = 0.0f;
static FAutoConsoleVariableRef CVarFSRMipBiasManualOffset(
	TEXT("r.FidelityFX.FSR.MipBias.Offset"),
	GFSR_MipBiasManualOffset,
	TEXT("MipBias value to apply when the r.FidelityFX.FSR.MipBias.Method is set to 2"),
	ECVF_RenderThreadSafe);

static int32 GFSR_RCAS = 1;
static FAutoConsoleVariableRef CVarFSRAddRCAS(
	TEXT("r.FidelityFX.FSR.RCAS.Enabled"),
	GFSR_RCAS,
	TEXT("FidelityFX FSR/RCAS : Robust Contrast Adaptive Sharpening Filter. Requires r.FidelityFX.FSR.PrimaryUpscale 1 or r.FidelityFX.FSR.SecondaryUpscale 1"),
	ECVF_RenderThreadSafe);

static float GFSR_Sharpness = 0.2; // 0.2 stops = 1 / (2^N) ~= 0.87 in lienar [0-1] sharpening of CAS
static FAutoConsoleVariableRef CVarFSRRCASSharpness(
	TEXT("r.FidelityFX.FSR.RCAS.Sharpness"),
	GFSR_Sharpness,
	TEXT("FidelityFX RCAS Sharpness in stops (0: sharpest, 1: 1/2 as sharp, 2: 1/4 as sharp, 3: 1/8 as sharp, etc.). A value of 0.2 would correspond to a ~0.87 sharpness in [0-1] linear scale"),
	ECVF_RenderThreadSafe);

static int32 GFSR_RCASDenoise = 0;
static FAutoConsoleVariableRef CVarFSRRCASDenoise(
	TEXT("r.FidelityFX.FSR.RCAS.Denoise"),
	GFSR_RCASDenoise,
	TEXT("FidelityFX RCAS Denoise support for grainy input such as dithered images or input with custom film grain effects applied prior to FSR. 1:On, 0:Off"),
	ECVF_RenderThreadSafe);

static float GFSR_HDR_PQDither = 1.0f;
static FAutoConsoleVariableRef CVarFSR_HDR_PQDither(
	TEXT("r.FidelityFX.FSR.HDR.PQDitherAmount"),
	GFSR_HDR_PQDither,
	TEXT("[HDR-Only] DitherAmount to apply for PQ->Gamma2 conversion to eliminate color banding, when the output device is ST2084/PQ."),
	ECVF_RenderThreadSafe);

// debug cvars -------------------------------------------------------------
static int32 GFSR_ForcePS = 0;
static FAutoConsoleVariableRef CVarFSRForcePS(
	TEXT("r.FidelityFX.FSR.Debug.ForcePS"),
	GFSR_ForcePS,
	TEXT("Run FSR and RCAS in VS-PS"),
	ECVF_RenderThreadSafe);

//---------------------------------------------------------------------------------------------------
// ! IMPORTANT NOTE !
// Some NVidia GPUs show image corruption issues on DX11 when FP16 path is enabled with FSR1.
// To ensure the correctness of the FSR1 pass output, we've added an additional boolean
// check here to determine whether or not to use FP16 for the DX11/NVidia path.
// Once this is no longer an issue, the default value of GFSR_FP16OnNvidiaDX11 can be set to 1
// or this block of code can be entirely removed.
static int32 GFSR_FP16OnNvidiaDX11 = 0;
static FAutoConsoleVariableRef CVarFSREnableFP16OnNvidiaDX11(
	TEXT("r.FidelityFX.FSR.EnableFP16OnNvDX11"),
	GFSR_FP16OnNvidiaDX11,
	TEXT("Enables FP16 path for DX11/NVidia GPUs, requires 'r.FidelityFX.FSR.UseFP16 1'. Default=0 as image corruption is seen on DX11/NVidia with FP16"),
	ECVF_RenderThreadSafe);
//---------------------------------------------------------------------------------------------------



// ===============================================================================================================================================
//
// SHADER DEFINITIONS
//
// ===============================================================================================================================================

enum class EFSR_OutputDevice // should match PostProcessFFX_Common.ush
{
	sRGB = 0,
	Linear,
	PQ,
	scRGB,
	MAX
};
enum class EFSR_OutputDeviceMaxNits // matching UE4's scRGB/PQ-1000Nits and scRGB/PQ-2000Nits
{
	EFSR_1000Nits,
	EFSR_2000Nits,
	MAX
};
enum class EFSR_MipBiasMethod
{
	DISABLED,
	AUTOMATIC,
	MANUAL,

	NumFSRMipBiasMethods
};
static EFSR_OutputDevice GetFSROutputDevice(ETonemapperOutputDevice UE4TonemapperOutputDevice)
{
	EFSR_OutputDevice ColorSpace = EFSR_OutputDevice::MAX;

	switch (UE4TonemapperOutputDevice)
	{
	case ETonemapperOutputDevice::sRGB:
	case ETonemapperOutputDevice::Rec709:
	case ETonemapperOutputDevice::ExplicitGammaMapping:
		ColorSpace = EFSR_OutputDevice::sRGB;
		break;
	case ETonemapperOutputDevice::ACES1000nitST2084:
	case ETonemapperOutputDevice::ACES2000nitST2084:
		ColorSpace = EFSR_OutputDevice::PQ;
		break;
	case ETonemapperOutputDevice::ACES1000nitScRGB:
	case ETonemapperOutputDevice::ACES2000nitScRGB:
		ColorSpace = EFSR_OutputDevice::scRGB;
		break;
	case ETonemapperOutputDevice::LinearEXR:
	case ETonemapperOutputDevice::LinearNoToneCurve:
	case ETonemapperOutputDevice::LinearWithToneCurve:
		ColorSpace = EFSR_OutputDevice::Linear;
		break;
	}

	return ColorSpace;
}

// permutation domains
class FFSR_UseFP16Dim             : SHADER_PERMUTATION_BOOL("ENABLE_FP16");
class FFSR_OutputDeviceDim        : SHADER_PERMUTATION_ENUM_CLASS("FSR_OUTPUTDEVICE", EFSR_OutputDevice);
class FFSR_OutputDeviceNitsDim    : SHADER_PERMUTATION_ENUM_CLASS("MAX_ODT_NITS_ENUM", EFSR_OutputDeviceMaxNits);
class FFSR_OutputDeviceConversion : SHADER_PERMUTATION_BOOL("CONVERT_TO_OUTPUT_DEVICE");
class FFSR_PQDitherDim            : SHADER_PERMUTATION_BOOL("ENABLE_PQ_DITHER");
class FFSR_GrainDim               : SHADER_PERMUTATION_BOOL("USE_GRAIN_INTENSITY");
class FRCAS_DenoiseDim            : SHADER_PERMUTATION_BOOL("USE_RCAS_DENOISE");

//
// SHADER PASS PARAMETERS
//
BEGIN_SHADER_PARAMETER_STRUCT(FFSRPassParameters_Grain, )
	SHADER_PARAMETER(FVector4, GrainRandomFull)
	SHADER_PARAMETER(FVector4, GrainScaleBiasJitter)
END_SHADER_PARAMETER_STRUCT()

BEGIN_SHADER_PARAMETER_STRUCT(FFSRPassParameters, )
	SHADER_PARAMETER(FUintVector4, Const0)
	SHADER_PARAMETER(FUintVector4, Const1)
	SHADER_PARAMETER(FUintVector4, Const2)
	SHADER_PARAMETER(FUintVector4, Const3)
	SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters_Grain, FilmGrain)
	SHADER_PARAMETER(FVector2D, VPColor_ExtentInverse)
	SHADER_PARAMETER(FVector2D, VPColor_ViewportMin)
	SHADER_PARAMETER_SAMPLER(SamplerState, samLinearClamp)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
END_SHADER_PARAMETER_STRUCT()

BEGIN_SHADER_PARAMETER_STRUCT(FRCASPassParameters, )
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
	SHADER_PARAMETER(FUintVector4, Const0)
	SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters_Grain, FilmGrain)
	SHADER_PARAMETER(FVector2D, VPColor_ExtentInverse)
END_SHADER_PARAMETER_STRUCT()


///
/// FSR COMPUTE SHADER
///
class FFSRCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FFSRCS);
	SHADER_USE_PARAMETER_STRUCT(FFSRCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_UseFP16Dim, FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceConversion, FFSR_GrainDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters, FSR)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		if (PermutationVector.Get<FFSR_UseFP16Dim>())
		{
			if (Parameters.Platform != SP_PCD3D_SM5)
			{
				return false; //only compile FP16 shader permutation for Desktop PC.
			}
		}

		if (!PermutationVector.Get<FFSR_OutputDeviceConversion>() && eFSROutputDevice != EFSR_OutputDevice::sRGB)
		{
			return false; // don't compile FSR_OUTPUTDEVICE dimensions when there's no HDR conversions (== sRGB case)
		}

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), 64);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), 1);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), 1);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};

IMPLEMENT_GLOBAL_SHADER(FFSRCS, "/Engine/Private/PostProcessFFX_FSR.usf", "MainCS", SF_Compute);

///
/// FSR PIXEL SHADER
///
class FFSRPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FFSRPS);
	SHADER_USE_PARAMETER_STRUCT(FFSRPS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_UseFP16Dim, FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceConversion, FFSR_GrainDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters, FSR)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		if (PermutationVector.Get<FFSR_UseFP16Dim>())
		{
			if (Parameters.Platform != SP_PCD3D_SM5)
			{
				return false; //only compile FP16 shader permutation for Desktop PC.
			}
		}

		if (!PermutationVector.Get<FFSR_OutputDeviceConversion>() && eFSROutputDevice != EFSR_OutputDevice::sRGB)
		{
			return false; // don't compile FSR_OUTPUTDEVICE dimensions when there's no HDR conversions (== sRGB case)
		}

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
	}
};

IMPLEMENT_GLOBAL_SHADER(FFSRPS, "/Engine/Private/PostProcessFFX_FSR.usf", "MainPS", SF_Pixel);



///
/// RCAS COMPUTE SHADER
///
class FRCASCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FRCASCS);
	SHADER_USE_PARAMETER_STRUCT(FRCASCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_UseFP16Dim, FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceConversion, FFSR_GrainDim, FRCAS_DenoiseDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FRCASPassParameters, RCAS)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		if (PermutationVector.Get<FFSR_UseFP16Dim>())
		{
			if (Parameters.Platform != SP_PCD3D_SM5)
			{
				return false; //only compile FP16 shader permutation for Desktop PC.
			}
		}

		if (!PermutationVector.Get<FFSR_OutputDeviceConversion>() && eFSROutputDevice != EFSR_OutputDevice::sRGB)
		{
			return false; // don't compile FSR_OUTPUTDEVICE dimensions when there's no HDR conversions (== sRGB case)
		}

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), 64);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), 1);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), 1);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};

IMPLEMENT_GLOBAL_SHADER(FRCASCS, "/Engine/Private/PostProcessFFX_RCAS.usf", "MainCS", SF_Compute);


///
/// RCAS PIXEL SHADER
///
class FRCASPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FRCASPS);
	SHADER_USE_PARAMETER_STRUCT(FRCASPS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_UseFP16Dim, FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceConversion, FFSR_GrainDim, FRCAS_DenoiseDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FRCASPassParameters, RCAS)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		if (PermutationVector.Get<FFSR_UseFP16Dim>())
		{
			if (Parameters.Platform != SP_PCD3D_SM5)
			{
				return false; //only compile FP16 shader permutation for Desktop PC.
			}
		}

		if (!PermutationVector.Get<FFSR_OutputDeviceConversion>() && eFSROutputDevice != EFSR_OutputDevice::sRGB)
		{
			return false; // don't compile FSR_OUTPUTDEVICE dimensions when there's no HDR conversions (== sRGB case)
		}

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
	}
};

IMPLEMENT_GLOBAL_SHADER(FRCASPS, "/Engine/Private/PostProcessFFX_RCAS.usf", "MainPS", SF_Pixel);


//
// STANDALONE CHROMATIC ABERRATION / PIXEL SHADER
//
class FChromaticAberrationPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FChromaticAberrationPS);
	SHADER_USE_PARAMETER_STRUCT(FChromaticAberrationPS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_GrainDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ColorTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, ColorSampler)
		SHADER_PARAMETER(FVector4, ChromaticAberrationParams)
		SHADER_PARAMETER(FVector4, LensPrincipalPointOffsetScale)
		SHADER_PARAMETER(FVector4, LensPrincipalPointOffsetScaleInverse)
		SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters_Grain, FilmGrain)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, Color)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
	}
};

IMPLEMENT_GLOBAL_SHADER(FChromaticAberrationPS, "/Engine/Private/PostProcessChromaticAberration.usf", "MainPS", SF_Pixel);

//
// STANDALONE CHROMATIC ABERRATION / COMPUTE SHADER
//
class FChromaticAberrationCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FChromaticAberrationCS);
	SHADER_USE_PARAMETER_STRUCT(FChromaticAberrationCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_OutputDeviceDim, FFSR_OutputDeviceNitsDim, FFSR_GrainDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ColorTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, ColorSampler)
		SHADER_PARAMETER(FVector4, ChromaticAberrationParams)
		SHADER_PARAMETER(FVector4, LensPrincipalPointOffsetScale)
		SHADER_PARAMETER(FVector4, LensPrincipalPointOffsetScaleInverse)
		SHADER_PARAMETER_STRUCT_INCLUDE(FFSRPassParameters_Grain, FilmGrain)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, Color)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		const EFSR_OutputDevice eFSROutputDevice = PermutationVector.Get<FFSR_OutputDeviceDim>();

		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), 64);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), 1);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), 1);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};

IMPLEMENT_GLOBAL_SHADER(FChromaticAberrationCS, "/Engine/Private/PostProcessChromaticAberration.usf", "MainCS", SF_Compute);

//
// COLOR CONVERSION COMPUTE SHADER
//
class FColorConversionCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FColorConversionCS);
	SHADER_USE_PARAMETER_STRUCT(FColorConversionCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<FFSR_OutputDeviceNitsDim, FFSR_OutputDeviceDim, FFSR_PQDitherDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
		SHADER_PARAMETER_STRUCT_REF(FBlueNoise, BlueNoise)
		SHADER_PARAMETER(float, DitherAmount)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);

		if (PermutationVector.Get<FFSR_OutputDeviceDim>() == EFSR_OutputDevice::sRGB)
		{
			return false; // no color conversion is needed for sRGB
		}

		if (PermutationVector.Get<FFSR_OutputDeviceDim>() != EFSR_OutputDevice::PQ)
		{
			if (PermutationVector.Get<FFSR_PQDitherDim>())
			{
				return false; // do not compile PQ variants for non-PQ output devices
			}
		}


		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), 64);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), 1);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), 1);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};
IMPLEMENT_GLOBAL_SHADER(FColorConversionCS, "/Engine/Private/PostProcessFFX_HDRColorConversion.usf", "MainCS", SF_Compute);


// ===============================================================================================================================================
//
// STATIC HELPERS
//
// ===============================================================================================================================================
static FFSRPassParameters_Grain GetFilmGrainParameters(const FViewInfo& View)
{
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Grain Intensity parameters ----------------
	FVector GrainRandomFullValue;
	{
		uint8 FrameIndexMod8 = 0;
		if (View.State)
		{
			FrameIndexMod8 = View.ViewState->GetFrameIndex(8);
		}
		GrainRandomFromFrame(&GrainRandomFullValue, FrameIndexMod8);
	}

	FVector GrainScaleBiasJitter;
	auto fnGrainPostSettings = [](FVector* RESTRICT const Constant, const FPostProcessSettings* RESTRICT const Settings)
	{
		float GrainJitter = Settings->GrainJitter;
		float GrainIntensity = Settings->GrainIntensity;
		Constant->X = GrainIntensity;
		Constant->Y = 1.0f + (-0.5f * GrainIntensity);
		Constant->Z = GrainJitter;
	};
	fnGrainPostSettings(&GrainScaleBiasJitter, &View.FinalPostProcessSettings);
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Grain Intensity parameters ----------------

	FFSRPassParameters_Grain Parameters;
	Parameters.GrainRandomFull = FVector4(GrainRandomFullValue.X, GrainRandomFullValue.Y, 0, 0);
	Parameters.GrainScaleBiasJitter = FVector4(GrainScaleBiasJitter, 0);
	return Parameters;
}
static FVector4 GetChromaticAberrationParameters(const FPostProcessSettings& PostProcessSettings)
{
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Color Fringe parameters ----------------
	FVector4 ChromaticAberrationParams;
	{
		// for scene color fringe
		// from percent to fraction
		float Offset = 0.0f;
		float StartOffset = 0.0f;
		float Multiplier = 1.0f;

		if (PostProcessSettings.ChromaticAberrationStartOffset < 1.0f - KINDA_SMALL_NUMBER)
		{
			Offset = PostProcessSettings.SceneFringeIntensity * 0.01f;
			StartOffset = PostProcessSettings.ChromaticAberrationStartOffset;
			Multiplier = 1.0f / (1.0f - StartOffset);
		}

		// Wavelength of primaries in nm
		const float PrimaryR = 611.3f;
		const float PrimaryG = 549.1f;
		const float PrimaryB = 464.3f;

		// Simple lens chromatic aberration is roughly linear in wavelength
		float ScaleR = 0.007f * (PrimaryR - PrimaryB);
		float ScaleG = 0.007f * (PrimaryG - PrimaryB);
		ChromaticAberrationParams = FVector4(Offset * ScaleR * Multiplier, Offset * ScaleG * Multiplier, StartOffset, 0.f);
	}
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Color Fringe parameters ----------------
	return ChromaticAberrationParams;
}
static void GetLensParameters(FVector4& LensPrincipalPointOffsetScale, FVector4& LensPrincipalPointOffsetScaleInverse, const FViewInfo& View)
{
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Color Fringe parameters ----------------
	LensPrincipalPointOffsetScale = View.LensPrincipalPointOffsetScale;

	// forward transformation from shader:
	//return LensPrincipalPointOffsetScale.xy + UV * LensPrincipalPointOffsetScale.zw;

	// reverse transformation from shader:
	//return UV*(1.0f/LensPrincipalPointOffsetScale.zw) - LensPrincipalPointOffsetScale.xy/LensPrincipalPointOffsetScale.zw;

	LensPrincipalPointOffsetScaleInverse.X = -View.LensPrincipalPointOffsetScale.X / View.LensPrincipalPointOffsetScale.Z;
	LensPrincipalPointOffsetScaleInverse.Y = -View.LensPrincipalPointOffsetScale.Y / View.LensPrincipalPointOffsetScale.W;
	LensPrincipalPointOffsetScaleInverse.Z = 1.0f / View.LensPrincipalPointOffsetScale.Z;
	LensPrincipalPointOffsetScaleInverse.W = 1.0f / View.LensPrincipalPointOffsetScale.W;
	// this section copy-pasted from PostProcessTonemap.cpp to re-generate Color Fringe parameters ----------------
}
static EFSR_OutputDeviceMaxNits GetOutputDeviceMaxNits(ETonemapperOutputDevice eDevice) // only needed for HDR color conversions
{
	// this will select shader variants so MAX cannot be used.
	EFSR_OutputDeviceMaxNits eMaxNits = EFSR_OutputDeviceMaxNits::EFSR_1000Nits;  // Assume 1000 for the default case.

	switch (eDevice)
	{
	case ETonemapperOutputDevice::ACES1000nitST2084:
	case ETonemapperOutputDevice::ACES1000nitScRGB:
		eMaxNits = EFSR_OutputDeviceMaxNits::EFSR_1000Nits;
		break;
	case ETonemapperOutputDevice::ACES2000nitST2084:
	case ETonemapperOutputDevice::ACES2000nitScRGB:
		eMaxNits = EFSR_OutputDeviceMaxNits::EFSR_2000Nits;
		break;

	case ETonemapperOutputDevice::sRGB:
	case ETonemapperOutputDevice::Rec709:
	case ETonemapperOutputDevice::ExplicitGammaMapping:
	case ETonemapperOutputDevice::LinearEXR:
	case ETonemapperOutputDevice::LinearNoToneCurve:
	case ETonemapperOutputDevice::LinearWithToneCurve:
	default:
		// noop
		break;
	}
	return eMaxNits;
}
static EFSR_MipBiasMethod GetMipBiasMethodEnum(const int32 iMethodValue)
{
	const int32 eMipBiasMethodValue = FMath::Max(0, FMath::Min((int32)EFSR_MipBiasMethod::MANUAL, iMethodValue)); // clamp for casting to enum
	return static_cast<EFSR_MipBiasMethod>(eMipBiasMethodValue);
}

// ===============================================================================================================================================
//
// ENGINE INTERFACE FUNCTIONS
//
// ===============================================================================================================================================
bool IsFidelityFXFSRPassEnabled(const FViewInfo& View)
{
	const bool bFFX_FSRAsPrimaryUpscale = View.PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::FFX_FSR;
	const bool bFFX_FSRAsSecondaryUpscale = View.Family->SecondaryScreenPercentageMethod == ESecondaryScreenPercentageMethod::FFX_FSR;
	const bool bFFX_FSRSelected = bFFX_FSRAsPrimaryUpscale || bFFX_FSRAsSecondaryUpscale;

	const float EffectiveResolutionFraction = GetFidelityFXFSREffectiveResolutionFraction(View);
	if (!bFFX_FSRSelected)
	{
		return false; // also avoids EffectiveResolutionFraction=-1.0f case
	}

	// FSR could be selected as enabled, but it won't run if InputResolution == OutputResolution
	// e.g. in the case of Dynamic Resolution Scaling allowing for 100%
	const bool bFFX_FSRUpscalingEnabled = bFFX_FSRSelected && (EffectiveResolutionFraction != 1.0f);

	// Even if we don't run the upscaling pass for the 100% resolution scaling case,
	// we may still want to run RCAS (and skip the chromatic aberration in the tonemapper pass).
	const bool bRCASEnabled = bFFX_FSRSelected && (GFSR_RCAS > 0);

	return bFFX_FSRUpscalingEnabled || bRCASEnabled;
}

float GetFidelityFXFSREffectiveResolutionFraction(const FViewInfo& View)
{
	const bool bFFX_FSRAsPrimaryUpscale = View.PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::FFX_FSR;
	const bool bFFX_FSRAsSecondaryUpscale = View.Family->SecondaryScreenPercentageMethod == ESecondaryScreenPercentageMethod::FFX_FSR;

	const float EffectivePrimaryResolutionFraction = float(View.ViewRect.Width()) / float(View.GetSecondaryViewRectSize().X);
	const float EffectiveSecondaryResolutionFraction = View.Family->SecondaryViewFraction;

	// if both FSR Primary&Secondary upscales are enabled, one FSR call
	// will handle the effective resolution, which will be the combination of the two.
	float EffectiveResolutionFraction = -1.0f;
	if (bFFX_FSRAsPrimaryUpscale && bFFX_FSRAsSecondaryUpscale)
	{
		EffectiveResolutionFraction = EffectivePrimaryResolutionFraction * EffectiveSecondaryResolutionFraction;
	}
	else if (bFFX_FSRAsPrimaryUpscale)
	{
		// There's no expected upscale pass before or after FSR Primary pass.
		EffectiveResolutionFraction = EffectivePrimaryResolutionFraction;
	}
	else if (bFFX_FSRAsSecondaryUpscale)
	{
		// There could be a TAAU or UE4-Spatial upscale before FSR1.
		// To calculate accurate MipBias values, also consider the primary upscale scaling factor
		// for the EffectiveResolutionFraction
		EffectiveResolutionFraction = EffectivePrimaryResolutionFraction * EffectiveSecondaryResolutionFraction;
	}
	return EffectiveResolutionFraction; // if FSR isn't enabled, returns -1
}

float GetFidelityFXFSRMipBias(const FViewInfo& View)
{
	const bool bFFX_FSRAsPrimaryUpscale = View.PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::FFX_FSR;
	const bool bFFX_FSRAsSecondaryUpscale = View.Family->SecondaryScreenPercentageMethod == ESecondaryScreenPercentageMethod::FFX_FSR;
	if (!bFFX_FSRAsSecondaryUpscale && !bFFX_FSRAsPrimaryUpscale)
	{
		return 0.0f;
	}

	const EFSR_MipBiasMethod eMipBiasMethod = GetMipBiasMethodEnum(GFSR_MipBiasMethod);

	float MipBias = 0.0f;
	switch (eMipBiasMethod)
	{
	case EFSR_MipBiasMethod::AUTOMATIC:
	{
		const float EffectiveResolutionFraction = GetFidelityFXFSREffectiveResolutionFraction(View);
		MipBias = -(FMath::Max(-FMath::Log2(EffectiveResolutionFraction), 0.0f));
		break;
	}
	case EFSR_MipBiasMethod::MANUAL:
	{
		MipBias = GFSR_MipBiasManualOffset;
		break;
	}
	}
	return MipBias;
}

bool ShouldFidelityFXFSROverrideMipBias(const FViewInfo& View)
{
	const EFSR_MipBiasMethod eMipBiasMethod = GetMipBiasMethodEnum(GFSR_MipBiasMethod);
	const bool bFidelityFXSuperResolutionEnabled = (View.PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::FFX_FSR) || (View.Family->SecondaryScreenPercentageMethod == ESecondaryScreenPercentageMethod::FFX_FSR);
	return bFidelityFXSuperResolutionEnabled && eMipBiasMethod != EFSR_MipBiasMethod::DISABLED;
}


// ===============================================================================================================================================
//
// RENDER PASS DEFINITION
//
// ===============================================================================================================================================

DECLARE_GPU_STAT(FidelityFXSuperResolutionPass)

FScreenPassTexture AddFidelityFXSuperResolutionPass(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FFSRInputs& Inputs)
{
	RDG_GPU_STAT_SCOPE(GraphBuilder, FidelityFXSuperResolutionPass);
	check(Inputs.SceneColor.IsValid());
	static TConsoleVariableData<int32>* CVarPostFSRColorFringe = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR.Post.ExperimentalChromaticAberration"));
	static TConsoleVariableData<int32>* CVarPostFSRFilmGrain   = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR.Post.FilmGrain"));
	const FTonemapperOutputDeviceParameters TonemapperOutputDeviceParameters = GetTonemapperOutputDeviceParameters(*View.Family);
	const ETonemapperOutputDevice eTonemapperOutputDevice = (ETonemapperOutputDevice)TonemapperOutputDeviceParameters.OutputDevice;
	const EFSR_OutputDevice eOUTPUT_DEVICE = GetFSROutputDevice(eTonemapperOutputDevice);
	const EFSR_OutputDeviceMaxNits eMAX_NITS = GetOutputDeviceMaxNits(eTonemapperOutputDevice);

	const bool bPlatformPC_D3D                     = View.GetShaderPlatform() == SP_PCD3D_SM5;
	      bool bUSE_FP16                           = (GFSR_FP16 > 0) && bPlatformPC_D3D;
	const bool bRCASEnabled                        = GFSR_RCAS > 0;
	const bool bUSE_RCAS_DENOISE                   = GFSR_RCASDenoise > 0;
	const bool bFORCE_VSPS                         = GFSR_ForcePS > 0;
	const bool bUSE_GRAIN_INTENSITY                = CVarPostFSRFilmGrain->GetValueOnRenderThread() > 0 && View.FinalPostProcessSettings.GrainIntensity > 0.0f;
	const bool bChromaticAberrationPassEnabled     = CVarPostFSRColorFringe->GetValueOnRenderThread() > 0 && View.FinalPostProcessSettings.SceneFringeIntensity > 0.01f;
	const FFSRPassParameters_Grain FilmGrainParams = GetFilmGrainParameters(View);
	const FPostProcessSettings& PostProcessSettings= View.FinalPostProcessSettings;

	const bool bHDRColorConversionPassEnabled = eOUTPUT_DEVICE != EFSR_OutputDevice::sRGB;
	const bool bFSRIsTheLastPass = !bRCASEnabled && !bChromaticAberrationPassEnabled;
	const bool bRCASIsTheLastPass = bRCASEnabled && !bChromaticAberrationPassEnabled;
	const bool bChromAbIsTheLastPass = bChromaticAberrationPassEnabled;


	//---------------------------------------------------------------------------------------------------
	// ! IMPORTANT NOTE !
	// Some NVidia GPUs show image corruption issues on DX11 when FP16 path is enabled with FSR1.
	// To ensure the correctness of the FSR1 pass output, we've added an additional boolean
	// check here to determine whether or not to use FP16 for the DX11/NVidia path.
	// Once this is no longer an issue, the default value of GFSR_FP16OnNvidiaDX11 can be set to 1
	// or this block of code can be entirely removed.
	#if PLATFORM_WINDOWS
	if (IsRHIDeviceNVIDIA())
	{
		static const bool bIsDX11 = FCString::Strcmp(GDynamicRHI->GetName(), TEXT("D3D11")) == 0;
		if (bIsDX11)
		{
			bUSE_FP16 = bUSE_FP16 && (GFSR_FP16OnNvidiaDX11 > 0);
		}
	}
	#endif
	//---------------------------------------------------------------------------------------------------

	//
	// Prepare Intermediate Pass Resources
	//
	FScreenPassTexture ColorConvertedTexture = FScreenPassTexture(Inputs.SceneColor);
	FScreenPassTexture UpscaleTexture = FScreenPassTexture(Inputs.SceneColor);
	FScreenPassTexture SharpenedTexture;
	FScreenPassTexture ColorFringeOutputTexture;

	FRDGTextureDesc FSROutputTextureDesc = Inputs.SceneColor.Texture->Desc;
	FSROutputTextureDesc.Reset();
	FSROutputTextureDesc.Extent = View.UnscaledViewRect.Max;
	FSROutputTextureDesc.ClearValue = FClearValueBinding::Black;
	FSROutputTextureDesc.Flags = TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable;

	if (bHDRColorConversionPassEnabled)
	{
		// FSR expects to work in a perceptual space, preferrably Gamma2.
		// For the HDR outputs, we will need to convert to Gamma2 prior to FSR.
		//
		// scRGB & Linear
		// --------------
		// Since we will convert scRGB into a perceptual space and store in 10:10:10:2_UNORM format
		// a Gamma2 encoded color w/ Rec2020 primaries, override the FSR output texture desc format here.
		// Although, override it only when FSR upscaling is not the last pass, becase RCAS or ChromAb may still
		// utilize the sRGB / 10:10:10:2 input for faster loads, and convert back to scRGB at the end of their invocation.
		// Otherwise, FSR pass will load 10:10:10:2 input, and convert back to scRGB before storing out.
		//
		// PQ
		// --------------
		// The RT format will already be 10:10:10:2, we just need to change the encoding during color conversion.
		if (!bFSRIsTheLastPass)
		{
			FSROutputTextureDesc.Format = PF_A2B10G10R10;
		}


		// prepare color conversion resources
		FRDGTextureDesc ColorConversionTextureDesc = Inputs.SceneColor.Texture->Desc;
		ColorConversionTextureDesc.Format = PF_A2B10G10R10;
		ColorConversionTextureDesc.Flags = TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable;
		ColorConvertedTexture.Texture = GraphBuilder.CreateTexture(ColorConversionTextureDesc, TEXT("FFX-ColorConversion-Output"), ERDGTextureFlags::MultiFrame);
		ColorConvertedTexture.ViewRect = Inputs.SceneColor.ViewRect;
	}
	UpscaleTexture.Texture = GraphBuilder.CreateTexture(FSROutputTextureDesc, TEXT("FFX-FSR-Output"), ERDGTextureFlags::MultiFrame);
	UpscaleTexture.ViewRect = View.UnscaledViewRect;

	if (bRCASEnabled)
	{
		const bool bRCASOutputsToLinearRT = bHDRColorConversionPassEnabled
			&& bRCASIsTheLastPass
			&& (eOUTPUT_DEVICE == EFSR_OutputDevice::scRGB || eOUTPUT_DEVICE == EFSR_OutputDevice::Linear);

		FRDGTextureDesc RCASOutputTextureDesc = UpscaleTexture.Texture->Desc;

		// if we have converted scRGB/Linear input to perceptual space before, we need to convert back to scRGB/Linear
		// again at the end of RCAS, hence, override the output format of RCAS from 10:10:10:2_UNORM to RGBA16F
		if (bRCASOutputsToLinearRT)
		{
			RCASOutputTextureDesc.Format = Inputs.SceneColor.Texture->Desc.Format; /*PF_FloatRGBA*/
		}
		SharpenedTexture = FScreenPassTexture(UpscaleTexture);
		SharpenedTexture.Texture = GraphBuilder.CreateTexture(RCASOutputTextureDesc, TEXT("FFX-RCAS-Output"), ERDGTextureFlags::MultiFrame);
	}

	if (bChromaticAberrationPassEnabled)
	{
		const bool bChromAbPassOutputsToLinearRT = bHDRColorConversionPassEnabled
			&& (eOUTPUT_DEVICE == EFSR_OutputDevice::scRGB || eOUTPUT_DEVICE == EFSR_OutputDevice::Linear);

		FRDGTextureDesc ChromAbOutputTextureDesc = UpscaleTexture.Texture->Desc;

		// if we have converted scRGB/Linear input to perceptual space before, we need to convert back to scRGB/Linear
		// again at the end of ChromAb, hence, override the output format of ChromAb from 10:10:10:2_UNORM to RGBA16F
		if (bChromAbPassOutputsToLinearRT)
		{
			ChromAbOutputTextureDesc.Format = Inputs.SceneColor.Texture->Desc.Format; /*PF_FloatRGBA*/
		}
		ColorFringeOutputTexture = FScreenPassTexture(UpscaleTexture);
		ColorFringeOutputTexture.Texture = GraphBuilder.CreateTexture(ChromAbOutputTextureDesc, TEXT("FFX-ChromAb-Output"), ERDGTextureFlags::MultiFrame);
	}

	const FScreenPassTextureViewport OutputViewport(UpscaleTexture);

	const FScreenPassTextureViewport InputViewport(Inputs.SceneColor);

	// We don't need to run the Upscale pass if both input and output viewrects are identical.
	// This case can happen for Dynamic Resolution Scaling (DRS) 100% scaling while FSR1 is on.
	// Note that we don't compare Rect.Width() and Rect.Height() here since they could be same rect
	// dimensions but OutputViewport may contain translation (for cinematic cameras for example).
	// - i.e. Min & Max values could be different but Width & Height could be the same
	const bool bFSRSkipUpscale = InputViewport.Rect == OutputViewport.Rect;
	if (bFSRSkipUpscale)
	{
		// If we're not actually going to do upscaling, and RCAS/ChromAb are both off
		// (== FSR is the last pass), then we don't need to run anything and early-out here.
		if (bFSRIsTheLastPass)
		{
			return Inputs.SceneColor;
		}

		// If we're skipping upscaling as well as RCAS, then don't run Post-FFX ChromaticAberration
		// alone since there's no need to skip the ChromAb in the Tonemapper in this case.
		if (!bRCASEnabled)
		{
			return Inputs.SceneColor;
		}
	}

	FScreenPassTextureViewportParameters PassOutputViewportParams = GetScreenPassTextureViewportParameters(OutputViewport);

	// --------------------------------------------------------------------------------------------------------------------

	FRDGTextureRef CurrentInputTexture = Inputs.SceneColor.Texture;
	FScreenPassRenderTarget FinalOutput;

	//
	// [HDR-only] Color Conversion Pass
	//
	if (bHDRColorConversionPassEnabled)
	{
		// --------------------------------------------------------------------------------------
		// HDR/scRGB
		// ---------------------------------------------------------------------------------------
		// In scRGB, there could be input ranging between [-0.5, 12.5].
		// scRGB uses sRGB primaries and is a linear color space, however,
		// FSR expects perceptual space (preferrably Gamma2/sRGB) and values in [0-1] range.
		//
		// Here, we will convert the scRGB input into a Gamma2 encoded perceptual space.
		// During the conversion, we will change the sRGB primaries into Rec2020 primaries
		// and clip the remaining negative values, while also normalizing [0,1]
		//
		// ---------------------------------------------------------------------------------------
		// HDR/PQ
		// ---------------------------------------------------------------------------------------
		// The render target format won't change, although the signal encoding will be Gamma2.
		// UE4 tonemapper will output normalized (depending on 1000 or 2000 nits) PQ space color.
		// The conversion shader will do the following and store the value: PQ -> Linear -> Gamma2
		//
		// ---------------------------------------------------------------------------------------
		// HDR/Linear
		// ---------------------------------------------------------------------------------------
		// Linear can contain values in [0, FP16_MAX)
		// we will use FSR1/SRTM (Simple Reversible Tonemapper) to normalize the lienar values
		// before storing in a 10:10:10:2_UNORM intermediate target at the end of this pass.
		//
		// ======================================================================================
		//
		// The last pass of the FSR1 chain will convert back to the original device output space.
		//
		// ======================================================================================

		// Set parameters
		FColorConversionCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FColorConversionCS::FParameters>();
		PassParameters->InputTexture = CurrentInputTexture;
		PassParameters->OutputTexture = GraphBuilder.CreateUAV(ColorConvertedTexture.Texture);
		FBlueNoise BlueNoise;
		InitializeBlueNoise(BlueNoise);
		PassParameters->BlueNoise = CreateUniformBufferImmediate(BlueNoise, EUniformBufferUsage::UniformBuffer_SingleFrame);
		PassParameters->DitherAmount = FMath::Min(1.0f, FMath::Max(0.0f, GFSR_HDR_PQDither));

		// Set shader variant
		FColorConversionCS::FPermutationDomain CSPermutationVector;
		CSPermutationVector.Set<FFSR_OutputDeviceDim>(eOUTPUT_DEVICE);
		CSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(eMAX_NITS);
		if (eOUTPUT_DEVICE == EFSR_OutputDevice::PQ)
		{
			CSPermutationVector.Set<FFSR_PQDitherDim>(PassParameters->DitherAmount > 0);
		}

		// Dispatch
		TShaderMapRef<FColorConversionCS> ComputeShaderFSR(View.ShaderMap, CSPermutationVector);
		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("FidelityFX-FSR1/HDRColorConversion (Gamma2) OutputDevice=%d DeviceMaxNits=%d (CS)"
				, TonemapperOutputDeviceParameters.OutputDevice
				, (eMAX_NITS == EFSR_OutputDeviceMaxNits::EFSR_1000Nits ? 1000 : 2000)
			),
			ComputeShaderFSR,
			PassParameters,
			FComputeShaderUtils::GetGroupCount(InputViewport.Rect.Size(), 16)
		);

		CurrentInputTexture = ColorConvertedTexture.Texture;
	}

	//
	// FidelityFX Super Resolution (FSR) Pass
	//
	AU1 const0[4];
	AU1 const1[4];
	AU1 const2[4];
	AU1 const3[4]; // Configure FSR
	if(!bFSRSkipUpscale)
	{
		FsrEasuCon(const0, const1, const2, const3,
			  static_cast<AF1>(InputViewport.Rect.Width())
			, static_cast<AF1>(InputViewport.Rect.Height()) // current frame render resolution
			, static_cast<AF1>(Inputs.SceneColor.Texture->Desc.Extent.X)
			, static_cast<AF1>(Inputs.SceneColor.Texture->Desc.Extent.Y) // input container resolution (for DRS)
			, static_cast<AF1>(OutputViewport.Rect.Width())
			, static_cast<AF1>(OutputViewport.Rect.Height()) // upscaled-to resolution
		);

		const bool bUseIntermediateRT = (bRCASEnabled || bChromaticAberrationPassEnabled) || !Inputs.OverrideOutput.IsValid();

		FScreenPassRenderTarget Output = bUseIntermediateRT
			? FScreenPassRenderTarget(UpscaleTexture.Texture, ERenderTargetLoadAction::ENoAction)
			: Inputs.OverrideOutput;

		const bool bOutputSupportsUAV = (Output.Texture->Desc.Flags & TexCreate_UAV) == TexCreate_UAV;
		const float MipBiasValue = GetFidelityFXFSRMipBias(View);

		// VS-PS
		if (!bOutputSupportsUAV || bFORCE_VSPS)
		{
			FFSRPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSRPS::FParameters>();
			for (int i = 0; i < 4; i++)
			{
				PassParameters->FSR.Const0[i] = const0[i];
				PassParameters->FSR.Const1[i] = const1[i];
				PassParameters->FSR.Const2[i] = const2[i];
				PassParameters->FSR.Const3[i] = const3[i];
			}
			PassParameters->FSR.InputTexture = CurrentInputTexture;
			PassParameters->FSR.samLinearClamp = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
			PassParameters->FSR.FilmGrain = FilmGrainParams;
			PassParameters->FSR.VPColor_ExtentInverse = PassOutputViewportParams.ExtentInverse;
			PassParameters->FSR.VPColor_ViewportMin = PassOutputViewportParams.ViewportMin;
			PassParameters->RenderTargets[0] = FRenderTargetBinding(Output.Texture, ERenderTargetLoadAction::ENoAction);

			FFSRPS::FPermutationDomain PSPermutationVector;
			PSPermutationVector.Set<FFSR_UseFP16Dim>(bUSE_FP16);
			PSPermutationVector.Set<FFSR_GrainDim>(bUSE_GRAIN_INTENSITY && bFSRIsTheLastPass);
			PSPermutationVector.Set<FFSR_OutputDeviceConversion>(bHDRColorConversionPassEnabled && bFSRIsTheLastPass);
			if (PSPermutationVector.Get<FFSR_OutputDeviceConversion>())
			{
				PSPermutationVector.Set<FFSR_OutputDeviceDim>(eOUTPUT_DEVICE);
				PSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(eMAX_NITS);
			}

			const bool bFSRWithGrain = PSPermutationVector.Get<FFSR_GrainDim>();

			TShaderMapRef<FFSRPS> PixelShader(View.ShaderMap, PSPermutationVector);

			AddDrawScreenPass(GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR1/Upscale %dx%d -> %dx%d MipBias=%.2f GrainIntensity=%d (PS)"
					, InputViewport.Rect.Width(), InputViewport.Rect.Height()
					, OutputViewport.Rect.Width(), OutputViewport.Rect.Height()
					, MipBiasValue
					, (bFSRWithGrain ? 1 : 0)),
				View, OutputViewport, InputViewport,
				PixelShader, PassParameters,
				EScreenPassDrawFlags::None
			);
		}

		// CS
		else
		{
			FFSRCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSRCS::FParameters>();

			for (int i = 0; i < 4; i++)
			{
				PassParameters->FSR.Const0[i] = const0[i];
				PassParameters->FSR.Const1[i] = const1[i];
				PassParameters->FSR.Const2[i] = const2[i];
				PassParameters->FSR.Const3[i] = const3[i];
			}
			PassParameters->FSR.InputTexture = CurrentInputTexture;
			PassParameters->FSR.samLinearClamp = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
			PassParameters->FSR.FilmGrain = FilmGrainParams;
			PassParameters->FSR.VPColor_ExtentInverse = PassOutputViewportParams.ExtentInverse;
			PassParameters->FSR.VPColor_ViewportMin = PassOutputViewportParams.ViewportMin;
			PassParameters->OutputTexture = GraphBuilder.CreateUAV(UpscaleTexture.Texture);

			FFSRCS::FPermutationDomain CSPermutationVector;
			CSPermutationVector.Set<FFSR_UseFP16Dim>(bUSE_FP16);
			CSPermutationVector.Set<FFSR_GrainDim>(bUSE_GRAIN_INTENSITY && bFSRIsTheLastPass);
			CSPermutationVector.Set<FFSR_OutputDeviceConversion>(bHDRColorConversionPassEnabled && bFSRIsTheLastPass);
			if (CSPermutationVector.Get<FFSR_OutputDeviceConversion>())
			{
				CSPermutationVector.Set<FFSR_OutputDeviceDim>(eOUTPUT_DEVICE);
				CSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(eMAX_NITS);
			}

			const bool bFSRWithGrain = CSPermutationVector.Get<FFSR_GrainDim>();

			TShaderMapRef<FFSRCS> ComputeShaderFSR(View.ShaderMap, CSPermutationVector);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR1/Upscale %dx%d -> %dx%d MipBias=%.2f GrainIntensity=%d (CS)"
					, InputViewport.Rect.Width(), InputViewport.Rect.Height()
					, OutputViewport.Rect.Width(), OutputViewport.Rect.Height()
					, MipBiasValue
					, (bFSRWithGrain ? 1 : 0)),
				ComputeShaderFSR,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(OutputViewport.Rect.Size(), 16));

		}
		FinalOutput = Output; // RCAS will override this if enabled
		CurrentInputTexture = Output.Texture;
	}


	//
	// FidelityFX Robust Contrast Adaptive Sharpening (RCAS) Pass
	//
	if (bRCASEnabled)
	{
		const bool bUSE_RCAS_GRAIN_INTENSITY = !bChromaticAberrationPassEnabled && bUSE_GRAIN_INTENSITY;

		FsrRcasCon(const0, GFSR_Sharpness);

		FScreenPassRenderTarget Output = bChromaticAberrationPassEnabled
			? FScreenPassRenderTarget(SharpenedTexture.Texture, ERenderTargetLoadAction::ENoAction)
			: Inputs.OverrideOutput; // if no ChromAb, RCAS is last pass so override output
		if (!Output.IsValid())
		{
			Output = FScreenPassRenderTarget(SharpenedTexture.Texture, ERenderTargetLoadAction::ENoAction);
		}
		const bool bOutputSupportsUAV = (Output.Texture->Desc.Flags & TexCreate_UAV) == TexCreate_UAV;

		// VS-PS
		if (!bOutputSupportsUAV || bFORCE_VSPS)
		{
			FRCASPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FRCASPS::FParameters>();

			// set pass inputs
			for (int i = 0; i < 4; i++) { PassParameters->RCAS.Const0[i] = const0[i]; }
			PassParameters->RCAS.InputTexture = CurrentInputTexture;
			PassParameters->RCAS.FilmGrain = FilmGrainParams;
			PassParameters->RCAS.VPColor_ExtentInverse = PassOutputViewportParams.ExtentInverse;
			PassParameters->RenderTargets[0] = FRenderTargetBinding(Output.Texture, ERenderTargetLoadAction::ENoAction);

			// grab shaders
			FRCASPS::FPermutationDomain PSPermutationVector;
			PSPermutationVector.Set<FFSR_UseFP16Dim>(bUSE_FP16);
			PSPermutationVector.Set<FFSR_GrainDim>(bUSE_RCAS_GRAIN_INTENSITY);
			PSPermutationVector.Set<FRCAS_DenoiseDim>(bUSE_RCAS_DENOISE);
			PSPermutationVector.Set<FFSR_OutputDeviceConversion>(bHDRColorConversionPassEnabled && bRCASIsTheLastPass);
			if (PSPermutationVector.Get<FFSR_OutputDeviceConversion>())
			{
				PSPermutationVector.Set<FFSR_OutputDeviceDim>(eOUTPUT_DEVICE);
				PSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(eMAX_NITS);
			}

			TShaderMapRef<FRCASPS> PixelShader(View.ShaderMap, PSPermutationVector);

			AddDrawScreenPass(GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR1/RCAS Sharpness=%.2f OutputDevice=%d GrainIntensity=%d (PS)"
					, GFSR_Sharpness
					, TonemapperOutputDeviceParameters.OutputDevice
					, ((bUSE_RCAS_GRAIN_INTENSITY) ? 1 : 0)),
				View, OutputViewport, OutputViewport,
				PixelShader, PassParameters,
				EScreenPassDrawFlags::None
			);
		}

		// CS
		else
		{
			FRCASCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FRCASCS::FParameters>();

			for (int i = 0; i < 4; i++)
			{
				PassParameters->RCAS.Const0[i] = const0[i];
			}
			PassParameters->RCAS.InputTexture = CurrentInputTexture;
			PassParameters->RCAS.FilmGrain = FilmGrainParams;
			PassParameters->RCAS.VPColor_ExtentInverse = PassOutputViewportParams.ExtentInverse;
			PassParameters->OutputTexture = GraphBuilder.CreateUAV(SharpenedTexture.Texture);

			FRCASCS::FPermutationDomain CSPermutationVector;
			CSPermutationVector.Set<FFSR_UseFP16Dim>(bUSE_FP16);
			CSPermutationVector.Set<FRCAS_DenoiseDim>(bUSE_RCAS_DENOISE);
			CSPermutationVector.Set<FFSR_GrainDim>(bUSE_RCAS_GRAIN_INTENSITY);
			CSPermutationVector.Set<FFSR_OutputDeviceConversion>(bHDRColorConversionPassEnabled && bRCASIsTheLastPass);
			if (CSPermutationVector.Get<FFSR_OutputDeviceConversion>())
			{
				CSPermutationVector.Set<FFSR_OutputDeviceDim>(eOUTPUT_DEVICE);
				CSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(eMAX_NITS);
			}

			TShaderMapRef<FRCASCS> ComputeShaderRCASPass(View.ShaderMap, CSPermutationVector);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR1/RCAS Sharpness=%.2f OutputDevice=%d GrainIntensity=%d (CS)"
					, GFSR_Sharpness
					, TonemapperOutputDeviceParameters.OutputDevice, ((bUSE_RCAS_GRAIN_INTENSITY) ? 1 : 0)),
				ComputeShaderRCASPass,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(OutputViewport.Rect.Size(), 16));
		}

		FinalOutput = Output;
		CurrentInputTexture = Output.Texture;
	}


	//
	// Post-upscale/sharpen Chromatic Aberration Pass
	//
	if (bChromaticAberrationPassEnabled)
	{
		FRHISamplerState* BilinearClampSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();

		FScreenPassRenderTarget Output = Inputs.OverrideOutput;
		if (!Output.IsValid())
		{
			Output = FScreenPassRenderTarget(ColorFringeOutputTexture.Texture, ERenderTargetLoadAction::ENoAction);
		}

		const bool bOutputSupportsUAV = (Output.Texture->Desc.Flags & TexCreate_UAV) == TexCreate_UAV;

		// VS-PS
		if(!bOutputSupportsUAV)
		{
			FChromaticAberrationPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FChromaticAberrationPS::FParameters>();

			// set pass inputs
			PassParameters->ColorTexture = CurrentInputTexture;
			PassParameters->ColorSampler = BilinearClampSampler;
			PassParameters->RenderTargets[0] = FRenderTargetBinding(Output.Texture, ERenderTargetLoadAction::ENoAction);
			PassParameters->Color = GetScreenPassTextureViewportParameters(FScreenPassTextureViewport(PassParameters->ColorTexture));
			PassParameters->ChromaticAberrationParams = GetChromaticAberrationParameters(View.FinalPostProcessSettings);
			PassParameters->FilmGrain = FilmGrainParams;
			GetLensParameters(PassParameters->LensPrincipalPointOffsetScale, PassParameters->LensPrincipalPointOffsetScaleInverse, View);


			// grab shaders
			FChromaticAberrationPS::FPermutationDomain PSPermutationVector;
			PSPermutationVector.Set<FFSR_GrainDim>(bUSE_GRAIN_INTENSITY);
			PSPermutationVector.Set<FFSR_OutputDeviceDim>(eOUTPUT_DEVICE);
			if (bHDRColorConversionPassEnabled)
			{
				PSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(eMAX_NITS);
			}

			TShaderMapRef<FChromaticAberrationPS> PixelShader(View.ShaderMap, PSPermutationVector);

			AddDrawScreenPass(GraphBuilder,
				RDG_EVENT_NAME("Post-FidelityFX ChromaticAberration GrainIntensity=%d OutputDevice=%d (PS)"
					, ((bUSE_GRAIN_INTENSITY) ? 1 : 0)
					, TonemapperOutputDeviceParameters.OutputDevice
				), View, OutputViewport, OutputViewport,
				PixelShader, PassParameters,
				EScreenPassDrawFlags::None
			);
		}

		// CS
		else
		{
			FChromaticAberrationCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FChromaticAberrationCS::FParameters>();

			// set pass inputs
			PassParameters->ColorTexture = CurrentInputTexture;
			PassParameters->ColorSampler = BilinearClampSampler;
			PassParameters->Color = GetScreenPassTextureViewportParameters(FScreenPassTextureViewport(PassParameters->ColorTexture));
			PassParameters->ChromaticAberrationParams = GetChromaticAberrationParameters(View.FinalPostProcessSettings);
			PassParameters->OutputTexture = GraphBuilder.CreateUAV(Output.Texture);
			PassParameters->FilmGrain = FilmGrainParams;
			GetLensParameters(PassParameters->LensPrincipalPointOffsetScale, PassParameters->LensPrincipalPointOffsetScaleInverse, View);


			FChromaticAberrationCS::FPermutationDomain CSPermutationVector;
			CSPermutationVector.Set<FFSR_GrainDim>(bUSE_GRAIN_INTENSITY);
			CSPermutationVector.Set<FFSR_OutputDeviceDim>(eOUTPUT_DEVICE);
			if (bHDRColorConversionPassEnabled)
			{
				CSPermutationVector.Set<FFSR_OutputDeviceNitsDim>(eMAX_NITS);
			}

			TShaderMapRef<FChromaticAberrationCS> ComputeShaderRCASPass(View.ShaderMap, CSPermutationVector);

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("Post-FidelityFX ChromaticAberration GrainIntensity=%d OutputDevice=%d (CS)"
					, ((bUSE_GRAIN_INTENSITY) ? 1 : 0)
					, TonemapperOutputDeviceParameters.OutputDevice
				),
				ComputeShaderRCASPass,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(OutputViewport.Rect.Size(), 16));
		}

		FinalOutput = Output;
	}

	return MoveTemp(FinalOutput);
}
