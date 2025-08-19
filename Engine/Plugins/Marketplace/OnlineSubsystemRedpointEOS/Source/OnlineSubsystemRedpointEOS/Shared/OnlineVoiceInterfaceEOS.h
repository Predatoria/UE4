// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/VoiceInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#if defined(EOS_VOICE_CHAT_SUPPORTED)
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManager.h"
#endif

EOS_ENABLE_STRICT_WARNINGS

#if defined(EOS_VOICE_CHAT_SUPPORTED)

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineVoiceInterfaceEOS
    : public IOnlineVoice,
      public TSharedFromThis<FOnlineVoiceInterfaceEOS, ESPMode::ThreadSafe>
{
private:
    TSharedRef<FEOSVoiceManager> VoiceManager;
    TSharedRef<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> IdentityEOS;

protected:
    virtual IVoiceEnginePtr CreateVoiceEngine() override;
    virtual void ProcessMuteChangeNotification() override;

public:
    FOnlineVoiceInterfaceEOS(
        TSharedRef<FEOSVoiceManager> InVoiceManager,
        TSharedRef<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentityEOS)
        : VoiceManager(MoveTemp(InVoiceManager))
        , IdentityEOS(MoveTemp(InIdentityEOS)){};
    UE_NONCOPYABLE(FOnlineVoiceInterfaceEOS);
    virtual ~FOnlineVoiceInterfaceEOS(){};

    virtual void StartNetworkedVoice(uint8 LocalUserNum) override;
    virtual void StopNetworkedVoice(uint8 LocalUserNum) override;
    virtual bool RegisterLocalTalker(uint32 LocalUserNum) override;
    virtual void RegisterLocalTalkers() override;
    virtual bool UnregisterLocalTalker(uint32 LocalUserNum) override;
    virtual void UnregisterLocalTalkers() override;
    virtual bool RegisterRemoteTalker(const FUniqueNetId &UniqueId) override;
    virtual bool UnregisterRemoteTalker(const FUniqueNetId &UniqueId) override;
    virtual void RemoveAllRemoteTalkers() override;
    virtual bool IsHeadsetPresent(uint32 LocalUserNum) override;
    virtual bool IsLocalPlayerTalking(uint32 LocalUserNum) override;
    virtual bool IsRemotePlayerTalking(const FUniqueNetId &UniqueId) override;
    virtual bool IsMuted(uint32 LocalUserNum, const FUniqueNetId &UniqueId) const override;
    virtual bool MuteRemoteTalker(uint8 LocalUserNum, const FUniqueNetId &PlayerId, bool bIsSystemWide) override;
    virtual bool UnmuteRemoteTalker(uint8 LocalUserNum, const FUniqueNetId &PlayerId, bool bIsSystemWide) override;
    virtual TSharedPtr<class FVoicePacket> SerializeRemotePacket(FArchive &Ar) override;
    virtual TSharedPtr<class FVoicePacket> GetLocalPacket(uint32 LocalUserNum) override;
    virtual int32 GetNumLocalTalkers() override;
    virtual void ClearVoicePackets() override;
    virtual void Tick(float DeltaTime) override;
    virtual FString GetVoiceDebugState() const override;
};

#endif

EOS_DISABLE_STRICT_WARNINGS