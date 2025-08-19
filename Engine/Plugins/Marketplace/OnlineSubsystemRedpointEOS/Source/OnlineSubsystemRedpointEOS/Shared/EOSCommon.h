// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "ProfilingDebugging/CountersTrace.h"
#include "Stats/Stats.h"
#include <functional>
#include <limits>

#if defined(UNREAL_CODE_ANALYZER)
#if UNREAL_CODE_ANALYZER
// HACK: When running under clang-tidy, these get mapped to stdcall attributes for Windows, which causes all sorts of
// weird compilation errors that aren't true when actually building under MSVC.
#define EOS_CALL
#define EOS_MEMORY_CALL
#endif
#endif

#if defined(EOS_BUILD_PLATFORM_NAME)
#include "eos_platform_prereqs.h"

#include "eos_base.h"
#endif

#include "eos_version.h"

#define EOS_VERSION_AT_LEAST(MAJOR, MINOR, PATCH)                                                                      \
    ((EOS_MAJOR_VERSION > (MAJOR)) || (EOS_MAJOR_VERSION == (MAJOR) && EOS_MINOR_VERSION > (MINOR)) ||                 \
     (EOS_MAJOR_VERSION == (MAJOR) && EOS_MINOR_VERSION == (MINOR) && EOS_PATCH_VERSION >= (PATCH)))
#define EOS_VERSION_AT_MOST(MAJOR, MINOR, PATCH)                                                                       \
    ((EOS_MAJOR_VERSION < (MAJOR)) || (EOS_MAJOR_VERSION == (MAJOR) && EOS_MINOR_VERSION < (MINOR)) ||                 \
     (EOS_MAJOR_VERSION == (MAJOR) && EOS_MINOR_VERSION == (MINOR) && EOS_PATCH_VERSION <= (PATCH)))

#include "eos_achievements.h"
#include "eos_auth.h"
#include "eos_common.h"
#include "eos_connect.h"
#include "eos_ecom.h"
#include "eos_friends.h"
#include "eos_leaderboards.h"
#include "eos_lobby.h"
#include "eos_logging.h"
#include "eos_metrics.h"
#include "eos_p2p.h"
#include "eos_playerdatastorage.h"
#include "eos_presence.h"
#include "eos_sdk.h"
#include "eos_sessions.h"
#include "eos_stats.h"
#if EOS_VERSION_AT_LEAST(1, 8, 0)
#include "eos_titlestorage.h"
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
#include "eos_reports.h"
#include "eos_sanctions.h"
#endif
#include "eos_ui.h"
#include "eos_userinfo.h"
#if PLATFORM_IOS
#include "eos_ios.h"
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
#include "eos_anticheatclient.h"
#if WITH_SERVER_CODE
#include "eos_anticheatserver.h"
#endif
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0)
#include "eos_rtc.h"
#include "eos_rtc_admin.h"
#include "eos_rtc_audio.h"
#define EOS_VOICE_CHAT_SUPPORTED 1
#endif

EOS_ENABLE_STRICT_WARNINGS

#if defined(EOS_SUBSYSTEM)
#undef EOS_SUBSYSTEM
#endif
#define REDPOINT_EOS_SUBSYSTEM FName(TEXT("RedpointEOS"))
#define REDPOINT_EAS_SUBSYSTEM FName(TEXT("RedpointEAS"))

// Used as the type for unique net IDs of EOS sessions (not users).
#define REDPOINT_EOS_SUBSYSTEM_SESSION FName(TEXT("RedpointEOS.Session"))

#define EOS_CHANNEL_ID_TYPE uint8
// We add one here because to get a range of numbers between e.g. 0 - 15 you would need to modulo by 16.
// We subtract one here because we use channel 255 as a control channel to reset sockets when a channel during
// re-connects (where a full disconnected through EOS_P2P_CloseConnection did not happen because other sockets kept the
// main connection alive).
#define EOS_CHANNEL_ID_MODULO (std::numeric_limits<EOS_CHANNEL_ID_TYPE>().max() + 1 - 1)
#define EOS_CHANNEL_ID_CONTROL (std::numeric_limits<EOS_CHANNEL_ID_TYPE>().max())

#ifndef EOS_P2P_SOCKET_NAME_MAX_LENGTH
#define EOS_P2P_SOCKET_NAME_MAX_LENGTH 32
#endif

#define INTERNET_ADDR_EOS_P2P_DOMAIN_SUFFIX TEXT("eosp2p")

#if PLATFORM_ANDROID
#define __CDECL_ATTR
#else
#define __CDECL_ATTR __cdecl
#endif

ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOS, Verbose, All);
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSIdentity, Verbose, Verbose);
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSSocket, Warning, Verbose);
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSNetworkTrace, Verbose, All);
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSNetworkAuth, Verbose, All);
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSStat, Warning, Verbose);
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSAntiCheat, Verbose, All);
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSFriends, Warning, Verbose);
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSFileTransfer, Warning, Verbose);
#if defined(EOS_IS_FREE_EDITION)
ONLINESUBSYSTEMREDPOINTEOS_API DECLARE_LOG_CATEGORY_EXTERN(LogEOSLicenseValidation, All, All);
#endif

#if defined(EOS_ENABLE_TRACE)
DECLARE_STATS_GROUP(TEXT("EOS"), STATGROUP_EOS, STATCAT_Advanced);

DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/RunOperation/Invoke"),
    STAT_EOSOpInvoke,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/RunOperation/Callback"),
    STAT_EOSOpCallback,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/RunOperationKeepAlive/Invoke"),
    STAT_EOSOpKeepAliveInvoke,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/RunOperationKeepAlive/Callback"),
    STAT_EOSOpKeepAliveCallback,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/RegisterEvent/Register"),
    STAT_EOSEvRegister,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/RegisterEvent/Callback"),
    STAT_EOSEvCallback,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/RegisterEvent/Deregister"),
    STAT_EOSEvDeregister,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);

DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/OnlineSubsystem/Init"),
    STAT_EOSOnlineSubsystemInit,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/OnlineSubsystem/Tick"),
    STAT_EOSOnlineSubsystemTick,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/OnlineSubsystem/Shutdown"),
    STAT_EOSOnlineSubsystemShutdown,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);

DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/NetDriver/TickDispatch"),
    STAT_EOSNetDriverTickDispatch,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/NetDriver/BaseTickDispatch"),
    STAT_EOSNetDriverBaseTickDispatch,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/NetDriver/OnIncomingConnection"),
    STAT_EOSNetDriverOnIncomingConnection,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/NetDriver/OnConnectionAccepted"),
    STAT_EOSNetDriverOnConnectionAccepted,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/NetDriver/OnConnectionClosed"),
    STAT_EOSNetDriverOnConnectionClosed,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/NetDriver/InitConnect"),
    STAT_EOSNetDriverInitConnect,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/NetDriver/InitListen"),
    STAT_EOSNetDriverInitListen,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);

DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/Socket/RecvFrom"),
    STAT_EOSSocketRecvFrom,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/Socket/HasPendingData"),
    STAT_EOSSocketHasPendingData,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_CYCLE_STAT_EXTERN(
    TEXT("EOS/Socket/SendTo"),
    STAT_EOSSocketSendTo,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);

DECLARE_DWORD_COUNTER_STAT_EXTERN(
    TEXT("EOS/P2P/ReceivedLoopIters"),
    STAT_EOSNetP2PReceivedLoopIters,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_DWORD_COUNTER_STAT_EXTERN(
    TEXT("EOS/P2P/ReceivedPackets"),
    STAT_EOSNetP2PReceivedPackets,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_DWORD_COUNTER_STAT_EXTERN(
    TEXT("EOS/P2P/ReceivedBytes"),
    STAT_EOSNetP2PReceivedBytes,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_DWORD_COUNTER_STAT_EXTERN(
    TEXT("EOS/P2P/SentPackets"),
    STAT_EOSNetP2PSentPackets,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_DWORD_COUNTER_STAT_EXTERN(
    TEXT("EOS/P2P/SentBytes"),
    STAT_EOSNetP2PSentBytes,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
DECLARE_DWORD_COUNTER_STAT_EXTERN(
    TEXT("EOS/RegulatedTicks/InvocationsLastSecond"),
    STAT_EOSRegulatedTicksInvocationsLastSecond,
    STATGROUP_EOS,
    ONLINESUBSYSTEMREDPOINTEOS_API);
TRACE_DECLARE_INT_COUNTER_EXTERN(CTR_EOSNetP2PReceivedLoopIters);
TRACE_DECLARE_INT_COUNTER_EXTERN(CTR_EOSNetP2PReceivedPackets);
TRACE_DECLARE_INT_COUNTER_EXTERN(CTR_EOSNetP2PReceivedBytes);
TRACE_DECLARE_INT_COUNTER_EXTERN(CTR_EOSNetP2PSentPackets);
TRACE_DECLARE_INT_COUNTER_EXTERN(CTR_EOSNetP2PSentBytes);
TRACE_DECLARE_INT_COUNTER_EXTERN(CTR_EOSRegulatedTicksInvocationsLastSecond);

#define EOS_SCOPE_CYCLE_COUNTER(StatName) SCOPE_CYCLE_COUNTER(StatName)
#define EOS_TRACE_COUNTER_SET(Ctr, CtrValue) TRACE_COUNTER_SET(Ctr, CtrValue)
#define EOS_TRACE_COUNTER_INCREMENT(Ctr) TRACE_COUNTER_INCREMENT(Ctr)
#define EOS_TRACE_COUNTER_ADD(Ctr, CtrValue) TRACE_COUNTER_ADD(Ctr, CtrValue)
#define EOS_INC_DWORD_STAT(StatName) INC_DWORD_STAT(StatName)
#define EOS_INC_DWORD_STAT_BY(StatName, StatValue) INC_DWORD_STAT_BY(StatName, StatValue)
#else
#define EOS_SCOPE_CYCLE_COUNTER(StatName)
#define EOS_TRACE_COUNTER_SET(Ctr, CtrValue)
#define EOS_TRACE_COUNTER_INCREMENT(Ctr)
#define EOS_TRACE_COUNTER_ADD(Ctr, CtrValue)
#define EOS_INC_DWORD_STAT(StatName)
#define EOS_INC_DWORD_STAT_BY(StatName, StatValue)
#endif

// The _MAX_LENGTH definitions in the SDK do not include the null terminator, but both the buffer and the
// length you pass into functions such as () must include the null terminator
// in their length (it's not enough to just add one when allocating the destination array, and then pass
// EOS_EPICACCOUNTID_MAX_LENGTH into EOS_EpicAccountId_ToString; you have to add one in both places). To make
// things easier, we define a _MAX_BUFFER_LEN for all of these _MAX_LENGTH values that include the null
// terminator, and then we use these definitions instead.
#define EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_BUFFER_LEN (EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH + 1)
#define EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_BUFFER_LEN (EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH + 1)
#define EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_BUFFER_LEN (EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH + 1)
#define EOS_LOBBY_INVITEID_MAX_BUFFER_LEN (EOS_LOBBY_INVITEID_MAX_LENGTH + 1)
#define EOS_PLAYERDATASTORAGE_FILENAME_MAX_BUFFER_BYTES_LEN (EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES + 1)
#define EOS_PRESENCEMODIFICATION_JOININFO_MAX_BUFFER_LEN (EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH + 1)
#define EOS_SESSIONS_INVITEID_MAX_BUFFER_LEN (EOS_SESSIONS_INVITEID_MAX_LENGTH + 1)
#define EOS_TITLESTORAGE_FILENAME_MAX_BUFFER_BYTES_LEN (EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES + 1)

/**
 * Runs an EOS SDK operation with the specified callback. This helper handles the mess of moving
 * a lambda to the heap, passing it's pointer through the ClientData and freeing the lambda after
 * the callback has run.
 */
template <typename TInter, typename TOpts, typename TResult>
void EOSRunOperation(
    TInter Inter,
    const TOpts *Opts,
    std::function<void(TInter, const TOpts *, void *, void(__CDECL_ATTR *)(const TResult *))> Op,
    std::function<void(const TResult *)> Callback)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOpInvoke);

    std::function<void(const TResult *)> *CallbackOnHeap =
        new std::function<void(const TResult *)>(std::move(Callback));

    Op(Inter, Opts, CallbackOnHeap, [](const TResult *Data) {
        EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOpCallback);

        if (!EOS_EResult_IsOperationComplete(Data->ResultCode))
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("EOSRunOperation got non-complete result status of %s, deferring callback."),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
            return;
        }

        auto CallbackOnHeap = (std::function<void(const TResult *)> *)Data->ClientData;
        (*CallbackOnHeap)(Data);
        delete CallbackOnHeap;
    });
};

/**
 * A variant of EOSRunOperation that handles operations that take non-const Options. This appears to be the case for
 * some RTC operations.
 */
template <typename TInter, typename TOpts, typename TResult>
void EOSRunOperation(
    TInter Inter,
    TOpts *Opts,
    std::function<void(TInter, TOpts *, void *, void(__CDECL_ATTR *)(const TResult *))> Op,
    std::function<void(const TResult *)> Callback)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOpInvoke);

    std::function<void(const TResult *)> *CallbackOnHeap =
        new std::function<void(const TResult *)>(std::move(Callback));

    Op(Inter, Opts, CallbackOnHeap, [](const TResult *Data) {
        EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOpCallback);

        if (!EOS_EResult_IsOperationComplete(Data->ResultCode))
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("EOSRunOperation got non-complete result status of %s, deferring callback."),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
            return;
        }

        auto CallbackOnHeap = (std::function<void(const TResult *)> *)Data->ClientData;
        (*CallbackOnHeap)(Data);
        delete CallbackOnHeap;
    });
};

/**
 * A variant of EOSRunOperation that allows you to keep the callback alive if you know
 * the SDK will call it again. Used by EOS_Auth_Login when handling device codes.
 */
template <typename TInter, typename TOpts, typename TResult>
void EOSRunOperationKeepAlive(
    TInter Inter,
    const TOpts *Opts,
    std::function<void(TInter, const TOpts *, void *, void(__CDECL_ATTR *)(const TResult *))> Op,
    std::function<void(const TResult *, bool &)> Callback)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOpKeepAliveInvoke);

    std::function<void(const TResult *, bool &)> *CallbackOnHeap =
        new std::function<void(const TResult *, bool &)>(std::move(Callback));

    Op(Inter, Opts, CallbackOnHeap, [](const TResult *Data) {
        EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOpKeepAliveCallback);

        if (!EOS_EResult_IsOperationComplete(Data->ResultCode) &&
            Data->ResultCode != EOS_EResult::EOS_Auth_PinGrantCode)
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("EOSRunOperationKeepAlive got non-complete result status of %s, deferring callback."),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
            return;
        }

        bool KeepAlive = false;
        auto CallbackOnHeap = (std::function<void(const TResult *, bool &)> *)Data->ClientData;
        (*CallbackOnHeap)(Data, KeepAlive);
        if (!KeepAlive)
        {
            delete CallbackOnHeap;
        }
    });
};

/**
 * An EOS event handle that wraps EOS_NotificationId and handles deregistering events when
 * the EOS handle is destructed (due to no more shared pointer references).
 */
template <typename TResult> class EOSEventHandle : public TSharedFromThis<EOSEventHandle<TResult>>
{
private:
    std::function<void()> DeregisterEvent;
    std::function<void(const TResult *)> Callback;
    EOS_NotificationId NotificationId;

public:
    UE_NONCOPYABLE(EOSEventHandle);
    EOSEventHandle(std::function<void(const TResult *)> InCallback)
        : DeregisterEvent([]() {
        })
        , Callback(InCallback)
        , NotificationId(EOS_INVALID_NOTIFICATIONID){};
    ~EOSEventHandle()
    {
        // Note: If you are getting a crash in this code, it's most likely because the underlying EOS_HPlatform
        // has been released (because the online subsystem has been released), even though there were still
        // events registered.
        //
        // You should ensure that any objects that are using events have a chain of shared pointers that can
        // be traced from the object that registered the event up to the online subsystem. This will ensure
        // that the object with the event registered will prevent the online subsystem from being destroyed
        // (or more accurately, the online subsystem check() will fail because there are still alive objects,
        // and then you can go and fix up the code to clean up the objects that still have events registered).

        if (this->NotificationId != EOS_INVALID_NOTIFICATIONID)
        {
            // Only call deregister event if the notification ID is valid.
            this->DeregisterEvent();
        }
        this->NotificationId = EOS_INVALID_NOTIFICATIONID;
    }
    void SetNotificationId(EOS_NotificationId InNotificationId)
    {
        check(this->NotificationId == EOS_INVALID_NOTIFICATIONID);
        this->NotificationId = InNotificationId;
    }
    void SetDeregisterEvent(const std::function<void()> &InDeregisterEvent)
    {
        this->DeregisterEvent = InDeregisterEvent;
    }
    void RunCallback(const TResult *Data)
    {
        this->Callback(Data);
    }
    bool IsValid()
    {
        return this->NotificationId != EOS_INVALID_NOTIFICATIONID;
    }
};

/**
 * Registers a new EOS event handler in the SDK, and returns the event handle. The event will be
 * automatically deregistered when there are no more references to the event handle.
 */
template <typename TInter, typename TAddOpts, typename TResult>
TSharedPtr<EOSEventHandle<TResult>> EOSRegisterEvent(
    TInter Inter,
    const TAddOpts *AddOpts,
    std::function<EOS_NotificationId(TInter Inter, const TAddOpts *Opts, void *, void(__CDECL_ATTR *)(const TResult *))>
        Register,
    std::function<void(TInter Inter, EOS_NotificationId InId)> Deregister,
    std::function<void(const TResult *)> Callback)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSEvRegister);

    TSharedPtr<EOSEventHandle<TResult>> EventHandle = MakeShared<EOSEventHandle<TResult>>(Callback);

    auto NotificationId = Register(Inter, AddOpts, EventHandle.Get(), [](const TResult *Data) {
        EOS_SCOPE_CYCLE_COUNTER(STAT_EOSEvCallback);

        TSharedRef<EOSEventHandle<TResult>> EventHandleLoaded =
            ((EOSEventHandle<TResult> *)Data->ClientData)->AsShared();
        EventHandleLoaded->RunCallback(Data);
    });
    EventHandle->SetNotificationId(NotificationId);
    EventHandle->SetDeregisterEvent([Deregister, Inter, NotificationId]() {
        EOS_SCOPE_CYCLE_COUNTER(STAT_EOSEvDeregister);

        Deregister(Inter, NotificationId);
    });

    return EventHandle;
}

/**
 * A variant of EOSRegisterEvent that handles operations that take non-const Options. This appears to be the case for
 * some RTC operations.
 */
template <typename TInter, typename TAddOpts, typename TResult>
TSharedPtr<EOSEventHandle<TResult>> EOSRegisterEvent(
    TInter Inter,
    TAddOpts *AddOpts,
    std::function<EOS_NotificationId(TInter Inter, TAddOpts *Opts, void *, void(__CDECL_ATTR *)(const TResult *))>
        Register,
    std::function<void(TInter Inter, EOS_NotificationId InId)> Deregister,
    std::function<void(const TResult *)> Callback)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSEvRegister);

    TSharedPtr<EOSEventHandle<TResult>> EventHandle = MakeShared<EOSEventHandle<TResult>>(Callback);

    auto NotificationId = Register(Inter, AddOpts, EventHandle.Get(), [](const TResult *Data) {
        EOS_SCOPE_CYCLE_COUNTER(STAT_EOSEvCallback);

        TSharedRef<EOSEventHandle<TResult>> EventHandleLoaded =
            ((EOSEventHandle<TResult> *)Data->ClientData)->AsShared();
        EventHandleLoaded->RunCallback(Data);
    });
    EventHandle->SetNotificationId(NotificationId);
    EventHandle->SetDeregisterEvent([Deregister, Inter, NotificationId]() {
        EOS_SCOPE_CYCLE_COUNTER(STAT_EOSEvDeregister);

        Deregister(Inter, NotificationId);
    });

    return EventHandle;
}

ONLINESUBSYSTEMREDPOINTEOS_API void EOSSendCustomEditorSignal(const FString &Context, const FString &SignalId);

EOS_DISABLE_STRICT_WARNINGS
