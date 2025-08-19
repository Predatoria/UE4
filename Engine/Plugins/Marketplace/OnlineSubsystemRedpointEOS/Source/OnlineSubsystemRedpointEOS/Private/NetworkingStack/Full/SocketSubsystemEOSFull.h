// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSocketSubsystemEOSFull;

#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/Full/SocketEOSFull.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRegulatedTicker.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "SocketSubsystem.h"
#include "Tickable.h"

EOS_ENABLE_STRICT_WARNINGS

class FSocketSubsystemEOSFull : public ISocketSubsystemEOS, public TSharedFromThis<FSocketSubsystemEOSFull>
{
    friend class FSocketEOSFull;
    friend class UEOSNetDriver;
    friend class FEOSSocketRoleNone;
    friend class FEOSSocketRoleClient;
    friend class FEOSSocketRoleListening;
    friend class FEOSSocketRoleRemote;
    friend class FGameplayDebuggerCategory_P2PConnections;

private:
    /** Reference to the EOS P2P interface. */
    EOS_HP2P EOSP2P;

    /** If true, the subsystem will not receive packets. */
    bool bReceiveSuspended;

    /**
     * Reference to the online subsystem. This is valid until Shutdown is called; we must clear it so that the OSS
     * IsUnique checks pass during shutdown.
     */
    TSharedPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> Subsystem;
    TSharedRef<FEOSConfig> Config;
    TSharedRef<FEOSRegulatedTicker> RegulatedTicker;
    FDelegateHandle ReceivePacketsDelegateHandle;

    /** A map that tracks all sockets allocated in memory. */
    TMap<FSocketEOSMemoryId, TSharedPtr<FSocketEOSFull>> HeldSockets;
    FSocketEOSMemoryId NextSocketId;

    /**
     * A map that tracks sockets for the purposes of routing received packets. A socket is only added to this map once
     * either Bind or SentTo has been called.
     */
    TSocketEOSMap<TWeakPtr<FSocketEOSFull>> AssignedSockets;

    // Pre-allocated role instances.
    TSharedRef<class FEOSSocketRoleNone> RoleInstance_None;
    TSharedRef<class FEOSSocketRoleClient> RoleInstance_Client;
    TSharedRef<class FEOSSocketRoleListening> RoleInstance_Listening;
    TSharedRef<class FEOSSocketRoleRemote> RoleInstance_Remote;

    // The listen manager, which holds references to listening sockets and
    // routes the IncomingConnection and RemoteConnectionClosed events correctly.
    TSharedRef<class FSocketSubsystemEOSListenManager> ListenManager;

    // The next reset ID for an outbound client connection.
    uint32_t NextResetId;

    // Returns the next reset ID and increments it.
    uint32_t GetResetIdForOutboundConnection()
    {
        return this->NextResetId++;
    }

    // Tracks pending reset IDs; that is, when we receive a reset ID for a socket that's not yet open, we don't yet have
    // the socket to check / assign it to. This holds a list of reset IDs we have received from remote hosts so that
    // when ::Accept actually happens we can load it in.
    TMap<FString, uint32_t> PendingResetIds;

#if EOS_VERSION_AT_LEAST(1, 11, 0)
    // The event handler for when the incoming packet queue is full.
    TSharedPtr<EOSEventHandle<EOS_P2P_OnIncomingPacketQueueFullInfo>> IncomingPacketQueueFull;
#endif

public:
    FSocketSubsystemEOSFull(
        const TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe> &InSubsystem,
        EOS_HPlatform InPlatform,
        TSharedRef<FEOSConfig> InConfig,
        TSharedRef<FEOSRegulatedTicker> InRegulatedTicker);
    UE_NONCOPYABLE(FSocketSubsystemEOSFull);
    ~FSocketSubsystemEOSFull();

    /**
     * Returns the product user ID to use when binding P2P addresses.
     */
    virtual EOS_ProductUserId GetBindingProductUserId_P2POnly() const override;

    /**
     * Returns the product user ID for either P2P or dedicated server connections. The product user ID
     * is potentially nullptr (for dedicated servers), so the boolean result is whether the result
     * is valid.
     */
    virtual bool GetBindingProductUserId_P2POrDedicatedServer(EOS_ProductUserId &OutPUID) const override;

    virtual FString GetSocketName(bool bListening, FURL InURL) const override;

    virtual bool Init(FString &Error) override;
    virtual void Shutdown() override;

    bool Tick_ReceiveP2PPackets(float DeltaSeconds);

    virtual FSocket *CreateSocket(const FName &SocketType, const FString &SocketDescription, const FName &ProtocolName)
        override;
    virtual void DestroySocket(FSocket *Socket) override;

    virtual FAddressInfoResult GetAddressInfo(
        const TCHAR *HostName,
        const TCHAR *ServiceName = nullptr,
        EAddressInfoFlags QueryFlags = EAddressInfoFlags::Default,
        const FName ProtocolTypeName = NAME_None,
        ESocketType SocketType = ESocketType::SOCKTYPE_Unknown) override;
    virtual TSharedPtr<FInternetAddr> GetAddressFromString(const FString &InAddress) override;

    virtual bool RequiresChatDataBeSeparate() override;
    virtual bool RequiresEncryptedPackets() override;
    virtual bool GetHostName(FString &HostName) override;

    virtual TSharedRef<FInternetAddr> CreateInternetAddr() override;
    virtual bool HasNetworkDevice() override;

    virtual const TCHAR *GetSocketAPIName() const override;
    virtual ESocketErrors GetLastErrorCode() override;
    virtual ESocketErrors TranslateErrorCode(int32 Code) override;

    virtual bool IsSocketWaitSupported() const override;

    virtual void SuspendReceive() override;
    virtual void ResumeReceive() override;
};

EOS_DISABLE_STRICT_WARNINGS