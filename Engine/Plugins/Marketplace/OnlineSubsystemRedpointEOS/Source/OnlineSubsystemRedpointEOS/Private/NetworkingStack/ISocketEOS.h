// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "../../Shared/EOSCommon.h"
#include "Sockets.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_RetVal_ThreeParams(
    bool /* Accept */,
    FSocketEOSOnIncomingConnection,
    const TSharedRef<class ISocketEOS> & /* Socket */,
    const TSharedRef<class FUniqueNetIdEOS> & /* LocalUser */,
    const TSharedRef<class FUniqueNetIdEOS> &
    /* RemoteUser */);
DECLARE_DELEGATE_FourParams(
    FSocketEOSOnConnectionAccepted,
    const TSharedRef<class ISocketEOS> & /* ListeningSocket */,
    const TSharedRef<class ISocketEOS> & /* AcceptedSocket */,
    const TSharedRef<class FUniqueNetIdEOS> & /* LocalUser */,
    const TSharedRef<class FUniqueNetIdEOS> & /* RemoteUser */);
DECLARE_DELEGATE_TwoParams(
    FSocketEOSOnConnectionClosed,
    const TSharedRef<class ISocketEOS> & /* ListeningSocket */,
    const TSharedRef<class ISocketEOS> & /* ClosedSocket */);

class ISocketEOS : public FSocket
{
protected:
    inline ISocketEOS(ESocketType InSocketType, const FString &InSocketDescription, const FName &InSocketProtocol)
        : FSocket(InSocketType, InSocketDescription, InSocketProtocol)
        , OnIncomingConnection()
        , OnConnectionAccepted()
        , OnConnectionClosed()
        , bEnableReliableSending(false)
    {
    }

public:
    virtual ~ISocketEOS(){};

    virtual TSharedRef<ISocketEOS> AsSharedEOS() = 0;

    /**
     * When a new connection is arriving that can potentially send data to this socket, this event is called
     * to evaluate whether or not the connection should be accepted. Only one socket needs to return true
     * from this function in order for a connection to be accepted (and connections are evaluated at the socket ID
     * level, not the channel level, so it's possible for this handler to return false or not be called at all).
     */
    FSocketEOSOnIncomingConnection OnIncomingConnection;

    /**
     * When a new remote socket has been accepted by this socket, this event is called so the owner of the listening
     * socket (this socket) can appropriately update it state to reflect the new connection.
     */
    FSocketEOSOnConnectionAccepted OnConnectionAccepted;

    /**
     * When the remote host has closed the connection, this event is called.
     */
    FSocketEOSOnConnectionClosed OnConnectionClosed;

    /**
     * Off by default. If turned on, all send operations will use reliable ordered instead of unreliable unordered.
     */
    bool bEnableReliableSending;
};

EOS_DISABLE_STRICT_WARNINGS