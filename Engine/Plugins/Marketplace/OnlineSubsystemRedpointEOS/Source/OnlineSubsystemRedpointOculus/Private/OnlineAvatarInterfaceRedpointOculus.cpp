// Copyright June Rhodes. All Rights Reserved.

#include "OnlineAvatarInterfaceRedpointOculus.h"

#if EOS_OCULUS_ENABLED

#include "Containers/Ticker.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "LogRedpointOculus.h"
#include "OVR_Platform.h"
#include "OnlineSubsystemOculus.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSTextureLoader.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

void FOnlineAvatarInterfaceRedpointOculus::OnDownloadAvatarComplete(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr Request,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpResponsePtr Response,
    bool bConnectedSuccessfully,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSoftObjectPtr<UTexture> DefaultTexture,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnGetAvatarComplete OnComplete)
{
    if (!bConnectedSuccessfully || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatar: HTTP request to download avatar from Oculus failed."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return;
    }

    UTexture *LoadedTexture = FEOSTextureLoader::LoadTextureFromHttpResponse(Response);
    if (IsValid(LoadedTexture))
    {
        OnComplete.ExecuteIfBound(true, LoadedTexture);
    }
    else
    {
        OnComplete.ExecuteIfBound(false, DefaultTexture);
    }
}

bool FOnlineAvatarInterfaceRedpointOculus::GetAvatar(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    TSoftObjectPtr<UTexture> DefaultTexture,
    FOnGetAvatarComplete OnComplete)
{
    if (TargetUserId.GetType() != OCULUS_SUBSYSTEM)
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatar: TargetUserId is non-Oculus user."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }
    if (!LocalUserId.DoesSharedInstanceExist())
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatar: LocalUserId is not a shareable FUniqueNetId."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }
    if (!TargetUserId.DoesSharedInstanceExist())
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatar: TargetUserId is not a shareable FUniqueNetId."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    // We know that the Oculus net ID returns the ovrID address from GetBytes.
    ovrID OculusUserId = *reinterpret_cast<const ovrID *>(TargetUserId.GetBytes());

    // Oculus is a singleton, so this is a safe way of accessing the OSS instance.
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    FOnlineSubsystemOculus *OSSSubsystem = (FOnlineSubsystemOculus *)IOnlineSubsystem::Get(OCULUS_SUBSYSTEM);
    if (OSSSubsystem == nullptr)
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatar: Oculus subsystem is not available."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    ovrRequest UserInfoRequest = ovr_User_Get(OculusUserId);
    OSSSubsystem->AddRequestDelegate(
        UserInfoRequest,
        FOculusMessageOnCompleteDelegate::CreateLambda([WeakThis = GetWeakThis(this),
                                                        TargetUserIdShared = TargetUserId.AsShared(),
                                                        OnComplete,
                                                        DefaultTexture](ovrMessageHandle Message, bool bIsError) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (bIsError || ovr_Message_IsError(Message))
                {
                    ovrErrorHandle ErrorHandle = ovr_Message_GetError(Message);
                    int ErrorCode = ovr_Error_GetCode(ErrorHandle);
                    int HttpErrorCode = ovr_Error_GetHttpCode(ErrorHandle);
                    FString ErrorMessage = ANSI_TO_TCHAR(ovr_Error_GetMessage(ErrorHandle));
                    FString DisplayableErrorMessage = ANSI_TO_TCHAR(ovr_Error_GetDisplayableMessage(ErrorHandle));
                    UE_LOG(
                        LogRedpointOculus,
                        Error,
                        TEXT("GetAvatar: Error from Oculus API while fetching user ID '%s'. Code: %d, HTTP Code: %d, "
                             "Message: '%s', Displayable Message: '%s'"),
                        *TargetUserIdShared->ToString(),
                        ErrorCode,
                        HttpErrorCode,
                        *ErrorMessage,
                        *DisplayableErrorMessage);
                    OnComplete.ExecuteIfBound(false, DefaultTexture);
                    return;
                }

                ovrUserHandle UserHandle = ovr_Message_GetUser(Message);
                const char *ImageUrl = ovr_User_GetImageUrl(UserHandle);

                auto Request = FHttpModule::Get().CreateRequest();
                Request->SetVerb("GET");
                Request->SetURL(ImageUrl);
                Request->OnProcessRequestComplete().BindThreadSafeSP(
                    This.ToSharedRef(),
                    &FOnlineAvatarInterfaceRedpointOculus::OnDownloadAvatarComplete,
                    DefaultTexture,
                    OnComplete);
                Request->ProcessRequest();
            }
        }));
    return true;
}

bool FOnlineAvatarInterfaceRedpointOculus::GetAvatarUrl(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    FString DefaultAvatarUrl,
    FOnGetAvatarUrlComplete OnComplete)
{
    if (TargetUserId.GetType() != OCULUS_SUBSYSTEM)
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatarUrl: TargetUserId is non-Oculus user."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }
    if (!LocalUserId.DoesSharedInstanceExist())
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatarUrl: LocalUserId is not a shareable FUniqueNetId."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }
    if (!TargetUserId.DoesSharedInstanceExist())
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatarUrl: TargetUserId is not a shareable FUniqueNetId."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }

    // We know that the Oculus net ID returns the ovrID address from GetBytes.
    ovrID OculusUserId = *reinterpret_cast<const ovrID *>(TargetUserId.GetBytes());

    // Oculus is a singleton, so this is a safe way of accessing the OSS instance.
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    FOnlineSubsystemOculus *OSSSubsystem = (FOnlineSubsystemOculus *)IOnlineSubsystem::Get(OCULUS_SUBSYSTEM);
    if (OSSSubsystem == nullptr)
    {
        UE_LOG(LogRedpointOculus, Error, TEXT("GetAvatarUrl: Oculus subsystem is not available."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }

    ovrRequest UserInfoRequest = ovr_User_Get(OculusUserId);
    OSSSubsystem->AddRequestDelegate(
        UserInfoRequest,
        FOculusMessageOnCompleteDelegate::CreateLambda([WeakThis = GetWeakThis(this),
                                                        TargetUserIdShared = TargetUserId.AsShared(),
                                                        OnComplete,
                                                        DefaultAvatarUrl](ovrMessageHandle Message, bool bIsError) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (bIsError || ovr_Message_IsError(Message))
                {
                    ovrErrorHandle ErrorHandle = ovr_Message_GetError(Message);
                    int ErrorCode = ovr_Error_GetCode(ErrorHandle);
                    int HttpErrorCode = ovr_Error_GetHttpCode(ErrorHandle);
                    FString ErrorMessage = ANSI_TO_TCHAR(ovr_Error_GetMessage(ErrorHandle));
                    FString DisplayableErrorMessage = ANSI_TO_TCHAR(ovr_Error_GetDisplayableMessage(ErrorHandle));
                    UE_LOG(
                        LogRedpointOculus,
                        Error,
                        TEXT(
                            "GetAvatarUrl: Error from Oculus API while fetching user ID '%s'. Code: %d, HTTP Code: %d, "
                            "Message: '%s', Displayable Message: '%s'"),
                        *TargetUserIdShared->ToString(),
                        ErrorCode,
                        HttpErrorCode,
                        *ErrorMessage,
                        *DisplayableErrorMessage);
                    OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
                    return;
                }

                ovrUserHandle UserHandle = ovr_Message_GetUser(Message);
                const char *ImageUrl = ovr_User_GetImageUrl(UserHandle);

                OnComplete.ExecuteIfBound(true, ImageUrl);
            }
        }));
    return true;
}

#endif
