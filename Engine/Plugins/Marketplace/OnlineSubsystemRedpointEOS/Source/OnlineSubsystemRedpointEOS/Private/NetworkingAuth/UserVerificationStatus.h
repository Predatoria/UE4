// Copyright June Rhodes. All Rights Reserved.

#pragma once

enum EUserVerificationStatus
{
    /** We haven't started verifying this user yet. */
    NotStarted,
    /** We check if the connecting user exists using the IOnlineUser interface. */
    CheckingAccountExistsFromListenServer,
    /** We check if the connecting user exists by impersonating them with EOS_Connect_Login. */
    CheckingAccountExistsFromDedicatedServer,
    /** We're checking that the user isn't sanctioned with a BAN action. */
    CheckingSanctions,
    /** We're establishing whether the client can run as an unprotected Anti-Cheat client. */
    EstablishingAntiCheatProof,
    /** We're waiting for Anti-Cheat integrity to be confirmed before finishing the connection. */
    WaitingForAntiCheatIntegrity,
    /** We've verified the user and login can proceed. */
    Verified,
    /** We couldn't verify the user and they're not permitted to connect. */
    Failed,
};