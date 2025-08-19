// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/P2PAddressCheckPhase.h"

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/IInternetAddrEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

void FP2PAddressCheckPhase::RegisterRoutes(UEOSControlChannel *ControlChannel)
{
}

void FP2PAddressCheckPhase::Start(const TSharedRef<FAuthVerificationPhaseContext> &Context)
{
    auto Connection = Context->GetConnection();
    auto RemoteAddress = Connection->LowLevelGetRemoteAddress();

    if (RemoteAddress.Contains(TEXT(".eosp2p")))
    {
        auto SocketSubsystem = Context->GetSocketSubsystem();
        if (SocketSubsystem == nullptr)
        {
            Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessSocketSubsystem);
            return;
        }

        TSharedPtr<IInternetAddrEOS> InternetAddr =
            StaticCastSharedPtr<IInternetAddrEOS>(SocketSubsystem->GetAddressFromString(RemoteAddress));
        if (!InternetAddr.IsValid() || !EOS_ProductUserId_IsValid(InternetAddr->GetUserId()))
        {
            Context->Finish(EAuthPhaseFailureCode::Phase_P2PAddressCheck_InvalidSourceAddress);
            return;
        }

        TSharedRef<FUniqueNetIdEOS> UserId = MakeShared<FUniqueNetIdEOS>(InternetAddr->GetUserId());
        if (*UserId != *Context->GetUserId())
        {
            Context->Finish(EAuthPhaseFailureCode::Phase_P2PAddressCheck_UserIdDoesNotMatchSource);
            return;
        }
    }

    // Either the P2P address matches, or we're not hosting over P2P (in which case we
    // just permit clients to connect to IP-based listen servers with no authentication).
    Context->GetConnection()->PlayerId = FUniqueNetIdRepl(Context->GetUserId()->AsShared());
    Context->Finish(EAuthPhaseFailureCode::Success);
}