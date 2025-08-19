// Copyright June Rhodes. All Rights Reserved.

#include "PlatformHelpers.h"

#include "Misc/DataDrivenPlatformInfoRegistry.h"
#include "PlatformInfo.h"

TArray<FName> GetAllPlatformNames()
{
    TArray<FName> Results;
#if defined(UE_5_0_OR_LATER)
#if DDPI_HAS_EXTENDED_PLATFORMINFO_DATA
    for (const auto &PlatformInfo : PlatformInfo::GetPlatformInfoArray())
    {
        const FDataDrivenPlatformInfo &PlatInfo =
            FDataDrivenPlatformInfoRegistry::GetPlatformInfo(PlatformInfo->VanillaInfo->Name);

        if (PlatformInfo->VanillaInfo->PlatformType == EBuildTargetType::Game &&
            PlatformInfo->VanillaInfo->Name.ToString() != "AllDesktop")
        {
            if (PlatformInfo->VanillaInfo->Name.ToString().EndsWith("NoEditor"))
            {
                Results.Add(FName(*PlatformInfo->VanillaInfo->Name.ToString().Mid(
                    0,
                    PlatformInfo->VanillaInfo->Name.ToString().Len() - 8)));
            }
            else
            {
                Results.Add(PlatformInfo->VanillaInfo->Name);
            }
        }
    }
#endif
#else
    const TArray<PlatformInfo::FPlatformInfo> &PlatformInfos = PlatformInfo::GetPlatformInfoArray();
    for (const auto &PlatformInfo : PlatformInfos)
    {
        if (PlatformInfo.PlatformType == EBuildTargetType::Game &&
            PlatformInfo.VanillaPlatformName.ToString() != "AllDesktop")
        {
            if (PlatformInfo.VanillaPlatformName.ToString().EndsWith("NoEditor"))
            {
                Results.Add(FName(*PlatformInfo.VanillaPlatformName.ToString().Mid(
                    0,
                    PlatformInfo.VanillaPlatformName.ToString().Len() - 8)));
            }
            else
            {
                Results.Add(PlatformInfo.VanillaPlatformName);
            }
        }
    }
#endif
    return Results;
}

TArray<FName> GetConfidentialPlatformNames()
{
    TArray<FName> Results;
#if defined(UE_5_0_OR_LATER)
#if DDPI_HAS_EXTENDED_PLATFORMINFO_DATA
    for (const auto &PlatformInfo : PlatformInfo::GetPlatformInfoArray())
    {
        const FDataDrivenPlatformInfo &PlatInfo =
            FDataDrivenPlatformInfoRegistry::GetPlatformInfo(PlatformInfo->VanillaInfo->Name);

        if (PlatformInfo->VanillaInfo->PlatformType == EBuildTargetType::Game &&
            (PlatInfo.bIsConfidential || PlatformInfo->VanillaInfo->Name.IsEqual(FName(TEXT("Switch")))))
        {
            Results.Add(PlatformInfo->VanillaInfo->Name);
        }
    }
#endif
#else
    const TArray<PlatformInfo::FPlatformInfo> &PlatformInfos = PlatformInfo::GetPlatformInfoArray();
    for (const auto &PlatformInfo : PlatformInfos)
    {
        if (PlatformInfo.PlatformType == EBuildTargetType::Game &&
            (PlatformInfo.bIsConfidential || PlatformInfo.VanillaPlatformName.IsEqual(FName(TEXT("Switch")))))
        {
            Results.Add(PlatformInfo.VanillaPlatformName);
        }
    }
#endif
    return Results;
}