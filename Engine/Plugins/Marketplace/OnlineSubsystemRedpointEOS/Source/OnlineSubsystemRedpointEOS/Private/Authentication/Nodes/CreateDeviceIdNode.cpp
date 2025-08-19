// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/CreateDeviceIdNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

void FCreateDeviceIdNode::Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone)
{
    EOS_Connect_CreateDeviceIdOptions Opts = {};
    Opts.ApiVersion = EOS_CONNECT_CREATEDEVICEID_API_LATEST;
    Opts.DeviceModel = "EOS Anonymous Login";

    EOSRunOperation<EOS_HConnect, EOS_Connect_CreateDeviceIdOptions, EOS_Connect_CreateDeviceIdCallbackInfo>(
        State->EOSConnect,
        &Opts,
        &EOS_Connect_CreateDeviceId,
        [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Connect_CreateDeviceIdCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_Success ||
                    Data->ResultCode == EOS_EResult::EOS_DuplicateNotAllowed)
                {
                    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                    return;
                }

                State->ErrorMessages.Add(FString::Printf(
                    TEXT("Unable to create Device Id, got result code %s"),
                    ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                return;
            }
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION