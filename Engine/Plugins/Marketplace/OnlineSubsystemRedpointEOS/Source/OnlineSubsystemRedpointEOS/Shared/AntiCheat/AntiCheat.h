// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#if defined(UE_5_0_OR_LATER)
#include "Online/CoreOnline.h"
#else
#include "UObject/CoreOnline.h"
#endif

class FAntiCheatSession : public TSharedFromThis<FAntiCheatSession>
{
public:
    FAntiCheatSession() = default;
    virtual ~FAntiCheatSession(){};
    UE_NONCOPYABLE(FAntiCheatSession);
};

#if !EOS_VERSION_AT_LEAST(1, 12, 0)
enum class EOS_EAntiCheatCommonClientAction : uint8
{
    EOS_ACCCA_Invalid = 0,
    EOS_ACCCA_RemovePlayer = 1
};
enum class EOS_EAntiCheatCommonClientActionReason : uint8
{
    EOS_ACCCAR_Invalid = 0,
    EOS_ACCCAR_InternalError = 1,
    EOS_ACCCAR_InvalidMessage = 2,
    EOS_ACCCAR_AuthenticationFailed = 3,
    EOS_ACCCAR_NullClient = 4,
    EOS_ACCCAR_HeartbeatTimeout = 5,
    EOS_ACCCAR_ClientViolation = 6,
    EOS_ACCCAR_BackendViolation = 7,
    EOS_ACCCAR_TemporaryCooldown = 8,
    EOS_ACCCAR_TemporaryBanned = 9,
    EOS_ACCCAR_PermanentBanned = 10
};
enum class EOS_EAntiCheatCommonClientAuthStatus : uint8
{
    EOS_ACCCAS_Invalid = 0,
    EOS_ACCCAS_LocalAuthComplete = 1,
    EOS_ACCCAS_RemoteAuthComplete = 2
};
enum class EOS_EAntiCheatCommonClientType : uint8
{
    EOS_ACCCT_ProtectedClient = 0,
    EOS_ACCCT_UnprotectedClient = 1,
    EOS_ACCCT_AIBot = 2
};
enum class EOS_EAntiCheatCommonClientPlatform : uint8
{
    EOS_ACCCP_Unknown = 0,
    EOS_ACCCP_Windows = 1,
    EOS_ACCCP_Mac = 2,
    EOS_ACCCP_Linux = 3,
    EOS_ACCCP_Xbox = 4,
    EOS_ACCCP_PlayStation = 5,
    EOS_ACCCP_Nintendo = 6,
    EOS_ACCCP_iOS = 7,
    EOS_ACCCP_Android = 8
};
#endif

FString EOS_EAntiCheatCommonClientActionReason_ToString(const EOS_EAntiCheatCommonClientActionReason &Reason);

DECLARE_DELEGATE_FiveParams(
    FOnAntiCheatSendNetworkMessage,
    const TSharedRef<FAntiCheatSession> & /* Session */,
    const FUniqueNetIdEOS & /* SourceUserId */,
    const FUniqueNetIdEOS & /* TargetUserId */,
    const uint8 * /* Data */,
    uint32_t /* Size */);
DECLARE_MULTICAST_DELEGATE_TwoParams(
    FOnAntiCheatPlayerAuthStatusChanged,
    const FUniqueNetIdEOS & /* TargetUserId */,
    EOS_EAntiCheatCommonClientAuthStatus /* NewAuthStatus */);
DECLARE_MULTICAST_DELEGATE_FourParams(
    FOnAntiCheatPlayerActionRequired,
    const FUniqueNetIdEOS & /* TargetUserId */,
    EOS_EAntiCheatCommonClientAction /* ClientAction */,
    EOS_EAntiCheatCommonClientActionReason /* ActionReasonCode */,
    const FString & /* ActionReasonDetailsString */);

class IAntiCheat : public FExec
{
public:
    virtual ~IAntiCheat(){};

    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    virtual TSharedPtr<FAntiCheatSession> CreateSession(
        bool bIsServer,
        const FUniqueNetIdEOS &HostUserId,
        bool bIsDedicatedServerSession,
        TSharedPtr<const FUniqueNetIdEOS> ListenServerUserId,
        FString ServerConnectionUrlOnClient) = 0;
    virtual bool DestroySession(FAntiCheatSession &Session) = 0;

    virtual bool RegisterPlayer(
        FAntiCheatSession &Session,
        const FUniqueNetIdEOS &UserId,
        EOS_EAntiCheatCommonClientType ClientType,
        EOS_EAntiCheatCommonClientPlatform ClientPlatform) = 0;
    virtual bool UnregisterPlayer(FAntiCheatSession &Session, const FUniqueNetIdEOS &UserId) = 0;

    /** Called by the IAntiCheat implementation when a network message needs to be sent. */
    FOnAntiCheatSendNetworkMessage OnSendNetworkMessage;
    virtual bool ReceiveNetworkMessage(
        FAntiCheatSession &Session,
        const FUniqueNetIdEOS &SourceUserId,
        const FUniqueNetIdEOS &TargetUserId,
        const uint8 *Data,
        uint32_t Size) = 0;

    /** Called by the IAntiCheat implementation when a player's auth status has been verified. */
    FOnAntiCheatPlayerAuthStatusChanged OnPlayerAuthStatusChanged;

    /** Called by the IAntiCheat implementation when a player needs to be kicked. */
    FOnAntiCheatPlayerActionRequired OnPlayerActionRequired;
};