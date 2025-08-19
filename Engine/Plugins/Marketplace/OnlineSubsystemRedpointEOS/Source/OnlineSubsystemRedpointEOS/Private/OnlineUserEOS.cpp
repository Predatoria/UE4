// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"

#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/WorldResolution.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemTypes.h"

EOS_ENABLE_STRICT_WARNINGS

FEOSUserData::FEOSUserData(
    const TArray<EOS_Connect_ExternalAccountInfo *> &InExternalInfo,
    const TArray<TSharedRef<const FUniqueNetId>> &InExternalIds,
    EOS_EExternalAccountType InDefaultPlatformType,
    EOS_UserInfo *InEpicAccountInfo)
    : ExternalInfo()
    , ExternalIds(InExternalIds)
    , DefaultPlatformType(InDefaultPlatformType)
    , EpicAccountInfo(InEpicAccountInfo)
{
    for (auto Info : InExternalInfo)
    {
        if (Info != nullptr)
        {
            this->ExternalInfo.Add(Info->AccountIdType, Info);
        }
    }
}

FEOSUserData::~FEOSUserData()
{
    for (auto KV : this->ExternalInfo)
    {
        EOS_Connect_ExternalAccountInfo_Release(KV.Value);
    }
    this->ExternalIds.Empty();
    this->ExternalInfo.Empty();
    if (this->EpicAccountInfo != nullptr)
    {
        EOS_UserInfo_Release(this->EpicAccountInfo);
        this->EpicAccountInfo = nullptr;
    }
}

void IEOSUser::SetData(const TSharedRef<const FEOSUserData> &InData)
{
    checkf(!this->Data.IsValid(), TEXT("This user object already has data associated with it!"));
    this->Data = InData;
}

const FEOSUserData &IEOSUser::GetData() const
{
    checkf(this->IsReady(), TEXT("This user object is not ready, so calling GetData is invalid!"));
    return *this->Data;
}

bool IEOSUser::IsReady() const
{
    return this->Data.IsValid();
}

TArray<TSharedRef<const FUniqueNetId>> IEOSUser::GetExternalUserIds() const
{
    if (this->Data.IsValid())
    {
        return this->Data->ExternalIds;
    }

    return TArray<TSharedRef<const FUniqueNetId>>();
}

EOS_EExternalAccountType StrToExternalAccountType(const FString &InType)
{
    if (InType.ToUpper() == TEXT("EPIC"))
    {
        return EOS_EExternalAccountType::EOS_EAT_EPIC;
    }
    else if (InType.ToUpper() == TEXT("STEAM"))
    {
        return EOS_EExternalAccountType::EOS_EAT_STEAM;
    }
    else if (InType.ToUpper() == TEXT("PSN"))
    {
        return EOS_EExternalAccountType::EOS_EAT_PSN;
    }
    else if (InType.ToUpper() == TEXT("XBL"))
    {
        return EOS_EExternalAccountType::EOS_EAT_XBL;
    }
    else if (InType.ToUpper() == TEXT("DISCORD"))
    {
        return EOS_EExternalAccountType::EOS_EAT_DISCORD;
    }
    else if (InType.ToUpper() == TEXT("GOG"))
    {
        return EOS_EExternalAccountType::EOS_EAT_GOG;
    }
    else if (InType.ToUpper() == TEXT("NINTENDO"))
    {
        return EOS_EExternalAccountType::EOS_EAT_NINTENDO;
    }
    else if (InType.ToUpper() == TEXT("UPLAY"))
    {
        return EOS_EExternalAccountType::EOS_EAT_UPLAY;
    }
    else if (InType.ToUpper() == TEXT("OPENID"))
    {
        return EOS_EExternalAccountType::EOS_EAT_OPENID;
    }
    else if (InType.ToUpper() == TEXT("APPLE"))
    {
        return EOS_EExternalAccountType::EOS_EAT_APPLE;
    }
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    else if (InType.ToUpper() == TEXT("GOOGLE"))
    {
        return EOS_EExternalAccountType::EOS_EAT_GOOGLE;
    }
#endif
#if EOS_VERSION_AT_LEAST(1, 10, 3)
    else if (InType.ToUpper() == TEXT("OCULUS"))
    {
        return EOS_EExternalAccountType::EOS_EAT_OCULUS;
    }
#endif
    return (EOS_EExternalAccountType)-1;
}

FString ExternalAccountTypeToStr(EOS_EExternalAccountType InType)
{
    switch (InType)
    {
    case EOS_EExternalAccountType::EOS_EAT_EPIC:
        return TEXT("EPIC");
    case EOS_EExternalAccountType::EOS_EAT_STEAM:
        return TEXT("STEAM");
    case EOS_EExternalAccountType::EOS_EAT_PSN:
        return TEXT("PSN");
    case EOS_EExternalAccountType::EOS_EAT_XBL:
        return TEXT("XBL");
    case EOS_EExternalAccountType::EOS_EAT_DISCORD:
        return TEXT("DISCORD");
    case EOS_EExternalAccountType::EOS_EAT_GOG:
        return TEXT("GOG");
    case EOS_EExternalAccountType::EOS_EAT_NINTENDO:
        return TEXT("NINTENDO");
    case EOS_EExternalAccountType::EOS_EAT_UPLAY:
        return TEXT("UPLAY");
    case EOS_EExternalAccountType::EOS_EAT_OPENID:
        return TEXT("OPENID");
    case EOS_EExternalAccountType::EOS_EAT_APPLE:
        return TEXT("APPLE");
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EExternalAccountType::EOS_EAT_GOOGLE:
        return TEXT("GOOGLE");
#endif
#if EOS_VERSION_AT_LEAST(1, 10, 3)
    case EOS_EExternalAccountType::EOS_EAT_OCULUS:
        return TEXT("OCULUS");
#endif
    }
    return TEXT("");
}

class FEOSUserFactoryOperation : public TSharedFromThis<FEOSUserFactoryOperation>
{
private:
    FName InstanceName;
    EOS_HConnect EOSConnect;
    TSharedPtr<IEOSRuntimePlatform> RuntimePlatform;
    TSharedPtr<const FUniqueNetIdEOS> QueryingUserId;
    /** Hold a reference to ourselves while we're doing our async operation. */
    TSharedPtr<class FEOSUserFactoryOperation> SelfReference;
    int NextAsyncOpId;
    TSet<int> AsyncOperationsInProgress;
    int AddAsyncOp();
    void MarkAsyncOpAsDone(int OpId);
    void EndMappingsResolve(int AsyncOp);

public:
    FEOSUserFactoryOperation(
        FName InInstanceName,
        EOS_HPlatform InPlatform,
        const TSharedRef<IEOSRuntimePlatform> &InRuntimePlatform,
        const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId);
    UE_NONCOPYABLE(FEOSUserFactoryOperation);
    TUserIdMap<TSharedPtr<IEOSUser>> Users;
    std::function<void(const TUserIdMap<TSharedPtr<IEOSUser>> &)> OnResolved;
    void BeginResolve();
};

FEOSUserFactoryOperation::FEOSUserFactoryOperation(
    FName InInstanceName,
    EOS_HPlatform InPlatform,
    const TSharedRef<IEOSRuntimePlatform> &InRuntimePlatform,
    const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId)
    : InstanceName(InInstanceName)
    , EOSConnect(EOS_Platform_GetConnectInterface(InPlatform))
    , RuntimePlatform(InRuntimePlatform)
    , QueryingUserId(InQueryingUserId)
    , SelfReference()
    , NextAsyncOpId(1000)
    , AsyncOperationsInProgress()
    , Users()
    , OnResolved()
{
}

int FEOSUserFactoryOperation::AddAsyncOp()
{
    int ThisId = this->NextAsyncOpId++;
    this->AsyncOperationsInProgress.Add(ThisId);
    return ThisId;
}

void FEOSUserFactoryOperation::MarkAsyncOpAsDone(int OpId)
{
    this->AsyncOperationsInProgress.Remove(OpId);
    if (this->AsyncOperationsInProgress.Num() == 0)
    {
        // We are done, fire OnResolved and remove SelfReference.
        this->OnResolved(this->Users);
        this->SelfReference.Reset();

        // warning: At this point, our memory is freed.
    }
}

void FEOSUserFactoryOperation::EndMappingsResolve(int AsyncOp)
{
    TSoftObjectPtr<UWorld> World = FWorldResolution::GetWorld(this->InstanceName, true);

    // Now iterate through all of the users and attempt to load in their resolved data.
    for (const auto &KV : this->Users)
    {
        EOS_ProductUserId PUID = KV.Value->GetUserIdEOS()->GetProductUserId();

        EOS_Connect_GetProductUserExternalAccountCountOptions CountOpts = {};
        CountOpts.ApiVersion = EOS_CONNECT_GETPRODUCTUSEREXTERNALACCOUNTCOUNT_API_LATEST;
        CountOpts.TargetUserId = PUID;
        int Count = EOS_Connect_GetProductUserExternalAccountCount(this->EOSConnect, &CountOpts);

        TArray<EOS_Connect_ExternalAccountInfo *> ExternalInfoToAttach;
        TArray<TSharedRef<const FUniqueNetId>> ExternalIdsToAttach;

        bool bRequireEpicAccountFetch = false;
        FString EpicAccountId = TEXT("");
        for (int i = 0; i < Count; i++)
        {
            EOS_Connect_CopyProductUserExternalAccountByIndexOptions CopyOpts = {};
            CopyOpts.ApiVersion = EOS_CONNECT_COPYPRODUCTUSEREXTERNALACCOUNTBYINDEX_API_LATEST;
            CopyOpts.ExternalAccountInfoIndex = i;
            CopyOpts.TargetUserId = PUID;

            EOS_Connect_ExternalAccountInfo *ExternalAccountInfo = nullptr;
            EOS_EResult CopyResult =
                EOS_Connect_CopyProductUserExternalAccountByIndex(this->EOSConnect, &CopyOpts, &ExternalAccountInfo);
            if (CopyResult == EOS_EResult::EOS_Success)
            {
                ExternalInfoToAttach.Add(ExternalAccountInfo);

                if (ExternalAccountInfo->AccountIdType == EOS_EExternalAccountType::EOS_EAT_EPIC)
                {
                    bRequireEpicAccountFetch = true;
                    EpicAccountId = EOSString_Connect_ExternalAccountId::FromAnsiString(ExternalAccountInfo->AccountId);
                }

                for (const auto &Integration : this->RuntimePlatform->GetIntegrations())
                {
                    TSharedPtr<const FUniqueNetId> ExternalId = Integration->GetUserId(World, ExternalAccountInfo);
                    if (ExternalId.IsValid())
                    {
                        ExternalIdsToAttach.Add(ExternalId.ToSharedRef());
                    }
                }

                // No need to release on success, as FEOSUserData will take ownership.
            }
            else if (ExternalAccountInfo != nullptr)
            {
                EOS_Connect_ExternalAccountInfo_Release(ExternalAccountInfo);
            }
        }

        // Try to figure out what the default platform should be when we just want to get the display name, etc. Use
        // EOS_Connect_CopyProductUserInfo and discover the type from the copied data, then release it.
        EOS_EExternalAccountType DefaultPlatformType = (EOS_EExternalAccountType)-1;
        {
            EOS_Connect_CopyProductUserInfoOptions CopyOpts = {};
            CopyOpts.ApiVersion = EOS_CONNECT_COPYPRODUCTUSERINFO_API_LATEST;
            CopyOpts.TargetUserId = PUID;
            EOS_Connect_ExternalAccountInfo *ExternalAccountInfo = nullptr;
            EOS_EResult CopyResult = EOS_Connect_CopyProductUserInfo(this->EOSConnect, &CopyOpts, &ExternalAccountInfo);
            if (CopyResult == EOS_EResult::EOS_Success)
            {
                DefaultPlatformType = ExternalAccountInfo->AccountIdType;
            }
            if (ExternalAccountInfo != nullptr)
            {
                EOS_Connect_ExternalAccountInfo_Release(ExternalAccountInfo);
            }
        }
        if (ExternalInfoToAttach.Num() > 0 &&
            (DefaultPlatformType == (EOS_EExternalAccountType)-1 ||
             ExternalInfoToAttach.FindByPredicate([DefaultPlatformType](EOS_Connect_ExternalAccountInfo *Val) {
                 return Val->AccountIdType == DefaultPlatformType;
             }) == nullptr))
        {
            // Pick the first external account info if we can't determine the default or if we don't actually have
            // information for the default platform (which would be weird, but we're guarding anyway).
            DefaultPlatformType = ExternalInfoToAttach[0]->AccountIdType;
        }

        if (bRequireEpicAccountFetch)
        {
            // @todo: If the external account info is an Epic account, we also need to try to make a request to
            // EOSUserInfo if the querying user has an Epic account, and then incorporate that data before generating
            // FEOSUserData.
        }

        TSharedRef<FEOSUserData> UserData =
            MakeShared<FEOSUserData>(ExternalInfoToAttach, ExternalIdsToAttach, DefaultPlatformType);
        KV.Value->SetData(UserData);
    }

    this->MarkAsyncOpAsDone(AsyncOp);
}

void FEOSUserFactoryOperation::BeginResolve()
{
    // Hold a reference while we do our asynchronous work.
    this->SelfReference = this->AsShared();

    // Kick off the batched operation to look up all external accounts associated with all of the users in this
    // operation.
    TArray<TSharedPtr<IEOSUser>> UsersArray;
    this->Users.GenerateValueArray(UsersArray);
    int QueryProductUserIdMappingsOp = this->AddAsyncOp();
    FMultiOperation<TSharedPtr<IEOSUser>, bool>::RunBatched(
        UsersArray,
        EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_MAX_ACCOUNT_IDS,
        [WeakThis = GetWeakThis(this)](
            TArray<TSharedPtr<IEOSUser>> &Batch,
            const std::function<void(TArray<bool> OutResults)> &OnDone) -> bool {
            if (auto This = PinWeakThis(WeakThis))
            {
                EOS_Connect_QueryProductUserIdMappingsOptions QueryOpts = {};
                QueryOpts.ApiVersion = EOS_CONNECT_QUERYPRODUCTUSERIDMAPPINGS_API_LATEST;
                QueryOpts.LocalUserId = This->QueryingUserId->GetProductUserId();
                EOSString_ProductUserId::AllocateToIdListViaAccessor<TSharedPtr<IEOSUser>>(
                    Batch,
                    [](const TSharedPtr<IEOSUser> &User) -> EOS_ProductUserId {
                        return User->GetUserIdEOS()->GetProductUserId();
                    },
                    QueryOpts.ProductUserIdCount,
                    QueryOpts.ProductUserIds);
                EOSRunOperation<
                    EOS_HConnect,
                    EOS_Connect_QueryProductUserIdMappingsOptions,
                    EOS_Connect_QueryProductUserIdMappingsCallbackInfo>(
                    This->EOSConnect,
                    &QueryOpts,
                    EOS_Connect_QueryProductUserIdMappings,
                    [WeakThis = GetWeakThis(This), QueryOpts, Batch, OnDone](
                        const EOS_Connect_QueryProductUserIdMappingsCallbackInfo *Data) {
                        EOSString_ProductUserId::FreeFromIdListConst(
                            QueryOpts.ProductUserIdCount,
                            QueryOpts.ProductUserIds);
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            UE_LOG(
                                LogEOS,
                                Verbose,
                                TEXT("EOS_Connect_QueryProductUserIdMappings returned result code: %s"),
                                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));

                            // We don't do any processing of the result here (and we don't really care
                            // about the result beyond logging). We'll process all of the mappings once
                            // we've finished all of our batching work.
                            TArray<bool> Results;
                            for (int i = 0; i < Batch.Num(); i++)
                            {
                                Results.Add(true);
                            }
                            OnDone(Results);
                        }
                    });
                return true;
            }

            return false;
        },
        [WeakThis = GetWeakThis(this), QueryProductUserIdMappingsOp](const TArray<bool> &OutResults) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->EndMappingsResolve(QueryProductUserIdMappingsOp);
            }
        });
}

TUserIdMap<TSharedPtr<IEOSUser>> FEOSUserFactory::GetInternal(
    const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId,
    const TArray<TSharedRef<const FUniqueNetIdEOS>> &InUserIds,
    TSharedRef<IEOSUser> (*ConstructUser)(const TSharedRef<const FUniqueNetIdEOS> &InUserId),
    std::function<void(const TUserIdMap<TSharedPtr<IEOSUser>> &)> OnResolved)
{
    TSharedRef<FEOSUserFactoryOperation> Operation = MakeShared<FEOSUserFactoryOperation>(
        this->InstanceName,
        this->Platform,
        this->RuntimePlatform,
        InQueryingUserId);
    for (const auto &UserId : InUserIds)
    {
        Operation->Users.Add(*UserId, ConstructUser(UserId));
    }
    Operation->OnResolved = MoveTemp(OnResolved);
    Operation->BeginResolve();
    return Operation->Users;
}

TUserIdMap<TSharedPtr<IEOSUser>> FEOSUserFactory::GetUnresolved(
    const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId,
    const TArray<TSharedRef<const FUniqueNetIdEOS>> &InUserIds,
    TSharedRef<IEOSUser> (*ConstructUser)(const TSharedRef<const FUniqueNetIdEOS> &InUserId))
{
    return GetInternal(InQueryingUserId, InUserIds, ConstructUser, [](const TUserIdMap<TSharedPtr<IEOSUser>> &) {
    });
}

void FEOSUserFactory::Get(
    const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId,
    const TArray<TSharedRef<const FUniqueNetIdEOS>> &InUserIds,
    TSharedRef<IEOSUser> (*ConstructUser)(const TSharedRef<const FUniqueNetIdEOS> &InUserId),
    std::function<void(const TUserIdMap<TSharedPtr<IEOSUser>> &)> OnResolved)
{
    GetInternal(InQueryingUserId, InUserIds, ConstructUser, MoveTemp(OnResolved));
}

FString FUserOnlineAccountEOS::GetAccessToken() const
{
    if (auto IdentityPinned = PinWeakThis(this->Identity))
    {
        EOS_Connect_IdToken *IdToken = nullptr;
        EOS_Connect_CopyIdTokenOptions CopyOpts = {};
        CopyOpts.ApiVersion = EOS_CONNECT_COPYIDTOKEN_API_LATEST;
        CopyOpts.LocalUserId = this->GetUserIdEOS()->GetProductUserId();
        EOS_EResult CopyResult = EOS_Connect_CopyIdToken(IdentityPinned->EOSConnect, &CopyOpts, &IdToken);
        if (CopyResult == EOS_EResult::EOS_Success)
        {
            FString Result = EOSString_AnsiUnlimited::FromAnsiString(IdToken->JsonWebToken);
            EOS_Connect_IdToken_Release(IdToken);
            return Result;
        }
        else
        {
            if (IdToken != nullptr)
            {
                EOS_Connect_IdToken_Release(IdToken);
            }

            UE_LOG(
                LogEOS,
                Warning,
                TEXT("Unable to obtain EOS Connect ID token for account: %s"),
                *ConvertError(
                     TEXT("FUserOnlineAccountEOS::GetAccessToken"),
                     TEXT("Attempt to obtain EOS Connect ID token for account failed"),
                     CopyResult)
                     .ToLogString());
            return TEXT("");
        }
    }
    else
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Returning an empty ID token from GetAccessToken, because the online identity interface is no longer "
                 "available."));
        return TEXT("");
    }
}

bool FUserOnlineAccountEOS::GetAuthAttribute(const FString &AttrName, FString &OutAttrValue) const
{
#if EOS_HAS_AUTHENTICATION
    if (AttrName == TEXT("crossPlatform.canLink") || AttrName == TEXT("epic.canLink"))
    {
        if (AttrName == TEXT("epic.canLink"))
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("The 'epic.canLink' attribute is deprecated, please use 'crossPlatform.canLink' instead."));
        }

        if (auto IdentityPtr = this->Identity.Pin())
        {
            if (IdentityPtr->GetCrossPlatformAccountId(*this->GetUserIdEOS()).IsValid())
            // NOLINTNEXTLINE(bugprone-branch-clone)
            {
                // We can't link, because we already have an account linked.
                OutAttrValue = TEXT("false");
            }
            else if (!IdentityPtr->IsCrossPlatformAccountProviderAvailable())
            {
                // We can't link, because the game doesn't have a cross-platform account provider set.
                OutAttrValue = TEXT("false");
            }
            else
            {
                // This account can be upgraded.
                OutAttrValue = TEXT("true");
            }
            return true;
        }
        else
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("The identity interface has been destroyed, so the 'crossPlatform.canLink' attribute can not be "
                     "read."));
            return false;
        }
    }

    if (AttrName == TEXT("idToken"))
    {
        // Forward to GetAccessToken, which always grabs the latest ID token.
        OutAttrValue = this->GetAccessToken();
        return !OutAttrValue.IsEmpty();
    }

    if (AttrName == TEXT("epic.idToken"))
    {
        // We have to handle this attribute as a special case, since it's lifetime
        // is not tied to the EOS refresh event. We always need to call the SDK to
        // get the current version, which means we can't pass this value up
        // through the authentication infrastructure.
        if (auto IdentityPtr = this->Identity.Pin())
        {
            TSharedPtr<const FCrossPlatformAccountId> CrossPlatformId =
                IdentityPtr->GetCrossPlatformAccountId(*this->GetUserIdEOS());
            if (CrossPlatformId.IsValid() && CrossPlatformId->GetType().IsEqual(EPIC_GAMES_ACCOUNT_ID))
            {
                TSharedPtr<const FEpicGamesCrossPlatformAccountId> EpicGamesId =
                    StaticCastSharedPtr<const FEpicGamesCrossPlatformAccountId>(CrossPlatformId);

                EOS_Auth_IdToken* IdToken = nullptr;
                EOS_Auth_CopyIdTokenOptions CopyOpts = {};
                CopyOpts.ApiVersion = EOS_AUTH_COPYIDTOKEN_API_LATEST;
                CopyOpts.AccountId = EpicGamesId->GetEpicAccountId();
                EOS_EResult CopyResult = EOS_Auth_CopyIdToken(IdentityPtr->EOSAuth, &CopyOpts, &IdToken);
                if (CopyResult == EOS_EResult::EOS_Success)
                {
                    OutAttrValue = EOSString_AnsiUnlimited::FromAnsiString(IdToken->JsonWebToken);
                    EOS_Auth_IdToken_Release(IdToken);
                    return true;
                }
                else
                {
                    if (IdToken != nullptr)
                    {
                        EOS_Auth_IdToken_Release(IdToken);
                    }

                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("Unable to obtain EOS Auth ID token for account: %s"),
                        *ConvertError(
                             TEXT("FUserOnlineAccountEOS::GetAuthAttribute"),
                             TEXT("Attempt to obtain EOS Auth ID token for account failed"),
                             CopyResult)
                             .ToLogString());
                }
            }
        }

        return false;
    }
#endif // #if EOS_HAS_AUTHENTICATION

    if (this->AuthAttributes.Contains(AttrName))
    {
        OutAttrValue = this->AuthAttributes[AttrName];
        return true;
    }

    return false;
}

bool FUserOnlineAccountEOS::SetUserAttribute(const FString &AttrName, const FString &AttrValue)
{
    return false;
}

FOnlineRecentPlayerEOS::FOnlineRecentPlayerEOS()
    : TBaseUserImplementation<FOnlineRecentPlayerEOS, FOnlineRecentPlayer>()
{
    this->LastSeen = FDateTime::MinValue();
};

FDateTime FOnlineRecentPlayerEOS::GetLastSeen() const
{
    return this->LastSeen;
}

EOS_DISABLE_STRICT_WARNINGS