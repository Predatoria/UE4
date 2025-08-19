// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineUserPresenceEAS : public FOnlineUserPresence, public TSharedFromThis<FOnlineUserPresenceEAS>
{
private:
    FString OurProductId;

public:
    FOnlineUserPresenceEAS(EOS_Presence_Info *InInitialPresence, const FString &InOurProductId);
    UE_NONCOPYABLE(FOnlineUserPresenceEAS);

    void UpdateFromPresence(EOS_Presence_Info *InNewPresence);
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION