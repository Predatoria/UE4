// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "../../../Shared/EOSCommon.h"
#include "../IInternetAddrEOS.h"
#include "CoreMinimal.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FInternetAddrEOSFull : public IInternetAddrEOS
{
private:
    EOS_ProductUserId UserId;
    FString SymmetricSocketName;
    EOS_CHANNEL_ID_TYPE SymmetricChannel;

public:
    FInternetAddrEOSFull(
        EOS_ProductUserId InUserId,
        const FString &InSymmetricSocketName,
        EOS_CHANNEL_ID_TYPE InSymmetricChannel);
    FInternetAddrEOSFull();

    virtual void SetFromParameters(
        EOS_ProductUserId InUserId,
        const FString &InSymmetricSocketName,
        EOS_CHANNEL_ID_TYPE InSymmetricChannel) override;
    virtual EOS_ProductUserId GetUserId() const override;
    FString GetSymmetricSocketName() const;
    EOS_P2P_SocketId GetSymmetricSocketId() const;

    virtual void SetIp(uint32 InAddr) override;
    virtual void SetIp(const TCHAR *InAddr, bool &bIsValid) override;
    virtual void GetIp(uint32 &OutAddr) const override;
    virtual void SetPort(int32 InPort) override;
    virtual int32 GetPort() const override;
    virtual void SetSymmetricChannel(EOS_CHANNEL_ID_TYPE InSymmetricChannel);
    virtual EOS_CHANNEL_ID_TYPE GetSymmetricChannel() const;
    virtual void SetRawIp(const TArray<uint8> &RawAddr) override;
    virtual TArray<uint8> GetRawIp() const override;
    virtual void SetAnyAddress() override;
    virtual void SetBroadcastAddress() override;
    virtual void SetLoopbackAddress() override;
    virtual FString ToString(bool bAppendPort) const override;
    virtual uint32 GetTypeHash() const override;
    virtual bool IsValid() const override;
    virtual TSharedRef<FInternetAddr> Clone() const override;

    virtual bool operator==(const FInternetAddr &Other) const override;

    virtual FName GetProtocolType() const
    {
        return REDPOINT_EOS_SUBSYSTEM;
    }
};

EOS_DISABLE_STRICT_WARNINGS