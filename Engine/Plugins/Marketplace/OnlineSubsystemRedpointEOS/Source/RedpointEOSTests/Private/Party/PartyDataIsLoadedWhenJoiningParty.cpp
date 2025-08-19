// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "./TestPartyManager.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty,
    "OnlineSubsystemEOS.OnlinePartyInterface.PartyDataIsLoadedWhenJoiningParty",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager);

class FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager>,
      public FSingleTestPartyManager
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager);
    virtual ~FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> HostPartySystemWk;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> ClientPartySystemWk;
    std::function<void()> OnDone;

    FDelegateHandle PartyDataReceivedHandle;
    FDelegateHandle PartyMemberDataReceivedHandle;
    FDelegateHandle ReceiveInviteHandle;
    FUTickerDelegateHandle InviteTimeoutHandle;

    void Start_CreatePartyOnHost();
    void Handle_CreatePartyOnHost(
        const FUniqueNetId &LocalUserId,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const ECreatePartyCompletionResult Result);

    void Start_SetPartyDataOnHost();
    void Handle_SetPartyDataOnHost(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FName &Namespace,
        const FOnlinePartyData &PartyData);

    void Start_SetPartyMemberDataOnHost();
    void Handle_SetPartyMemberDataOnHost(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId,
        const FName &Namespace,
        const FOnlinePartyData &PartyData);

    void Start_SendInviteToClient();
    void Handle_SendInviteToClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        const ESendPartyInvitationCompletionResult Result);

    void Start_RestoreInvitesOnClient();
    void Handle_RestoreInvitesOnClient(const FUniqueNetId &LocalUserId, const FOnlineError &Result);

    void Handle_JoinParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const EJoinPartyCompletionResult Result,
        const int32 NotApprovedReason);

    bool HasClientReceivedInvite() const
    {
        if (this->HasParty(*this->Client.UserId))
        {
            return true;
        }
        return false;
    }

    void Handle_ReceiveInviteOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &SenderId);
    bool Handle_InviteTimeout(float DeltaTime);

    void Start();

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager(
        class FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client = MoveTemp(InClient);
        this->OnDone = MoveTemp(InOnDone);

        this->HostPartySystemWk = this->Host.Subsystem.Pin()->GetPartyInterface();
        this->ClientPartySystemWk = this->Client.Subsystem.Pin()->GetPartyInterface();
    }
};

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Start_CreatePartyOnHost()
{
    FOnlinePartyTypeId PartyTypeId = FOnlinePartyTypeId(1);

    FPartyConfiguration HostConfig;
    HostConfig.MaxMembers = 4;
    HostConfig.InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.PresencePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.bIsAcceptingMembers = true;
    HostConfig.bChatEnabled = false;

    UE_LOG(LogEOSTests, Log, TEXT("Creating party 1"));
    if (!T->TestTrue(
            TEXT("CreateParty operation started"),
            this->HostPartySystemWk.Pin()->CreateParty(
                *this->Host.UserId,
                PartyTypeId,
                HostConfig,
                FOnCreatePartyComplete::CreateSP(this, &TThisClass::Handle_CreatePartyOnHost))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Handle_CreatePartyOnHost(
    const FUniqueNetId &LocalUserId,
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const ECreatePartyCompletionResult Result)
{
    if (!T->TestEqual(TEXT("Party was successfully created"), Result, ECreatePartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->AddParty(this->Host, *PartyId);

    this->Start_SetPartyDataOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Start_SetPartyDataOnHost()
{
    if (auto HostPartySystem = this->HostPartySystemWk.Pin())
    {
        check(!this->PartyDataReceivedHandle.IsValid());
        this->PartyDataReceivedHandle = HostPartySystem->AddOnPartyDataReceivedDelegate_Handle(
            FOnPartyDataReceivedDelegate::CreateSP(this, &TThisClass::Handle_SetPartyDataOnHost));

        auto Data = HostPartySystem->GetPartyData(*this->Host.UserId, *this->Party(*this->Host.UserId), NAME_Default);
        auto NewData = MakeShared<FOnlinePartyData>(*Data);
        NewData->SetAttribute(TEXT("MyAttribute"), FVariantData(TEXT("MyData")));
        NewData->SetAttribute(TEXT("MyAttributeInt"), FVariantData((int64)10));
        NewData->SetAttribute(TEXT("MyAttributeDouble"), FVariantData((double)10.0));
        NewData->SetAttribute(TEXT("MyAttributeBool"), FVariantData((bool)true));

        if (!T->TestTrue(
                TEXT("Party set data was successful"),
                HostPartySystem
                    ->UpdatePartyData(*this->Host.UserId, *this->Party(*this->Host.UserId), NAME_Default, *NewData)))
        {
            this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
            return;
        }
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Handle_SetPartyDataOnHost(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FName &Namespace,
    const FOnlinePartyData &PartyData)
{
    if (auto HostPartySystem = this->HostPartySystemWk.Pin())
    {
        HostPartySystem->ClearOnPartyDataReceivedDelegate_Handle(this->PartyDataReceivedHandle);
        this->PartyDataReceivedHandle.Reset();

        this->Start_SetPartyMemberDataOnHost();
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::
    Start_SetPartyMemberDataOnHost()
{
    if (auto HostPartySystem = this->HostPartySystemWk.Pin())
    {
        check(!this->PartyMemberDataReceivedHandle.IsValid());
        this->PartyMemberDataReceivedHandle = HostPartySystem->AddOnPartyMemberDataReceivedDelegate_Handle(
            FOnPartyMemberDataReceivedDelegate::CreateSP(this, &TThisClass::Handle_SetPartyMemberDataOnHost));

        auto Data = HostPartySystem->GetPartyMemberData(
            *this->Host.UserId,
            *this->Party(*this->Host.UserId),
            *this->Host.UserId,
            NAME_Default);
        auto NewData = MakeShared<FOnlinePartyData>(*Data);
        NewData->SetAttribute(TEXT("MyMemberAttribute"), FVariantData(TEXT("MyData")));
        NewData->SetAttribute(TEXT("MyMemberAttributeInt"), FVariantData((int64)10));
        NewData->SetAttribute(TEXT("MyMemberAttributeDouble"), FVariantData((double)10.0));
        NewData->SetAttribute(TEXT("MyMemberAttributeBool"), FVariantData((bool)true));

        if (!T->TestTrue(
                TEXT("Party member set data was successful"),
                HostPartySystem->UpdatePartyMemberData(
                    *this->Host.UserId,
                    *this->Party(*this->Host.UserId),
                    NAME_Default,
                    *NewData)))
        {
            this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
            return;
        }
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::
    Handle_SetPartyMemberDataOnHost(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId,
        const FName &Namespace,
        const FOnlinePartyData &PartyData)
{
    if (auto HostPartySystem = this->HostPartySystemWk.Pin())
    {
        HostPartySystem->ClearOnPartyMemberDataReceivedDelegate_Handle(this->PartyMemberDataReceivedHandle);
        this->PartyMemberDataReceivedHandle.Reset();

        this->Start_SendInviteToClient();
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Start_SendInviteToClient()
{
    UE_LOG(LogEOSTests, Log, TEXT("Sending invitation"));
    if (!T->TestTrue(
            TEXT("SendInvitation operation started"),
            this->HostPartySystemWk.Pin()->SendInvitation(
                *this->Host.UserId,
                *this->Party(*this->Host.UserId),
                FPartyInvitationRecipient(this->Client.UserId.ToSharedRef()),
                FOnSendPartyInvitationComplete::CreateSP(this, &TThisClass::Handle_SendInviteToClient))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Handle_SendInviteToClient(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &RecipientId,
    const ESendPartyInvitationCompletionResult Result)
{
    T->TestTrue("Local user is sender", LocalUserId == *this->Host.UserId);
    T->TestTrue("Recipient matches", RecipientId == *Client.UserId);

    if (!T->TestEqual(
            FString::Printf(TEXT("Invitation for %s was successfully sent"), *PartyId.ToString()),
            Result,
            ESendPartyInvitationCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->Start_RestoreInvitesOnClient();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Start_RestoreInvitesOnClient()
{
    this->ClientPartySystemWk.Pin()->RestoreInvites(
        *this->Client.UserId,
        FOnRestoreInvitesComplete::CreateSP(this, &TThisClass::Handle_RestoreInvitesOnClient));
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Handle_RestoreInvitesOnClient(
    const FUniqueNetId &LocalUserId,
    const FOnlineError &Result)
{
    if (!T->TestTrue(TEXT("Restoring parties for client was successful"), Result.bSucceeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!this->HasClientReceivedInvite())
    {
        this->InviteTimeoutHandle = FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(this, &TThisClass::Handle_InviteTimeout),
            30.0f);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Handle_ReceiveInviteOnClient(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &SenderId)
{
    if (!this->HasParty(*this->Host.UserId))
    {
        return;
    }
    if (PartyId != *this->Party(*this->Host.UserId) || this->HasParty(*this->Client.UserId))
    {
        return;
    }

    this->AddParty(this->Client, PartyId);

    if (this->InviteTimeoutHandle.IsValid())
    {
        FUTicker::GetCoreTicker().RemoveTicker(this->InviteTimeoutHandle);
        this->InviteTimeoutHandle.Reset();
    }

    TArray<IOnlinePartyJoinInfoConstRef> Invites;
    if (!T->TestTrue(
            "Can retrieve pending invites",
            this->ClientPartySystemWk.Pin()->GetPendingInvites(*this->Client.UserId, Invites)))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    // We may have more than one invite from previous tests. Reject any we don't care about
    // and use the one that we do.
    IOnlinePartyJoinInfoConstPtr DesiredInvite;
    for (const auto &JoinInfo : Invites)
    {
        if (*JoinInfo->GetPartyId() != *this->Party(*this->Client.UserId))
        {
            if (!T->TestTrue(
                    "Can reject unwanted invitation",
                    this->ClientPartySystemWk.Pin()->RejectInvitation(
                        *this->Client.UserId,
                        *JoinInfo->GetSourceUserId())))
            {
                this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
                return;
            }
        }
        else
        {
            DesiredInvite = JoinInfo;
        }
    }

    if (!T->TestTrue("Got desired invite", DesiredInvite.IsValid()))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!T->TestTrue(
            "Can join party",
            this->ClientPartySystemWk.Pin()->JoinParty(
                *this->Client.UserId,
                *DesiredInvite,
                FOnJoinPartyComplete::CreateSP(this, &TThisClass::Handle_JoinParty))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Handle_JoinParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const EJoinPartyCompletionResult Result,
    const int32 NotApprovedReason)
{
    if (!T->TestEqual("Did join party", Result, EJoinPartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (auto ClientPartySystem = this->ClientPartySystemWk.Pin())
    {
        auto VerifyAttributes = [T = this->T](const FOnlinePartyDataConstPtr &Data, const FString &AttributePrefix) {
            {
                FString AttributeKey = FString::Printf(TEXT("%s"), *AttributePrefix);
                FVariantData Attr;
                if (T->TestTrue(
                        FString::Printf(TEXT("Attribute %s is present"), *AttributeKey),
                        Data->GetAttribute(AttributeKey, Attr)))
                {
                    if (T->TestEqual(
                            FString::Printf(TEXT("Attribute %s is of expected type"), *AttributeKey),
                            Attr.GetType(),
                            EOnlineKeyValuePairDataType::String))
                    {
                        FString AttrValue;
                        Attr.GetValue(AttrValue);
                        T->TestEqual(
                            FString::Printf(TEXT("Attribute %s value is correct"), *AttributeKey),
                            AttrValue,
                            TEXT("MyData"));
                    }
                }
            }

            {
                FString AttributeKey = FString::Printf(TEXT("%sInt"), *AttributePrefix);
                FVariantData Attr;
                if (T->TestTrue(
                        FString::Printf(TEXT("Attribute %s is present"), *AttributeKey),
                        Data->GetAttribute(AttributeKey, Attr)))
                {
                    if (T->TestEqual(
                            FString::Printf(TEXT("Attribute %s is of expected type"), *AttributeKey),
                            Attr.GetType(),
                            EOnlineKeyValuePairDataType::Int64))
                    {
                        int64 AttrValue;
                        Attr.GetValue(AttrValue);
                        T->TestEqual(
                            FString::Printf(TEXT("Attribute %s value is correct"), *AttributeKey),
                            AttrValue,
                            10);
                    }
                }
            }

            {
                FString AttributeKey = FString::Printf(TEXT("%sDouble"), *AttributePrefix);
                FVariantData Attr;
                if (T->TestTrue(
                        FString::Printf(TEXT("Attribute %s is present"), *AttributeKey),
                        Data->GetAttribute(AttributeKey, Attr)))
                {
                    if (T->TestEqual(
                            FString::Printf(TEXT("Attribute %s is of expected type"), *AttributeKey),
                            Attr.GetType(),
                            EOnlineKeyValuePairDataType::Double))
                    {
                        double AttrValue;
                        Attr.GetValue(AttrValue);
                        T->TestEqual(
                            FString::Printf(TEXT("Attribute %s value is correct"), *AttributeKey),
                            AttrValue,
                            10.0);
                    }
                }
            }

            {
                FString AttributeKey = FString::Printf(TEXT("%sBool"), *AttributePrefix);
                FVariantData Attr;
                if (T->TestTrue(
                        FString::Printf(TEXT("Attribute %s is present"), *AttributeKey),
                        Data->GetAttribute(AttributeKey, Attr)))
                {
                    if (T->TestEqual(
                            FString::Printf(TEXT("Attribute %s is of expected type"), *AttributeKey),
                            Attr.GetType(),
                            EOnlineKeyValuePairDataType::Bool))
                    {
                        bool AttrValue;
                        Attr.GetValue(AttrValue);
                        T->TestEqual(
                            FString::Printf(TEXT("Attribute %s value is correct"), *AttributeKey),
                            AttrValue,
                            true);
                    }
                }
            }
        };

        auto Data =
            ClientPartySystem->GetPartyData(*this->Client.UserId, *this->Party(*this->Client.UserId), NAME_Default);
        VerifyAttributes(Data, TEXT("MyAttribute"));

        auto MemberCount =
            ClientPartySystem->GetPartyMemberCount(*this->Client.UserId, *this->Party(*this->Client.UserId));
        if (T->TestEqual(TEXT("Party should have two members (host and client)"), MemberCount, 2))
        {
            auto MemberData = ClientPartySystem->GetPartyMemberData(
                *this->Client.UserId,
                *this->Party(*this->Client.UserId),
                *this->Host.UserId,
                NAME_Default);
            VerifyAttributes(MemberData, TEXT("MyMemberAttribute"));
        }

        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

bool FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Handle_InviteTimeout(
    float DeltaTime)
{
    T->TestTrue("Invitation timed out", false);
    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    return false;
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager::Start()
{
    this->ClientPartySystemWk.Pin()->AddOnPartyInviteReceivedDelegate_Handle(
        FOnPartyInviteReceivedDelegate::CreateSP(this, &TThisClass::Handle_ReceiveInviteOnClient));

    this->Start_CreatePartyOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Instance =
                MakeShared<FOnlineSubsystemEOS_OnlinePartyInterface_PartyDataIsLoadedWhenJoiningParty_Manager>(
                    this,
                    Instances[0],
                    Instances[1],
                    OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}

#endif // #if EOS_HAS_AUTHENTICATION
