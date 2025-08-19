// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

#if WITH_EDITOR
#include "Modules/ModuleManager.h"
#include "OnlineSubsystemRedpointEOS/Public/OnlineSubsystemRedpointEOSModule.h"
#endif

EOS_ENABLE_STRICT_WARNINGS

DEFINE_LOG_CATEGORY(LogEOS);
DEFINE_LOG_CATEGORY(LogEOSIdentity);
DEFINE_LOG_CATEGORY(LogEOSSocket);
DEFINE_LOG_CATEGORY(LogEOSNetworkTrace);
DEFINE_LOG_CATEGORY(LogEOSNetworkAuth);
DEFINE_LOG_CATEGORY(LogEOSStat);
DEFINE_LOG_CATEGORY(LogEOSAntiCheat);
DEFINE_LOG_CATEGORY(LogEOSFriends);
DEFINE_LOG_CATEGORY(LogEOSFileTransfer);
#if defined(EOS_IS_FREE_EDITION)
DEFINE_LOG_CATEGORY(LogEOSLicenseValidation);
#endif

#if defined(EOS_ENABLE_TRACE)
DEFINE_STAT(STAT_EOSOpInvoke);
DEFINE_STAT(STAT_EOSOpCallback);
DEFINE_STAT(STAT_EOSOpKeepAliveInvoke);
DEFINE_STAT(STAT_EOSOpKeepAliveCallback);
DEFINE_STAT(STAT_EOSEvRegister);
DEFINE_STAT(STAT_EOSEvCallback);
DEFINE_STAT(STAT_EOSEvDeregister);

DEFINE_STAT(STAT_EOSOnlineSubsystemInit);
DEFINE_STAT(STAT_EOSOnlineSubsystemTick);
DEFINE_STAT(STAT_EOSOnlineSubsystemShutdown);

DEFINE_STAT(STAT_EOSNetDriverTickDispatch);
DEFINE_STAT(STAT_EOSNetDriverBaseTickDispatch);
DEFINE_STAT(STAT_EOSNetDriverOnIncomingConnection);
DEFINE_STAT(STAT_EOSNetDriverOnConnectionAccepted);
DEFINE_STAT(STAT_EOSNetDriverOnConnectionClosed);
DEFINE_STAT(STAT_EOSNetDriverInitConnect);
DEFINE_STAT(STAT_EOSNetDriverInitListen);

DEFINE_STAT(STAT_EOSSocketRecvFrom);
DEFINE_STAT(STAT_EOSSocketHasPendingData);
DEFINE_STAT(STAT_EOSSocketSendTo);

DEFINE_STAT(STAT_EOSNetP2PReceivedLoopIters);
DEFINE_STAT(STAT_EOSNetP2PReceivedPackets);
DEFINE_STAT(STAT_EOSNetP2PReceivedBytes);
DEFINE_STAT(STAT_EOSNetP2PSentPackets);
DEFINE_STAT(STAT_EOSNetP2PSentBytes);
TRACE_DECLARE_INT_COUNTER(CTR_EOSNetP2PReceivedLoopIters, TEXT("EOS/P2P/ReceivedLoopIters"));
TRACE_DECLARE_INT_COUNTER(CTR_EOSNetP2PReceivedPackets, TEXT("EOS/P2P/ReceivedPackets"));
TRACE_DECLARE_INT_COUNTER(CTR_EOSNetP2PReceivedBytes, TEXT("EOS/P2P/ReceivedBytes"));
TRACE_DECLARE_INT_COUNTER(CTR_EOSNetP2PSentPackets, TEXT("EOS/P2P/SentPackets"));
TRACE_DECLARE_INT_COUNTER(CTR_EOSNetP2PSentBytes, TEXT("EOS/P2P/SentBytes"));
TRACE_DECLARE_INT_COUNTER(CTR_EOSRegulatedTicksInvocationsLastSecond, TEXT("EOS/TickRegulation/InvocationsLastSecond"));
#endif

void EOSSendCustomEditorSignal(const FString &Context, const FString &SignalId)
{
#if WITH_EDITOR
    FModuleManager &ModuleManager = FModuleManager::Get();
    auto RuntimeModule = (FOnlineSubsystemRedpointEOSModule *)ModuleManager.GetModule("OnlineSubsystemRedpointEOS");
    if (RuntimeModule != nullptr)
    {
        RuntimeModule->SendCustomSignal(Context, SignalId);
    }
#endif
}

EOS_DISABLE_STRICT_WARNINGS
