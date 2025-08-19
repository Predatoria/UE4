// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/Queues/QueuedBeaconEntry.h"

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthBeaconPhaseContext.h"

void FQueuedBeaconEntry::SetContext(const TSharedRef<IAuthPhaseContext> &InContext)
{
    this->Context = StaticCastSharedRef<FAuthBeaconPhaseContext>(InContext);
}

void FQueuedBeaconEntry::SendSuccess()
{
    if (!ControlChannel.IsValid())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Control channel is no longer valid, not sending NMT_BeaconJoin packet."));
        return;
    }

    FOutBunch Bunch;
    Bunch << BeaconType;
    Bunch << IncomingUser;
    FInBunch InBunch(ControlChannel->Connection, Bunch.GetData(), Bunch.GetNumBits());
    ControlChannel->Connection->Driver->Notify->NotifyControlMessage(
        ControlChannel->Connection,
        NMT_BeaconJoin,
        InBunch);
}

void FQueuedBeaconEntry::SendFailure(const FString &ErrorMessage)
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

bool FQueuedBeaconEntry::Contains()
{
    if (ControlChannel->QueuedBeacons.Contains(*IncomingUser.GetUniqueNetId()))
    {
        return ControlChannel->QueuedBeacons[*IncomingUser.GetUniqueNetId()].Contains(BeaconType);
    }
    return false;
}

void FQueuedBeaconEntry::Track()
{
    if (!ControlChannel->QueuedBeacons.Contains(*IncomingUser.GetUniqueNetId()))
    {
        ControlChannel->QueuedBeacons.Add(
            *IncomingUser.GetUniqueNetId(),
            TMap<FString, TSharedPtr<FQueuedBeaconEntry>>());
    }
    ControlChannel->QueuedBeacons[*IncomingUser.GetUniqueNetId()].Add(BeaconType, this->AsShared());
}

void FQueuedBeaconEntry::Release()
{
    if (!ControlChannel->QueuedBeacons.Contains(*IncomingUser.GetUniqueNetId()))
    {
        return;
    }
    ControlChannel->QueuedBeacons[*IncomingUser.GetUniqueNetId()].Remove(BeaconType);
}