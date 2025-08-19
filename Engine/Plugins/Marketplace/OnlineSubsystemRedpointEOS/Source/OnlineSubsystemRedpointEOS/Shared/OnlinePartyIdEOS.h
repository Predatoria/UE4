// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlinePartyInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlinePartyIdEOS : public FOnlinePartyId
{
private:
    const char *AnsiData;
    int32 AnsiDataLen;

public:
    UE_NONCOPYABLE(FOnlinePartyIdEOS);
    FOnlinePartyIdEOS(EOS_LobbyId InId);
    ~FOnlinePartyIdEOS();

    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;
    virtual FString ToDebugString() const override;
    virtual EOS_LobbyId GetLobbyId() const;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION