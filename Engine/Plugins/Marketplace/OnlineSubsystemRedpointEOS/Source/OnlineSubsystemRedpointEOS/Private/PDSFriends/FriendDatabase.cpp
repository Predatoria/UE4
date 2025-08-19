// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/FriendDatabase.h"

#include "Misc/SecureHash.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "Serialization/MemoryArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

EOS_ENABLE_STRICT_WARNINGS

FFriendDatabase::FFriendDatabase(
    const TSharedRef<const FUniqueNetIdEOS> &InLocalUserId,
    const TSharedRef<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloudImpl)
    : LocalUserId(InLocalUserId)
    , UserCloudImpl(InUserCloudImpl)
    , bLoading(false)
    , bLoaded(false)
    , bDirty(false)
    , bFlushing(false)
    , EnumerateHandle()
    , LoadHandle()
    , PendingCallbacksForNextFlushBatch()
    , NextChainedFlushOperation()
    , Friends()
    , DelegatedSubsystemCachedFriends()
    , RecentPlayers()
    , BlockedUsers()
    , PendingFriendRequests()
    , FriendAliases()
    , ExpectedHeader(TArray<uint8>{'F', 'R', 'I', 'E', 'N', 'D', 'D', 'B', '1'})
{
}

void FFriendDatabase::OnEnumerateFilesComplete(
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FFriendDatabaseOperationComplete OnComplete)
{
    if (UserId != *this->LocalUserId)
    {
        // This event isn't for us.
        return;
    }

    checkf(this->bLoading, TEXT("OnEnumerateFilesComplete fired, but friend database wasn't loading data!"));

    if (auto UserCloud = this->UserCloudImpl.Pin())
    {
        UserCloud->ClearOnEnumerateUserFilesCompleteDelegate_Handle(this->EnumerateHandle);
        this->EnumerateHandle.Reset();

        if (!bWasSuccessful)
        {
            this->bLoaded = false;
            this->bLoading = false;
            OnComplete.ExecuteIfBound(false);
            return;
        }

        TArray<FCloudFileHeader> UserFiles;
        UserCloud->GetUserFileList(*this->LocalUserId, UserFiles);

        bool bFoundDatabase = false;
        for (const auto &File : UserFiles)
        {
            if (File.FileName == TEXT("__EOS__/PDSFriends.db"))
            {
                bFoundDatabase = true;
                break;
            }
        }

        if (!bFoundDatabase)
        {
            // User doesn't have a friends database yet.
            this->bLoaded = true;
            this->bLoading = false;
            OnComplete.ExecuteIfBound(true);
            return;
        }

        this->LoadHandle = UserCloud->AddOnReadUserFileCompleteDelegate_Handle(
            FOnReadUserFileCompleteDelegate::CreateSP(this, &FFriendDatabase::OnFileReadComplete, OnComplete));
        if (!UserCloud->ReadUserFile(*this->LocalUserId, TEXT("__EOS__/PDSFriends.db")))
        {
            UserCloud->ClearOnReadUserFileCompleteDelegate_Handle(this->LoadHandle);
            OnComplete.ExecuteIfBound(false);
        }
    }
    else
    {
        this->bLoaded = true;
        this->bLoading = false;
        OnComplete.ExecuteIfBound(true);
        return;
    }
}

template <typename T>
void MapArchive(FArchive &Ar, TUserIdMap<T> &Map, std::function<TSharedPtr<const FUniqueNetId>(const T &)> IdMapper)
{
    if (Ar.IsLoading())
    {
        int32 Count;
        Ar << Count;
        for (int32 i = 0; i < Count; i++)
        {
            T Val;
            Ar << Val;
            TSharedPtr<const FUniqueNetId> KeyId = IdMapper(Val);
            if (KeyId != nullptr)
            {
                Map.Add(*KeyId, Val);
            }
            else
            {
                UE_LOG(
                    LogEOSFriends,
                    Warning,
                    TEXT("Invalid user ID on archive entry %d; it will be ignored during deserialization."),
                    i);
            }
        }
    }
    else
    {
        int32 Count = Map.Num();
        Ar << Count;
        for (const auto &KV : Map)
        {
            T Val = KV.Value;
            Ar << Val;
        }
    }
}

void FFriendDatabase::Archive(FArchive &Ar)
{
    int8 Version;
    if (Ar.IsSaving())
    {
        Version = 2;
    }
    Ar << Version;
    MapArchive<FSerializedFriend>(Ar, this->Friends, [](const FSerializedFriend &F) {
        return FUniqueNetIdEOS::ParseFromString(F.ProductUserId);
    });
    MapArchive<FSerializedRecentPlayer>(Ar, this->RecentPlayers, [](const FSerializedRecentPlayer &F) {
        return FUniqueNetIdEOS::ParseFromString(F.ProductUserId);
    });
    MapArchive<FSerializedBlockedUser>(Ar, this->BlockedUsers, [](const FSerializedBlockedUser &F) {
        return FUniqueNetIdEOS::ParseFromString(F.ProductUserId);
    });
    MapArchive<FSerializedPendingFriendRequest>(
        Ar,
        this->PendingFriendRequests,
        [](const FSerializedPendingFriendRequest &F) {
            return FUniqueNetIdEOS::ParseFromString(F.ProductUserId);
        });
    Ar << this->DelegatedSubsystemCachedFriends;
    if (Version >= 2)
    {
        if (Ar.IsSaving())
        {
            int32 AliasCount = this->FriendAliases.Num();
            Ar << AliasCount;
            for (const auto &KV : this->FriendAliases)
            {
                FString Key = KV.Key;
                FString Value = KV.Value;
                Ar << Key;
                Ar << Value;
            }
        }
        else
        {
            this->FriendAliases.Empty();
            int32 AliasCount;
            Ar << AliasCount;
            for (int32 i = 0; i < AliasCount; i++)
            {
                FString Key, Value;
                Ar << Key;
                Ar << Value;
                this->FriendAliases.Add(Key, Value);
            }
        }
    }
}

void FFriendDatabase::OnFileReadComplete(
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FString &FileName,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FFriendDatabaseOperationComplete OnComplete)
{
    if (UserId != *this->LocalUserId || FileName != TEXT("__EOS__/PDSFriends.db"))
    {
        // This event isn't for us.
        return;
    }

    checkf(this->bLoading, TEXT("OnFileReadComplete fired, but friend database wasn't loading data!"));

    if (auto UserCloud = this->UserCloudImpl.Pin())
    {
        UserCloud->ClearOnReadUserFileCompleteDelegate_Handle(this->LoadHandle);
        this->LoadHandle.Reset();

        if (!bWasSuccessful)
        {
            this->bLoaded = false;
            this->bLoading = false;
            OnComplete.ExecuteIfBound(false);
            return;
        }

        TArray<uint8> DbContents;
        if (!UserCloud->GetFileContents(UserId, FileName, DbContents))
        {
            this->bLoaded = false;
            this->bLoading = false;
            OnComplete.ExecuteIfBound(false);
            return;
        }

        // See if we have an integrity hash at the start of the file.
        int32 HeaderSize = this->ExpectedHeader.Num() + FSHA1::DigestSize;
        if (DbContents.Num() < HeaderSize)
        {
            UE_LOG(
                LogEOSFriends,
                Error,
                TEXT("Cross-platform friends database is corrupt (not large enough) - discarding!"));
            this->bLoaded = true;
            this->bLoading = false;
            OnComplete.ExecuteIfBound(true);
            return;
        }
        bool bHasMagicSignature = true;
        for (int i = 0; i < this->ExpectedHeader.Num(); i++)
        {
            if (DbContents[i] != this->ExpectedHeader[i])
            {
                bHasMagicSignature = false;
                break;
            }
        }

        if (!bHasMagicSignature)
        {
            UE_LOG(
                LogEOSFriends,
                Error,
                TEXT("Cross-platform friends database is corrupt (missing signature) - discarding!"));
            this->bLoaded = true;
            this->bLoading = false;
            OnComplete.ExecuteIfBound(true);
            return;
        }

        FSHA1 Sha;
        FSHAHash Hash;
        Sha.HashBuffer(DbContents.GetData() + HeaderSize, DbContents.Num() - HeaderSize, Hash.Hash);

        // Compare hash.
        for (int i = 0; i < FSHA1::DigestSize; i++)
        {
            if (Hash.Hash[i] != DbContents[this->ExpectedHeader.Num() + i])
            {
                UE_LOG(
                    LogEOSFriends,
                    Error,
                    TEXT("Cross-platform friends database is corrupt (mismatched hash) - discarding!"));
                this->bLoaded = true;
                this->bLoading = false;
                OnComplete.ExecuteIfBound(true);
                return;
            }
        }

        // Skip over the header content before reading.
        DbContents = TArray<uint8>(DbContents.GetData() + HeaderSize, DbContents.Num() - HeaderSize);

        UE_LOG(LogEOSFriends, Verbose, TEXT("Cross-platform friends database passed hash check."));

        FMemoryReader Reader(DbContents, true);
        this->Archive(Reader);
        this->bLoaded = true;
        this->bLoading = false;
        OnComplete.ExecuteIfBound(true);
        return;
    }
    else
    {
        this->bLoaded = false;
        this->bLoading = false;
        OnComplete.ExecuteIfBound(false);
        return;
    }
}

void FFriendDatabase::WaitUntilLoaded(const FFriendDatabaseOperationComplete &OnComplete)
{
    if (this->bLoaded)
    {
        OnComplete.ExecuteIfBound(true);
        return;
    }

    if (this->bLoading)
    {
        // Someone else is loading. Wait for them to be done.
        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateLambda([WeakThis = GetWeakThis(this), OnComplete](float DeltaSeconds) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    This->WaitUntilLoaded(OnComplete);
                }
                return false;
            }),
            500);
        return;
    }

    if (auto UserCloud = this->UserCloudImpl.Pin())
    {
        this->bLoaded = false;
        this->bLoading = true;

        this->EnumerateHandle =
            UserCloud->AddOnEnumerateUserFilesCompleteDelegate_Handle(FOnEnumerateUserFilesCompleteDelegate::CreateSP(
                this,
                &FFriendDatabase::OnEnumerateFilesComplete,
                OnComplete));
        UserCloud->EnumerateUserFiles(*this->LocalUserId);
    }
}

void FFriendDatabase::OnFileWriteComplete(
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FString &FileName,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TArray<FFriendDatabaseOperationComplete> OnCompleteCallbacks)
{
    if (UserId != *this->LocalUserId || FileName != TEXT("__EOS__/PDSFriends.db"))
    {
        // This event isn't for us.
        return;
    }

    checkf(this->bFlushing, TEXT("OnFileWriteComplete fired, but friend database wasn't flushing data!"));

    if (auto UserCloud = this->UserCloudImpl.Pin())
    {
        UserCloud->ClearOnWriteUserFileCompleteDelegate_Handle(this->SaveHandle);
    }
    this->SaveHandle.Reset();

    // Write is complete.
    this->bFlushing = false;
    for (const auto &Cb : OnCompleteCallbacks)
    {
        Cb.ExecuteIfBound(bWasSuccessful);
    }
    if (this->NextChainedFlushOperation.IsBound())
    {
        auto ChainCopy = this->NextChainedFlushOperation;
        this->NextChainedFlushOperation.Unbind();
        ChainCopy.ExecuteIfBound(bWasSuccessful);
    }
}

void FFriendDatabase::InternalFlush(const TArray<FFriendDatabaseOperationComplete> &OnCompleteCallbacks)
{
    // Serialize the database.
    TArray<uint8> RawDbContents;
    FMemoryWriter Writer(RawDbContents, true);
    this->Archive(Writer);
    Writer.Flush();

    // Hash the database.
    FSHA1 Sha;
    FSHAHash Hash;
    Sha.HashBuffer(RawDbContents.GetData(), RawDbContents.Num(), Hash.Hash);

    // Construct the final file data with the hash + data.
    TArray<uint8> DbContents;
    for (int i = 0; i < this->ExpectedHeader.Num(); i++)
    {
        DbContents.Add(this->ExpectedHeader[i]);
    }
    for (int i = 0; i < FSHA1::DigestSize; i++)
    {
        DbContents.Add(Hash.Hash[i]);
    }
    for (int i = 0; i < RawDbContents.Num(); i++)
    {
        DbContents.Add(RawDbContents[i]);
    }

    if (auto UserCloud = this->UserCloudImpl.Pin())
    {
        this->SaveHandle =
            UserCloud->AddOnWriteUserFileCompleteDelegate_Handle(FOnWriteUserFileCompleteDelegate::CreateSP(
                this,
                &FFriendDatabase::OnFileWriteComplete,
                OnCompleteCallbacks));
        if (!UserCloud->WriteUserFile(*this->LocalUserId, TEXT("__EOS__/PDSFriends.db"), DbContents, false))
        {
            UserCloud->ClearOnWriteUserFileCompleteDelegate_Handle(this->SaveHandle);
            this->bFlushing = false;
            for (const auto &Cb : OnCompleteCallbacks)
            {
                Cb.ExecuteIfBound(false);
            }
            if (this->NextChainedFlushOperation.IsBound())
            {
                auto ChainCopy = this->NextChainedFlushOperation;
                this->NextChainedFlushOperation.Unbind();
                ChainCopy.ExecuteIfBound(false);
            }
            return;
        }
    }
}

void FFriendDatabase::DirtyAndFlush(const FFriendDatabaseOperationComplete &OnComplete)
{
    if (!this->bLoaded)
    {
        // If we haven't loaded, we can't possibly have changes to flush.
        return;
    }

    if (this->bDirty)
    {
        // There's another pending flush queued up. Just schedule our callback for when that
        // queued flush completes.
        this->PendingCallbacksForNextFlushBatch.Add(OnComplete);
        return;
    }

    if (this->bFlushing)
    {
        // Someone else is already flushing, but we might be writing stale data when we have another
        // flush request come in. Ask them to tell us when they finish their flush operation, so we can start.
        this->NextChainedFlushOperation = FFriendDatabaseOperationComplete::CreateLambda(
            [WeakThis = GetWeakThis(this), OnComplete](bool bWasSuccessful) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    FUTicker::GetCoreTicker().AddTicker(
                        FTickerDelegate::CreateLambda([WeakThis = GetWeakThis(This), OnComplete](float DeltaSeconds) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                This->bDirty = false;
                                This->bFlushing = true;
                                TArray<FFriendDatabaseOperationComplete> Callbacks;
                                Callbacks.Add(OnComplete);
                                for (const auto &BatchedCallback : This->PendingCallbacksForNextFlushBatch)
                                {
                                    Callbacks.Add(BatchedCallback);
                                }
                                This->PendingCallbacksForNextFlushBatch.Empty();
                                This->InternalFlush(Callbacks);
                            }
                            return false;
                        }),
                        0.0f);
                }
            });
        this->bDirty = true;
        return;
    }

    // No-one is flushing. Immediately mark as no longer dirty and flushing, and start the flush.
    this->bDirty = false;
    this->bFlushing = true;
    TArray<FFriendDatabaseOperationComplete> Callbacks;
    Callbacks.Add(OnComplete);
    this->InternalFlush(Callbacks);
}

EOS_DISABLE_STRICT_WARNINGS