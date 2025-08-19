// Copyright June Rhodes. All Rights Reserved.

#include "OnlineEngineInterfaceEOS.h"

#include "Engine/NetConnection.h"
#include "Interfaces/OnlineChatInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "IpNetDriver.h"
#include "Misc/ConfigCacheIni.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemNames.h"
#include "OnlineSubsystemUtils.h"

// This is one giant hack.
//
// You see, the engine hard codes initializing it's UOnlineEngineInterface instance to
// /Script/OnlineSubsystemUtils.OnlineEngineInterfaceImpl, which propagates a hard coded AutoLogin call that sits inside
// AGameSession every time a multiplayer match starts. For *most* scenarios this doesn't cause problems, but if you're
// using EASOptional and are signed in without an EAS account, then the AutoLogin call gets treated as a "please link an
// EAS account" and opens the EAS web browser.
//
// The one workaround that's viable is that the interface is a UObject, which means we can redirect the implementation
// using CoreRedirects. We should be able to use a redirect to point
// /Script/OnlineSubsystemUtils.OnlineEngineInterfaceImpl at this instance instead.
//
// ... right? Wrong.
//
// The class is loaded with StaticLoadClass which only ever hits CoreRedirects for *package* level redirects, not class
// redirects. So the only thing we could do is replace OnlineSubsystemUtils in it's entirety. This is not viable because
// this is also the package that contains classes like UIpNetDriver, which we're not reimplementing any time soon.
//
// Instead, armed with our knowledge of the UE4 reflection system and some.. debatable usage of pointers to call
// LowLevelRename on the UClass instance, we rename our overriding class's StaticClass into the OnlineSubsystemUtils
// package. Then, later on when the class resolution happens inside the StaticLoadClass, it looks up the hash tables,
// finds the class it's looking for and it just so happens to be our UClass instead of the original UClass.

void UOnlineEngineInterfaceEOS::DoHack()
{
    UPackage *OnlineSubsystemUtils = FindPackage(nullptr, TEXT("/Script/OnlineSubsystemUtils"));

    (UOnlineEngineInterfaceEOS::StaticClass()->*&UOnlineEngineInterfaceEOS::LowLevelRename)(
        TEXT("OnlineEngineInterfaceImpl"),
        OnlineSubsystemUtils);
}

UOnlineEngineInterfaceEOS::UOnlineEngineInterfaceEOS(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
    , VoiceSubsystemNameOverride(NAME_None)
{
    TArray<FString> CompatibleUniqueNetIdTypesStr;
    this->CompatibleUniqueNetIdTypes.Empty();
    if (GConfig->GetArray(
            TEXT("/Script/OnlineSubsystemUtils.OnlineEngineInterfaceImpl"),
            TEXT("CompatibleUniqueNetIdTypes"),
            CompatibleUniqueNetIdTypesStr,
            GEngineIni) > 0)
    {
        for (const auto &Entry : CompatibleUniqueNetIdTypesStr)
        {
            this->CompatibleUniqueNetIdTypes.Add(FName(*Entry));
        }
    }

    FString VoiceSubsystemNameOverrideStr;
    if (GConfig->GetString(
            TEXT("/Script/OnlineSubsystemUtils.OnlineEngineInterfaceImpl"),
            TEXT("VoiceSubsystemNameOverride"),
            VoiceSubsystemNameOverrideStr,
            GEngineIni))
    {
        this->VoiceSubsystemNameOverride = FName(*VoiceSubsystemNameOverrideStr);
    }

#if defined(UE_5_0_OR_LATER)
    this->MappedUniqueNetIdTypes.Empty();
    FString MappedUniqueNetIdTypesStr;
    if (GConfig->GetString(
            TEXT("/Script/OnlineSubsystemUtils.OnlineEngineInterfaceImpl"),
            TEXT("MappedUniqueNetIdTypes"),
            MappedUniqueNetIdTypesStr,
            GEngineIni))
    {
        // Horrible, but we have to parse a map type from configuration manually.
        MappedUniqueNetIdTypesStr = MappedUniqueNetIdTypesStr.Mid(0, MappedUniqueNetIdTypesStr.Len() - 2);
        bool bInPair = false;
        bool bInQuote = false;
        int bPairIndexStart = 0;
        for (int i = 0; i < MappedUniqueNetIdTypesStr.Len(); i++)
        {
            if (MappedUniqueNetIdTypesStr[i] == TEXT('(') && !bInQuote)
            {
                bInPair = true;
                bPairIndexStart = i + 1;
            }
            else if (MappedUniqueNetIdTypesStr[i] == TEXT(')') && !bInQuote && bInPair)
            {
                bInPair = false;
                FString Pair = MappedUniqueNetIdTypesStr.Mid(bPairIndexStart, i - bPairIndexStart);
                TArray<FString> Elems;
                Pair.ParseIntoArray(Elems, TEXT(","));
                FString Key = Elems[0].Mid(1, Elems[0].Len() - 2);
                FString Value = Elems[1].Mid(1, Elems[0].Len() - 2);
                this->MappedUniqueNetIdTypes.Add(FName(*Key), FName(*Value));
            }
            else if (MappedUniqueNetIdTypesStr[i] == TEXT('"') && bInPair)
            {
                bInQuote = !bInQuote;
            }
            else if (MappedUniqueNetIdTypesStr[i] == TEXT(',') && !bInQuote && !bInPair)
            // NOLINTNEXTLINE(bugprone-branch-clone)
            {
                // This is the comma between entries.
            }
            else if (MappedUniqueNetIdTypesStr[i] == TEXT(',') && !bInQuote && bInPair)
            // NOLINTNEXTLINE(bugprone-branch-clone)
            {
                // This is the comma between the key-value pair.
            }
            else if (MappedUniqueNetIdTypesStr[i] == TEXT(' ') || MappedUniqueNetIdTypesStr[i] == TEXT('\t'))
            // NOLINTNEXTLINE(bugprone-branch-clone)
            {
            }
            else if (bInQuote)
            // NOLINTNEXTLINE(bugprone-branch-clone)
            {
            }
            else
            {
                checkf(
                    false,
                    TEXT("Unexpected character '%s' (bInPair: %s, bInQuote: %s) when parsing [/Script/OnlineSubsystemUtils.OnlineEngineInterfaceImpl] "
                         "MappedUniqueNetIdTypes configuration value!"),
                    *(FString() + MappedUniqueNetIdTypesStr[i]),
                    bInPair ? TEXT("true") : TEXT("false"),
                    bInQuote ? TEXT("true") : TEXT("false"));
            }
        }
    }
#endif
}

bool UOnlineEngineInterfaceEOS::IsLoaded(FName OnlineIdentifier)
{
    return IOnlineSubsystem::IsLoaded(OnlineIdentifier);
}

FName UOnlineEngineInterfaceEOS::GetOnlineIdentifier(FWorldContext &WorldContext)
{
    IOnlineSubsystemUtils *Utils = Online::GetUtils();
    if (Utils)
    {
        return Utils->GetOnlineIdentifier(WorldContext);
    }
    return NAME_None;
}

FName UOnlineEngineInterfaceEOS::GetOnlineIdentifier(UWorld *World)
{
    if (FWorldContext *WorldContext = GEngine->GetWorldContextFromWorld(World))
    {
        return GetOnlineIdentifier(*WorldContext);
    }

    return NAME_None;
}

bool UOnlineEngineInterfaceEOS::DoesInstanceExist(FName OnlineIdentifier)
{
    return IOnlineSubsystem::DoesInstanceExist(OnlineIdentifier);
}

void UOnlineEngineInterfaceEOS::ShutdownOnlineSubsystem(FName OnlineIdentifier)
{
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get(OnlineIdentifier);
    if (OnlineSub)
    {
        OnlineSub->Shutdown();
    }
}

void UOnlineEngineInterfaceEOS::DestroyOnlineSubsystem(FName OnlineIdentifier)
{
    IOnlineSubsystem::Destroy(OnlineIdentifier);
}

FName UOnlineEngineInterfaceEOS::GetDefaultOnlineSubsystemName() const
{
    // World context (PIE) isn't necessary here as it's just the name of the default
    // no matter how many instances actually exist
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();
    return OnlineSub ? OnlineSub->GetSubsystemName() : NAME_None;
}

uint8 UOnlineEngineInterfaceEOS::GetReplicationHashForSubsystem(FName InSubsystemName) const
{
    return Online::GetUtils()->GetReplicationHashForSubsystem(InSubsystemName);
}

FName UOnlineEngineInterfaceEOS::GetSubsystemFromReplicationHash(uint8 InHash) const
{
    return Online::GetUtils()->GetSubsystemFromReplicationHash(InHash);
}

FName UOnlineEngineInterfaceEOS::GetDedicatedServerSubsystemNameForSubsystem(const FName Subsystem) const
{
    // For console platforms with their own online subsystem, there may be a separate
    // online system that can run on dedicated servers, since the console one typically
    // won't compile/run on dedicated server platforms. The console and server OSSs should
    // maintain compatibility with serialized data, such as voice packets, so that the server
    // OSS can properly forward them to other clients using the console OSS.

    // Clients may send their platform subsystem name via the "OnlinePlatform=" login URL option,
    // then the server can pass the value of that option to this function to get the name of
    // the corresponding server OSS for that client, if one exists.

    PRAGMA_DISABLE_DEPRECATION_WARNINGS
    if (Subsystem == LIVE_SUBSYSTEM)
    {
        return LIVESERVER_SUBSYSTEM;
    }
    PRAGMA_ENABLE_DEPRECATION_WARNINGS
    else if (Subsystem == PS4_SUBSYSTEM)
    {
        return PS4SERVER_SUBSYSTEM;
    }

    return NAME_None;
}

#if defined(UE_5_0_OR_LATER)

FUniqueNetIdWrapper UOnlineEngineInterfaceEOS::CreateUniquePlayerIdWrapper(const FString &Str, FName Type)
{
    // Foreign types may be passed into this function, do not load OSS modules explicitly here
    FUniqueNetIdPtr UniqueId = nullptr;
    // Configuration driven mapping for UniqueNetIds so we don't treat the mapped ids as foreign
    const FName *MappedUniqueNetIdType = MappedUniqueNetIdTypes.Find(Type);

    bool bIsPrimaryLoaded = IsLoaded(Type);
    bool bIsMappedUniqueNetIdTypeLoaded = (MappedUniqueNetIdType != nullptr ? IsLoaded(*MappedUniqueNetIdType) : false);

    if (bIsPrimaryLoaded || bIsMappedUniqueNetIdTypeLoaded)
    {
        // No UWorld here, but ok since this is just a factory
        UWorld *World = nullptr;
        IOnlineIdentityPtr IdentityInt =
            Online::GetIdentityInterface(World, bIsPrimaryLoaded ? Type : *MappedUniqueNetIdType);
        if (IdentityInt.IsValid())
        {
            UniqueId = IdentityInt->CreateUniquePlayerId(Str);
        }
    }

    if (!UniqueId.IsValid())
    {
        if (IOnlineSubsystemUtils *Utils = Online::GetUtils())
        {
            // Create a unique id for other platforms unknown to this instance
            // Will not compare correctly against native types (do not use on platform where native type is available)
            // Used to maintain opaque unique id that will compare against other non native types
            UniqueId = Utils->CreateForeignUniqueNetId(Str, Type);
        }
    }
    return UniqueId;
}

FUniqueNetIdWrapper UOnlineEngineInterfaceEOS::GetUniquePlayerIdWrapper(UWorld *World, int32 LocalUserNum, FName Type)
{
    IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(World, Type);
    if (IdentityInt.IsValid())
    {
        FUniqueNetIdPtr UniqueId = IdentityInt->GetUniquePlayerId(LocalUserNum);
        return UniqueId;
    }

    UE_LOG_ONLINE(Verbose, TEXT("GetUniquePlayerId() returning null, can't find OSS of type %s"), *Type.ToString());
    return FUniqueNetIdWrapper();
}

#else

TSharedPtr<const FUniqueNetId> UOnlineEngineInterfaceEOS::CreateUniquePlayerId(const FString &Str, FName Type)
{
    // Foreign types may be passed into this function, do not load OSS modules explicitly here
    TSharedPtr<const FUniqueNetId> UniqueId = nullptr;
    if (IsLoaded(Type))
    {
        // No UWorld here, but ok since this is just a factory
        UWorld *World = nullptr;
        IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(World, Type);
        if (IdentityInt.IsValid())
        {
            UniqueId = IdentityInt->CreateUniquePlayerId(Str);
        }
    }

    if (!UniqueId.IsValid())
    {
        if (IOnlineSubsystemUtils *Utils = Online::GetUtils())
        {
            // Create a unique id for other platforms unknown to this instance
            // Will not compare correctly against native types (do not use on platform where native type is available)
            // Used to maintain opaque unique id that will compare against other non native types
            UniqueId = Utils->CreateForeignUniqueNetId(Str, Type);
        }
    }
    return UniqueId;
}

TSharedPtr<const FUniqueNetId> UOnlineEngineInterfaceEOS::GetUniquePlayerId(
    UWorld *World,
    int32 LocalUserNum,
    FName Type)
{
    IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(World, Type);
    if (IdentityInt.IsValid())
    {
        TSharedPtr<const FUniqueNetId> UniqueId = IdentityInt->GetUniquePlayerId(LocalUserNum);
        return UniqueId;
    }

    UE_LOG_ONLINE(Verbose, TEXT("GetUniquePlayerId() returning null, can't find OSS of type %s"), *Type.ToString());
    return nullptr;
}

#endif

#if defined(UE_5_0_OR_LATER)

FString UOnlineEngineInterfaceEOS::GetPlayerNickname(UWorld *World, const FUniqueNetIdWrapper &UniqueId)
{
    check(UniqueId.IsValid() && UniqueId.IsV1());
    IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(World, UniqueId.GetType());
    if (IdentityInt.IsValid())
    {
        return IdentityInt->GetPlayerNickname(*UniqueId);
    }

    static FString InvalidName(TEXT("InvalidOSSUser"));
    return InvalidName;
}

#else

FString UOnlineEngineInterfaceEOS::GetPlayerNickname(UWorld *World, const FUniqueNetId &UniqueId)
{
    IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(World, UniqueId.GetType());
    if (IdentityInt.IsValid())
    {
        return IdentityInt->GetPlayerNickname(UniqueId);
    }

    static FString InvalidName(TEXT("InvalidOSSUser"));
    return InvalidName;
}

#endif

bool UOnlineEngineInterfaceEOS::GetPlayerPlatformNickname(UWorld *World, int32 LocalUserNum, FString &OutNickname)
{
    IOnlineSubsystem *PlatformSubsystem = IOnlineSubsystem::GetByPlatform(false);
    if (PlatformSubsystem)
    {
        IOnlineIdentityPtr OnlineIdentityInt = PlatformSubsystem->GetIdentityInterface();
        if (OnlineIdentityInt.IsValid())
        {
            OutNickname = OnlineIdentityInt->GetPlayerNickname(LocalUserNum);
            if (!OutNickname.IsEmpty())
            {
                return true;
            }
        }
    }
    return false;
}

bool UOnlineEngineInterfaceEOS::AutoLogin(
    UWorld *World,
    int32 LocalUserNum,
    const FOnlineAutoLoginComplete &InCompletionDelegate)
{
    // No.
    return false;
}

bool UOnlineEngineInterfaceEOS::IsLoggedIn(UWorld *World, int32 LocalUserNum)
{
    IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(World);
    if (IdentityInt.IsValid())
    {
        return (IdentityInt->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn);
    }

    return false;
}

void UOnlineEngineInterfaceEOS::StartSession(
    UWorld *World,
    FName SessionName,
    FOnlineSessionStartComplete &InCompletionDelegate)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        FNamedOnlineSession *Session = SessionInt->GetNamedSession(SessionName);
        if (Session && (Session->SessionState == EOnlineSessionState::Pending ||
                        Session->SessionState == EOnlineSessionState::Ended))
        {
            FName OnlineIdentifier = GetOnlineIdentifier(World);

            FDelegateHandle StartSessionCompleteHandle =
                SessionInt->AddOnStartSessionCompleteDelegate_Handle(FOnStartSessionCompleteDelegate::CreateUObject(
                    this,
                    &ThisClass::OnStartSessionComplete,
                    OnlineIdentifier,
                    InCompletionDelegate));
            OnStartSessionCompleteDelegateHandles.Add(OnlineIdentifier, StartSessionCompleteHandle);

            SessionInt->StartSession(SessionName);
        }
        else
        {
            InCompletionDelegate.ExecuteIfBound(SessionName, false);
        }
    }
    else
    {
        InCompletionDelegate.ExecuteIfBound(SessionName, false);
    }
}

void UOnlineEngineInterfaceEOS::OnStartSessionComplete(
    FName SessionName,
    bool bWasSuccessful,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FName OnlineIdentifier,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnlineSessionStartComplete CompletionDelegate)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(OnlineIdentifier);
    if (SessionInt.IsValid())
    {
        // Cleanup the login delegate before calling create below
        FDelegateHandle *DelegateHandle = OnStartSessionCompleteDelegateHandles.Find(OnlineIdentifier);
        if (DelegateHandle)
        {
            SessionInt->ClearOnStartSessionCompleteDelegate_Handle(*DelegateHandle);
        }
    }

    CompletionDelegate.ExecuteIfBound(SessionName, bWasSuccessful);
}

void UOnlineEngineInterfaceEOS::EndSession(
    UWorld *World,
    FName SessionName,
    FOnlineSessionEndComplete &InCompletionDelegate)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        FName OnlineIdentifier = GetOnlineIdentifier(World);

        FDelegateHandle EndSessionCompleteHandle =
            SessionInt->AddOnEndSessionCompleteDelegate_Handle(FOnEndSessionCompleteDelegate::CreateUObject(
                this,
                &ThisClass::OnEndSessionComplete,
                OnlineIdentifier,
                InCompletionDelegate));
        OnEndSessionCompleteDelegateHandles.Add(OnlineIdentifier, EndSessionCompleteHandle);

        SessionInt->EndSession(SessionName);
    }
    else
    {
        InCompletionDelegate.ExecuteIfBound(SessionName, false);
    }
}

void UOnlineEngineInterfaceEOS::OnEndSessionComplete(
    FName SessionName,
    bool bWasSuccessful,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FName OnlineIdentifier,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnlineSessionEndComplete CompletionDelegate)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(OnlineIdentifier);
    if (SessionInt.IsValid())
    {
        FDelegateHandle *DelegateHandle = OnEndSessionCompleteDelegateHandles.Find(OnlineIdentifier);
        if (DelegateHandle)
        {
            SessionInt->ClearOnEndSessionCompleteDelegate_Handle(*DelegateHandle);
        }
    }

    CompletionDelegate.ExecuteIfBound(SessionName, bWasSuccessful);
}

bool UOnlineEngineInterfaceEOS::DoesSessionExist(UWorld *World, FName SessionName)
{
    FOnlineSessionSettings *SessionSettings = nullptr;
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        SessionSettings = SessionInt->GetSessionSettings(SessionName);
    }

    return SessionSettings != nullptr;
}

bool UOnlineEngineInterfaceEOS::GetSessionJoinability(
    UWorld *World,
    FName SessionName,
    FJoinabilitySettings &OutSettings)
{
    bool bValidData = false;

    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        FOnlineSessionSettings *SessionSettings = SessionInt->GetSessionSettings(SessionName);
        if (SessionSettings)
        {
            OutSettings.SessionName = SessionName;
            OutSettings.bPublicSearchable = SessionSettings->bShouldAdvertise;
            OutSettings.bAllowInvites = SessionSettings->bAllowInvites;
            OutSettings.bJoinViaPresence = SessionSettings->bAllowJoinViaPresence;
            OutSettings.bJoinViaPresenceFriendsOnly = SessionSettings->bAllowJoinViaPresenceFriendsOnly;
            bValidData = true;
        }
    }

    return bValidData;
}

void UOnlineEngineInterfaceEOS::UpdateSessionJoinability(
    UWorld *World,
    FName SessionName,
    bool bPublicSearchable,
    bool bAllowInvites,
    bool bJoinViaPresence,
    bool bJoinViaPresenceFriendsOnly)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        FOnlineSessionSettings *SessionSettings = SessionInt->GetSessionSettings(SessionName);
        if (SessionSettings != nullptr)
        {
            SessionSettings->bShouldAdvertise = bPublicSearchable;
            SessionSettings->bAllowInvites = bAllowInvites;
            SessionSettings->bAllowJoinViaPresence = bJoinViaPresence && !bJoinViaPresenceFriendsOnly;
            SessionSettings->bAllowJoinViaPresenceFriendsOnly = bJoinViaPresenceFriendsOnly;
            SessionInt->UpdateSession(SessionName, *SessionSettings, true);
        }
    }
}

#if defined(UE_5_0_OR_LATER)

void UOnlineEngineInterfaceEOS::RegisterPlayer(
    UWorld *World,
    FName SessionName,
    const FUniqueNetIdWrapper &UniqueId,
    bool bWasInvited)
{
    check(UniqueId.IsValid() && UniqueId.IsV1());
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid() && UniqueId.IsValid())
    {
        SessionInt->RegisterPlayer(SessionName, *UniqueId, bWasInvited);
    }
}

void UOnlineEngineInterfaceEOS::UnregisterPlayer(UWorld *World, FName SessionName, const FUniqueNetIdWrapper &UniqueId)
{
    check(UniqueId.IsValid() && UniqueId.IsV1());
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        SessionInt->UnregisterPlayer(SessionName, *UniqueId);
    }
}

void UOnlineEngineInterfaceEOS::UnregisterPlayers(
    UWorld *World,
    FName SessionName,
    const TArray<FUniqueNetIdWrapper> &Players)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        TArray<FUniqueNetIdRef> PlayerIdsAsRefs;
        for (const FUniqueNetIdWrapper &PlayerId : Players)
        {
            check(PlayerId.IsValid() && PlayerId.IsV1());
            PlayerIdsAsRefs.Emplace(PlayerId.GetV1().ToSharedRef());
        }
        SessionInt->UnregisterPlayers(SessionName, PlayerIdsAsRefs);
    }
}

#else

void UOnlineEngineInterfaceEOS::RegisterPlayer(
    UWorld *World,
    FName SessionName,
    const FUniqueNetId &UniqueId,
    bool bWasInvited)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid() && UniqueId.IsValid())
    {
        SessionInt->RegisterPlayer(SessionName, UniqueId, bWasInvited);
    }
}

void UOnlineEngineInterfaceEOS::UnregisterPlayer(UWorld *World, FName SessionName, const FUniqueNetId &UniqueId)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        SessionInt->UnregisterPlayer(SessionName, UniqueId);
    }
}

void UOnlineEngineInterfaceEOS::UnregisterPlayers(
    UWorld *World,
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &Players)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        SessionInt->UnregisterPlayers(SessionName, Players);
    }
}

#endif

bool UOnlineEngineInterfaceEOS::GetResolvedConnectString(UWorld *World, FName SessionName, FString &URL)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid() && SessionInt->GetResolvedConnectString(SessionName, URL))
    {
        return true;
    }

    return false;
}

TSharedPtr<FVoicePacket> UOnlineEngineInterfaceEOS::GetLocalPacket(UWorld *World, uint8 LocalUserNum)
{
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        TSharedPtr<FVoicePacket> LocalPacket = VoiceInt->GetLocalPacket(LocalUserNum);
        return LocalPacket;
    }

    return nullptr;
}

TSharedPtr<FVoicePacket> UOnlineEngineInterfaceEOS::SerializeRemotePacket(
    UWorld *World,
    const UNetConnection *const RemoteConnection,
    FArchive &Ar)
{
    FName VoiceSubsystemName = VoiceSubsystemNameOverride;
    if (RemoteConnection && RemoteConnection->Driver && RemoteConnection->Driver->GetNetMode() == NM_DedicatedServer)
    {
        FName DedicatedVoiceSubsystemName =
            GetDedicatedServerSubsystemNameForSubsystem(RemoteConnection->GetPlayerOnlinePlatformName());

        if (DedicatedVoiceSubsystemName != NAME_None)
        {
            VoiceSubsystemName = DedicatedVoiceSubsystemName;
        }
    }

    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemName);
    if (VoiceInt.IsValid())
    {
        return VoiceInt->SerializeRemotePacket(Ar);
    }
    return nullptr;
}

void UOnlineEngineInterfaceEOS::StartNetworkedVoice(UWorld *World, uint8 LocalUserNum)
{
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        VoiceInt->StartNetworkedVoice(LocalUserNum);
    }
}

void UOnlineEngineInterfaceEOS::StopNetworkedVoice(UWorld *World, uint8 LocalUserNum)
{
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        VoiceInt->StopNetworkedVoice(LocalUserNum);
    }
}

void UOnlineEngineInterfaceEOS::ClearVoicePackets(UWorld *World)
{
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        VoiceInt->ClearVoicePackets();
    }
}

#if defined(UE_5_0_OR_LATER)

bool UOnlineEngineInterfaceEOS::MuteRemoteTalker(
    UWorld *World,
    uint8 LocalUserNum,
    const FUniqueNetIdWrapper &PlayerId,
    bool bIsSystemWide)
{
    check(PlayerId.IsValid() && PlayerId.IsV1());
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        return VoiceInt->MuteRemoteTalker(LocalUserNum, *PlayerId, bIsSystemWide);
    }
    return false;
}

bool UOnlineEngineInterfaceEOS::UnmuteRemoteTalker(
    UWorld *World,
    uint8 LocalUserNum,
    const FUniqueNetIdWrapper &PlayerId,
    bool bIsSystemWide)
{
    check(PlayerId.IsValid() && PlayerId.IsV1());
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        return VoiceInt->UnmuteRemoteTalker(LocalUserNum, *PlayerId, bIsSystemWide);
    }
    return false;
}

#else

bool UOnlineEngineInterfaceEOS::MuteRemoteTalker(
    UWorld *World,
    uint8 LocalUserNum,
    const FUniqueNetId &PlayerId,
    bool bIsSystemWide)
{
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        return VoiceInt->MuteRemoteTalker(LocalUserNum, PlayerId, bIsSystemWide);
    }
    return false;
}

bool UOnlineEngineInterfaceEOS::UnmuteRemoteTalker(
    UWorld *World,
    uint8 LocalUserNum,
    const FUniqueNetId &PlayerId,
    bool bIsSystemWide)
{
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        return VoiceInt->UnmuteRemoteTalker(LocalUserNum, PlayerId, bIsSystemWide);
    }
    return false;
}

#endif

int32 UOnlineEngineInterfaceEOS::GetNumLocalTalkers(UWorld *World)
{
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World, VoiceSubsystemNameOverride);
    if (VoiceInt.IsValid())
    {
        return VoiceInt->GetNumLocalTalkers();
    }

    return 0;
}

void UOnlineEngineInterfaceEOS::ShowLeaderboardUI(UWorld *World, const FString &CategoryName)
{
    IOnlineExternalUIPtr ExternalUI = Online::GetExternalUIInterface(World);
    if (ExternalUI.IsValid())
    {
        ExternalUI->ShowLeaderboardUI(CategoryName);
    }
}

void UOnlineEngineInterfaceEOS::ShowAchievementsUI(UWorld *World, int32 LocalUserNum)
{
    IOnlineExternalUIPtr ExternalUI = Online::GetExternalUIInterface(World);
    if (ExternalUI.IsValid())
    {
        ExternalUI->ShowAchievementsUI(LocalUserNum);
    }
}

#ifdef OSS_ADDED_SHOW_WEB
void UOnlineEngineInterfaceEOS::ShowWebURL(
    const FString &CurrentURL,
    const UOnlineEngineInterface::FShowWebUrlParams &ShowParams,
    const FOnlineShowWebUrlClosed &CompletionDelegate)
{
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        IOnlineExternalUIPtr ExternalUI = OnlineSub->GetExternalUIInterface();
        if (ExternalUI.IsValid())
        {
            ::FShowWebUrlParams Params;
            Params.bEmbedded = ShowParams.bEmbedded;
            Params.bShowBackground = ShowParams.bShowBackground;
            Params.bShowCloseButton = ShowParams.bShowCloseButton;
            Params.bHideCursor = ShowParams.bHideCursor;
            Params.OffsetX = ShowParams.OffsetX;
            Params.OffsetY = ShowParams.OffsetY;
            Params.SizeX = ShowParams.SizeX;
            Params.SizeY = ShowParams.SizeY;

            ExternalUI->ShowWebURL(CurrentURL, Params, CompletionDelegate);
        }
    }
}

bool UOnlineEngineInterfaceEOS::CloseWebURL()
{
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        IOnlineExternalUIPtr ExternalUI = OnlineSub->GetExternalUIInterface();
        if (ExternalUI.IsValid())
        {
            return ExternalUI->CloseWebURL();
        }
    }

    return false;
}
#endif

void UOnlineEngineInterfaceEOS::BindToExternalUIOpening(const FOnlineExternalUIChanged &Delegate)
{
    IOnlineSubsystemUtils *Utils = Online::GetUtils();
    if (Utils)
    {
        FOnExternalUIChangeDelegate OnExternalUIChangeDelegate;
        OnExternalUIChangeDelegate.BindUObject(this, &ThisClass::OnExternalUIChange, Delegate);
        Utils->SetEngineExternalUIBinding(OnExternalUIChangeDelegate);
    }
}

void UOnlineEngineInterfaceEOS::OnExternalUIChange(
    bool bInIsOpening,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnlineExternalUIChanged Delegate)
{
    Delegate.ExecuteIfBound(bInIsOpening);
}

void UOnlineEngineInterfaceEOS::DumpSessionState(UWorld *World)
{
    IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
    if (SessionInt.IsValid())
    {
        SessionInt->DumpSessionState();
    }
}

void UOnlineEngineInterfaceEOS::DumpPartyState(UWorld *World)
{
    IOnlinePartyPtr PartyInt = Online::GetPartyInterface(World);
    if (PartyInt.IsValid())
    {
        PartyInt->DumpPartyState();
    }
}

void UOnlineEngineInterfaceEOS::DumpVoiceState(UWorld *World)
{
    IOnlineVoicePtr VoiceInt = Online::GetVoiceInterface(World);
    if (VoiceInt.IsValid())
    {
        UE_LOG_ONLINE(Verbose, TEXT("\n%s"), *VoiceInt->GetVoiceDebugState());
    }
}

void UOnlineEngineInterfaceEOS::DumpChatState(UWorld *World)
{
    IOnlineChatPtr ChatInt = Online::GetChatInterface(World);
    if (ChatInt.IsValid())
    {
        ChatInt->DumpChatState();
    }
}

#if WITH_EDITOR
bool UOnlineEngineInterfaceEOS::SupportsOnlinePIE()
{
    if (GIsAutomationTesting)
    {
        return this->GetNumPIELogins() > 0;
    }
    return Online::GetUtils()->SupportsOnlinePIE();
}

void UOnlineEngineInterfaceEOS::SetShouldTryOnlinePIE(bool bShouldTry)
{
    if (GIsAutomationTesting)
    {
        return;
    }
    Online::GetUtils()->SetShouldTryOnlinePIE(bShouldTry);
}

int32 UOnlineEngineInterfaceEOS::GetNumPIELogins()
{
    if (GIsAutomationTesting)
    {
        if (this->AutomationCredentials.IsValid())
        {
            return this->AutomationCredentials->Credentials.Num();
        }
        return 0;
    }
    return Online::GetUtils()->GetNumPIELogins();
}

void UOnlineEngineInterfaceEOS::SetForceDedicated(FName OnlineIdentifier, bool bForce)
{
    if (GIsAutomationTesting)
    {
        return;
    }

#if WITH_EDITOR
    if (bForce)
    {
        // When running a dedicated server with single process, the editor doesn't
        // correctly set RunAsDedicated flag on world context before the online
        // subsystem is initialized. This means we would initialize with the
        // incorrect values as our IsTrueDedicatedServer() check would not be able
        // to detect that we are a dedicated server.
        //
        // Look up the world context based on the online identifier, and force the
        // RunAsDedicated flag to true.
        TArray<FString> Entries;
        if (OnlineIdentifier.ToString().ParseIntoArray(Entries, TEXT(":"), false) >= 2)
        {
            FWorldContext *WorldContext = GEngine->GetWorldContextFromHandle(FName(*Entries[1]));
            if (WorldContext != nullptr)
            {
                // Fix up the flag.
                WorldContext->RunAsDedicated = true;
            }
        }
    }
#endif // #if WITH_EDITOR

    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get(OnlineIdentifier);
    if (OnlineSub)
    {
        OnlineSub->SetForceDedicated(bForce);
    }
}

void UOnlineEngineInterfaceEOS::LoginPIEInstance(
    FName OnlineIdentifier,
    int32 LocalUserNum,
    int32 PIELoginNum,
    FOnPIELoginComplete &CompletionDelegate)
{
    FString ErrorStr;

    if (GIsAutomationTesting)
    {
        if (this->AutomationCredentials.IsValid())
        {
            if (this->AutomationCredentials->Credentials.IsValidIndex(PIELoginNum))
            {
                IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(OnlineIdentifier);
                if (IdentityInt.IsValid())
                {
                    FDelegateHandle DelegateHandle = IdentityInt->AddOnLoginCompleteDelegate_Handle(
                        LocalUserNum,
                        FOnLoginCompleteDelegate::CreateUObject(
                            this,
                            &ThisClass::OnPIELoginComplete,
                            OnlineIdentifier,
                            CompletionDelegate));
                    OnLoginPIECompleteDelegateHandlesForPIEInstances.Add(OnlineIdentifier, DelegateHandle);
                    IdentityInt->Login(LocalUserNum, this->AutomationCredentials->Credentials[PIELoginNum]);
                }
                else
                {
                    ErrorStr = TEXT("No identify interface to login");
                }
            }
            else
            {
                ErrorStr = FString::Printf(
                    TEXT("Invalid credential index for PIE login. Index: %d NumLogins: %d"),
                    PIELoginNum,
                    this->AutomationCredentials->Credentials.Num());
            }
        }
        else
        {
            ErrorStr = TEXT("PIE login not supported");
        }
    }
    else if (SupportsOnlinePIE())
    {
        TArray<FOnlineAccountCredentials> PIELogins;
        Online::GetUtils()->GetPIELogins(PIELogins);
        if (PIELogins.IsValidIndex(PIELoginNum))
        {
            IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(OnlineIdentifier);
            if (IdentityInt.IsValid())
            {
                FDelegateHandle DelegateHandle = IdentityInt->AddOnLoginCompleteDelegate_Handle(
                    LocalUserNum,
                    FOnLoginCompleteDelegate::CreateUObject(
                        this,
                        &ThisClass::OnPIELoginComplete,
                        OnlineIdentifier,
                        CompletionDelegate));
                OnLoginPIECompleteDelegateHandlesForPIEInstances.Add(OnlineIdentifier, DelegateHandle);
                IdentityInt->Login(LocalUserNum, PIELogins[PIELoginNum]);
            }
            else
            {
                ErrorStr = TEXT("No identify interface to login");
            }
        }
        else
        {
            ErrorStr = FString::Printf(
                TEXT("Invalid credential index for PIE login. Index: %d NumLogins: %d"),
                PIELoginNum,
                PIELogins.Num());
        }
    }
    else
    {
        ErrorStr = TEXT("PIE login not supported");
    }

    if (!ErrorStr.IsEmpty())
    {
        CompletionDelegate.ExecuteIfBound(LocalUserNum, false, ErrorStr);
    }
}

void UOnlineEngineInterfaceEOS::OnPIELoginComplete(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FString &Error,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FName OnlineIdentifier,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnlineAutoLoginComplete InCompletionDelegate)
{
    IOnlineIdentityPtr IdentityInt = Online::GetIdentityInterface(OnlineIdentifier);

    // Cleanup the login delegate before calling create below
    FDelegateHandle *DelegateHandle = OnLoginPIECompleteDelegateHandlesForPIEInstances.Find(OnlineIdentifier);
    if (DelegateHandle)
    {
        IdentityInt->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, *DelegateHandle);
        OnLoginPIECompleteDelegateHandlesForPIEInstances.Remove(OnlineIdentifier);
    }

    InCompletionDelegate.ExecuteIfBound(LocalUserNum, bWasSuccessful, Error);
}

#endif