// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/SanctionCheckPhase.h"

#if EOS_VERSION_AT_LEAST(1, 11, 0)

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

void FSanctionCheckPhase::RegisterRoutes(UEOSControlChannel *ControlChannel)
{
}

void FSanctionCheckPhase::Start(const TSharedRef<FAuthVerificationPhaseContext> &Context)
{
    Context->SetVerificationStatus(EUserVerificationStatus::CheckingSanctions);

    UE_LOG(
        LogEOSNetworkAuth,
        Verbose,
        TEXT("Server authentication: %s: Checking to see if user has any BAN sanctions..."),
        *Context->GetUserId()->ToString());

    FOnlineSubsystemEOS *OSS = Context->GetOSS();
    if (OSS == nullptr)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Disconnecting client because we could not access our online subsystem."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessOSS);
        return;
    }

    EOS_HSanctions EOSSanctions = EOS_Platform_GetSanctionsInterface(OSS->GetPlatformInstance());

    EOS_Sanctions_QueryActivePlayerSanctionsOptions Opts = {};
    Opts.ApiVersion = EOS_SANCTIONS_QUERYACTIVEPLAYERSANCTIONS_API_LATEST;
    Opts.TargetUserId = Context->GetUserId()->GetProductUserId();
    auto Identity = OSS->GetIdentityInterface();
    checkf(Identity.IsValid(), TEXT("Identity interface is not valid"));
    TArray<TSharedPtr<FUserOnlineAccount>> Accounts = Identity->GetAllUserAccounts();
    checkf(Accounts.Num() > 0, TEXT("No locally signed in account when checking sanctions"));
    auto LocalUserId = StaticCastSharedRef<const FUniqueNetIdEOS>(Accounts[0]->GetUserId());
    if (LocalUserId->IsDedicatedServer())
    {
        Opts.LocalUserId = nullptr;
    }
    else
    {
        Opts.LocalUserId = LocalUserId->GetProductUserId();
    }

    EOSRunOperation<
        EOS_HSanctions,
        EOS_Sanctions_QueryActivePlayerSanctionsOptions,
        EOS_Sanctions_QueryActivePlayerSanctionsCallbackInfo>(
        EOSSanctions,
        &Opts,
        &EOS_Sanctions_QueryActivePlayerSanctions,
        [WeakThis = GetWeakThis(this), EOSSanctions, Context](
            const EOS_Sanctions_QueryActivePlayerSanctionsCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_AccessDenied ||
                    Data->ResultCode == EOS_EResult::EOS_NotConfigured ||
                    Data->ResultCode == EOS_EResult::EOS_MissingPermissions)
                {
                    UE_LOG(
                        LogEOSNetworkAuth,
                        Warning,
                        TEXT("Server authentication: %s: Server's client policy did not have permission to query "
                             "sanctions. Please update the client policy in the EOS Developer Portal. Bypassing "
                             "sanctions check due to lack of API permissions."),
                        *Context->GetUserId()->ToString());
                    Context->Finish(EAuthPhaseFailureCode::Success);
                }
                else if (Data->ResultCode == EOS_EResult::EOS_Success)
                {
                    bool bIsDenied = false;
                    EOS_Sanctions_GetPlayerSanctionCountOptions CountOpts = {};
                    CountOpts.ApiVersion = EOS_SANCTIONS_GETPLAYERSANCTIONCOUNT_API_LATEST;
                    CountOpts.TargetUserId = Context->GetUserId()->GetProductUserId();
                    uint32_t SanctionCount = EOS_Sanctions_GetPlayerSanctionCount(EOSSanctions, &CountOpts);
                    for (uint32_t i = 0; i < SanctionCount; i++)
                    {
                        bool bStop = false;
                        EOS_Sanctions_PlayerSanction *Sanction = nullptr;
                        EOS_Sanctions_CopyPlayerSanctionByIndexOptions CopyOpts = {};
                        CopyOpts.ApiVersion = EOS_SANCTIONS_COPYPLAYERSANCTIONBYINDEX_API_LATEST;
                        CopyOpts.SanctionIndex = i;
                        CopyOpts.TargetUserId = Context->GetUserId()->GetProductUserId();
                        EOS_EResult CopyResult =
                            EOS_Sanctions_CopyPlayerSanctionByIndex(EOSSanctions, &CopyOpts, &Sanction);
                        if (CopyResult == EOS_EResult::EOS_Success)
                        {
							if (FString(ANSI_TO_TCHAR(Sanction->Action)) == TEXT("BAN") ||
								FString(ANSI_TO_TCHAR(Sanction->Action)) == TEXT("RESTRICT_GAME_ACCESS"))
                            {
                                // User is banned.
                                UE_LOG(
                                    LogEOSNetworkAuth,
                                    Error,
                                    TEXT("Server authentication: %s: User is banned because they have an active BAN or RESTRICT_GAME_ACCESS"
                                         "sanction."),
                                    *Context->GetUserId()->ToString());
                                Context->Finish(EAuthPhaseFailureCode::Phase_SanctionCheck_AccountBanned);
                                bStop = true;
                                bIsDenied = true;
                            }

                            // Otherwise this is some other kind of sanction we don't handle yet.
                        }
                        else
                        {
                            UE_LOG(
                                LogEOSNetworkAuth,
                                Error,
                                TEXT("Server authentication: %s: Failed to copy sanction at index %u, got result code "
                                     "%s on server."),
                                *Context->GetUserId()->ToString(),
                                i,
                                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                            Context->Finish(EAuthPhaseFailureCode::Phase_SanctionCheck_FailedToCopySanctionResult);
                            bStop = true;
                            bIsDenied = true;
                        }
                        if (Sanction != nullptr)
                        {
                            EOS_Sanctions_PlayerSanction_Release(Sanction);
                        }
                        if (bStop)
                        {
                            // Must do this after EOS_Sanctions_PlayerSanction_Release and not use break directly.
                            break;
                        }
                    }
                    if (!bIsDenied)
                    {
                        UE_LOG(
                            LogEOSNetworkAuth,
                            Verbose,
                            TEXT("Server authentication: %s: Did not find any BAN sanctions for user."),
                            *Context->GetUserId()->ToString());
                        Context->Finish(EAuthPhaseFailureCode::Success);
                    }
                }
                else
                {
                    UE_LOG(
                        LogEOSNetworkAuth,
                        Error,
                        TEXT("Server authentication: %s: Failed to retrieve sanctions for user, got result code %s on "
                             "server."),
                        *Context->GetUserId()->ToString(),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    Context->Finish(EAuthPhaseFailureCode::Phase_SanctionCheck_FailedToRetrieveSanctions);
                }
            }
        });
}

#endif
