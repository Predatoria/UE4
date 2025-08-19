// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if EOS_DISCORD_ENABLED

#include "DiscordGameSDK.h"
#include "Interfaces/OnlinePresenceInterface.h"

class FOnlineUserPresenceRedpointDiscord : public FOnlineUserPresence,
                                           public TSharedFromThis<FOnlineUserPresenceRedpointDiscord>
{
public:
    FOnlineUserPresenceRedpointDiscord();
    UE_NONCOPYABLE(FOnlineUserPresenceRedpointDiscord);

    void UpdateFromPresence(const discord::Presence &InNewPresence);
};

#endif