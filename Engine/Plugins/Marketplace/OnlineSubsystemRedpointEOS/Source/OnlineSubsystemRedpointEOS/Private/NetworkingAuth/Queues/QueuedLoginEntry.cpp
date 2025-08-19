// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/Queues/QueuedLoginEntry.h"

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhaseContext.h"

void FQueuedLoginEntry::SetContext(const TSharedRef<IAuthPhaseContext> &InContext)
{
    this->Context = StaticCastSharedRef<FAuthLoginPhaseContext>(InContext);
}

void FQueuedLoginEntry::SendSuccess()
{
    if (!ControlChannel.IsValid())
    {
        UE_LOG(LogEOSNetworkAuth, Error, TEXT("Control channel is no longer valid, not sending NMT_Login packet."));
        return;
    }

    FOutBunch Bunch;
    Bunch << ClientResponse;
    Bunch << URLString;
    Bunch << IncomingUser;
    Bunch << OnlinePlatformNameString;
    FInBunch InBunch(ControlChannel->Connection, Bunch.GetData(), Bunch.GetNumBits());
    ControlChannel->Connection->Driver->Notify->NotifyControlMessage(ControlChannel->Connection, NMT_Login, InBunch);
}

void FQueuedLoginEntry::SendFailure(const FString &ErrorMessage)
{
    if (!ControlChannel.IsValid())
    {
        UE_LOG(LogEOSNetworkAuth, Error, TEXT("Control channel is no longer valid, not sending NMT_Failure packet."));
        return;
    }

    FString ErrorMessageTemp = ErrorMessage;
    while (ErrorMessageTemp.RemoveFromEnd(TEXT(".")))
        ;
    FString UserErrorMessage = FString::Printf(
        TEXT("%s. You have been disconnected."),
        ErrorMessageTemp.IsEmpty() ? TEXT("Failed to verify your account or EAC integrity on connection")
                                   : *ErrorMessageTemp);
    FNetControlMessage<NMT_Failure>::Send(ControlChannel->Connection, UserErrorMessage);
    ControlChannel->Connection->FlushNet(true);
    ControlChannel->Connection->Close();
}

bool FQueuedLoginEntry::Contains()
{
    return ControlChannel->QueuedLogins.Contains(*IncomingUser.GetUniqueNetId());
}

void FQueuedLoginEntry::Track()
{
    ControlChannel->QueuedLogins.Add(*IncomingUser.GetUniqueNetId(), this->AsShared());
}

void FQueuedLoginEntry::Release()
{
    ControlChannel->QueuedLogins.Remove(*IncomingUser.GetUniqueNetId());
}
