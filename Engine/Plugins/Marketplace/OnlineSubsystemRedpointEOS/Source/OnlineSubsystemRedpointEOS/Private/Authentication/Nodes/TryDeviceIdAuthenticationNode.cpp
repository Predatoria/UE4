// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/TryDeviceIdAuthenticationNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"

EOS_ENABLE_STRICT_WARNINGS

void FTryDeviceIdAuthenticationNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    FAuthenticationGraphRefreshEOSCredentials RefreshCallback = FAuthenticationGraphRefreshEOSCredentials::CreateLambda(
        [](const TSharedPtr<FAuthenticationGraphRefreshEOSCredentialsInfo> &Info) {
            UE_LOG(LogEOS, Verbose, TEXT("Refreshing device ID login for user..."));
            FEOSAuthentication::DoRequest(
                Info->EOSConnect,
                TEXT("Anonymous"),
                TEXT(""),
                EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN,
                FEOSAuth_DoRequestComplete::CreateLambda([Info](const EOS_Connect_LoginCallbackInfo *Data) {
                    // FEOSAuthentication does EOS_Connect_Login, which is all we need to do.
                    if (Data->ResultCode == EOS_EResult::EOS_Success)
                    {
                        UE_LOG(LogEOS, Verbose, TEXT("Successfully refreshed device ID login for user"));
                        Info->OnComplete.ExecuteIfBound(true);
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Failed to refresh device ID login for user, got result code: %s"),
                            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                        Info->OnComplete.ExecuteIfBound(false);
                    }
                }));
        });

    FEOSAuthentication::DoRequest(
        State->EOSConnect,
        TEXT("Anonymous"),
        TEXT(""),
        EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN,
        FEOSAuth_DoRequestComplete::CreateLambda([WeakThis = GetWeakThis(this), State, OnDone, RefreshCallback](
                                                     const EOS_Connect_LoginCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_InvalidUser || Data->ResultCode == EOS_EResult::EOS_Success)
                {
                    TMap<FString, FString> Attributes;
                    Attributes.Add(EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH, TEXT("deviceId"));
                    State->AddEOSConnectCandidate(
                        NSLOCTEXT("OnlineSubsystemRedpointEOS", "PlatformDisplayName_DeviceId", "This Device"),
                        Attributes,
                        Data,
                        RefreshCallback,
                        NAME_None,
                        EAuthenticationGraphEOSCandidateType::DeviceId);
                }
                else
                {
                    State->ErrorMessages.Add(FString::Printf(
                        TEXT("Error while authenticating against device ID: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
                }

                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
            }
        }));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION