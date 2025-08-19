// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/StringConv.h"
#include "EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include <functional>

EOS_ENABLE_STRICT_WARNINGS

#ifndef EOS_UNLIMITED_MAX_LENGTH
#define EOS_UNLIMITED_MAX_LENGTH (-1)
#endif

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_OpaqueId
{
    static_assert(
        MaxLengthExcludingNullTerminator != EOS_UNLIMITED_MAX_LENGTH,
        "MaxLengthExcludingNullTerminator for EOSString_OpaqueId must not be unlimited");
    static_assert(
        MaxLengthExcludingNullTerminator > EOS_UNLIMITED_MAX_LENGTH,
        "MaxLengthExcludingNullTerminator must be non-negative");

public:
    struct AnsiString
    {
        FString StrTCHAR;
        EOS_EResult Result;
        TStringConversion<TStringConvert<TCHAR, ANSICHAR>> Ptr;

        AnsiString(const TId InId);
    };

    static const TId None;

    static bool IsNone(const TId InId);
    static bool IsValid(const TId InId);
    static bool IsEqual(const TId InA, const TId InB);
    static EOS_EResult ToString(const TId InId, FString &OutString);
    static AnsiString ToAnsiString(const TId InId);
    static EOS_EResult FromString(const FString &InString, TId &OutId);
    static EOS_EResult AllocateToCharBuffer(const TId InId, const char *&OutCharPtr);
    static void FreeFromCharBuffer(const char *&CharPtr);
    static void AllocateToCharList(const TArray<TId> &InIds, uint32_t &OutIdCount, const char **&OutIds);
    static void FreeFromCharListConst(uint32_t IdCount, const char **const Ids);
    static void FreeFromCharList(uint32_t IdCount, const char **&Ids);
    static void AllocateToIdList(const TArray<TId> &InIds, uint32_t &OutIdCount, TId *&OutIds);
    template <typename TObject>
    static void AllocateToIdListViaAccessor(
        const TArray<TObject> &InObjects,
        std::function<TId(const TObject &)> Accessor,
        uint32_t &OutIdCount,
        TId *&OutIds)
    {
        TArray<TId> Ids;
        for (const auto &Object : InObjects)
        {
            Ids.Add(Accessor(Object));
        }
        return AllocateToIdList(Ids, OutIdCount, OutIds);
    }
    static void FreeFromIdListConst(uint32_t IdCount, TId *const Ids);
    static void FreeFromIdList(uint32_t IdCount, TId *&Ids);
};

template <typename TId, int32_t MaxLengthExcludingNullTerminator>
class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_CharBasedId
{
    static_assert(
        MaxLengthExcludingNullTerminator >= EOS_UNLIMITED_MAX_LENGTH,
        "MaxLengthExcludingNullTerminator must be non-negative");

public:
    static const TId None;

    static bool IsNone(const TId InId);
    static bool IsValid(const TId InId);
    static FString ToString(const TId InId);
    static EOS_EResult AllocateToCharBuffer(const TId InId, const char *&OutCharPtr, int32 &OutLen);
    static void FreeFromCharBuffer(const char *&CharPtr);
};

template <typename TToken> class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_OpaqueToken
{
public:
    static const TToken None;

    static bool IsNone(const TToken InToken);
};

template <int32_t MaxLengthExcludingNullTerminator> class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString
{
    static_assert(
        MaxLengthExcludingNullTerminator >= EOS_UNLIMITED_MAX_LENGTH,
        "MaxLengthExcludingNullTerminator must be non-negative");

private:
    static EOS_EResult CopyToExistingBufferPtr(const FString &InString, char *ExistingPtr);

public:
    struct AnsiString
    {
        FString StrTCHAR;
        EOS_EResult Result;
        TStringConversion<TStringConvert<TCHAR, ANSICHAR>> Ptr;

        AnsiString(const FString &InString);
    };

    static bool IsValid(const FString &InString);
    static AnsiString ToAnsiString(const FString &InString);
    static FString FromAnsiString(const char *InAnsiString);
    static void AllocateEmptyCharBuffer(char *&OutCharPtr, int32 &OutLen);
    static EOS_EResult AllocateToCharBuffer(const FString &InString, const char *&OutCharPtr);
    static EOS_EResult AllocateToCharBuffer(const FString &InString, const char *&OutCharPtr, int32 &OutLen);
    template <int32_t N = MaxLengthExcludingNullTerminator + 1>
    static EOS_EResult CopyToExistingBuffer(const FString &InString, char ExistingPtr[N])
    {
        static_assert(
            N == MaxLengthExcludingNullTerminator + 1,
            "N must be set to MaxLengthExcludingNullTerminator + 1");
        static_assert(
            MaxLengthExcludingNullTerminator >= 0,
            "CopyToExistingBuffer can only be used when the string has a fixed size");
        return CopyToExistingBufferPtr(InString, &ExistingPtr[0]);
    }
    static void FreeFromCharBuffer(char *&CharPtr);
    static void FreeFromCharBuffer(const char *&CharPtr);
    static void FreeFromCharBufferConst(const char *const CharPtr);
    static void AllocateToCharList(const TArray<FString> &InIds, uint32_t &OutIdCount, const char **&OutIds);
    template <typename TObject>
    static void AllocateToCharListViaAccessor(
        const TArray<TObject> &InObjects,
        std::function<FString(const TObject &)> Accessor,
        uint32_t &OutIdCount,
        const char **&OutIds)
    {
        TArray<FString> Ids;
        for (const auto &Object : InObjects)
        {
            Ids.Add(Accessor(Object));
        }
        return AllocateToCharList(Ids, OutIdCount, OutIds);
    }
    static void FreeFromCharList(uint32_t IdCount, const char **&Ids);
    static void FreeFromCharListConst(uint32_t IdCount, const char **const Ids);

    /**
     * Calls a synchronous EOS operation that can return a string of unlimited length. This is only valid if
     * MaxLengthExcludingNullTerminator == EOS_UNLIMITED_MAX_LENGTH.
     */
    template <typename TInter, typename TOpts>
    static EOS_EResult FromDynamicLengthApiCall(
        TInter Handle,
        const TOpts *Opts,
        EOS_EResult(__CDECL_ATTR *Operation)(TInter, const TOpts *, char *, uint32_t *),
        FString &OutResult)
    {
        static_assert(
            MaxLengthExcludingNullTerminator == EOS_UNLIMITED_MAX_LENGTH,
            "FromDynamicLengthApiCall can only be used on unlimited length strings");

        uint32_t BufferSize = 2048;
        char *Buffer = (char *)FMemory::MallocZeroed(BufferSize + 1);
        EOS_EResult Result = Operation(Handle, Opts, Buffer, &BufferSize);
        if (Result == EOS_EResult::EOS_LimitExceeded)
        {
            // The buffer isn't big enough to hold the result. Resize, then run the operation again.
            FMemory::Free((void *)Buffer);
            Buffer = (char *)FMemory::MallocZeroed(BufferSize + 1);
            Result = Operation(Handle, Opts, Buffer, &BufferSize);
        }
        OutResult = FromAnsiString(Buffer);
        FMemory::Free(Buffer);
        return Result;
    }
};

template <int32_t MaxLengthExcludingNullTerminator> class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String
{
    static_assert(
        MaxLengthExcludingNullTerminator >= EOS_UNLIMITED_MAX_LENGTH,
        "MaxLengthExcludingNullTerminator must be non-negative");

public:
    struct Utf8String
    {
        FString StrTCHAR;
        EOS_EResult Result;
        FTCHARToUTF8 Ptr;

        const char *GetAsChar()
        {
            return (const char *)Ptr.Get();
        }

        Utf8String(const FString &InString);
    };

    static bool IsValid(const FString &InString);
    static FString FromUtf8String(const char *InPtr);
    static FString FromUtf8String(const UTF8CHAR *InPtr);
    static Utf8String ToUtf8String(const FString &InString);
    static EOS_EResult AllocateToCharBuffer(const FString &InString, const char *&OutCharPtr);
    static void FreeFromCharBuffer(const char *&CharPtr);
};

#ifndef EOS_ATTRIBUTE_MAX_LENGTH
#define EOS_ATTRIBUTE_MAX_LENGTH 1000
#endif
#ifdef EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS
#undef EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS
#endif
#define EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS 32
#ifdef EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS
#undef EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS
#endif
#define EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS 32
#ifndef EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH
#define EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH 32
#endif
#ifndef EOS_TITLESTORAGE_TAG_MAX_LENGTH
#define EOS_TITLESTORAGE_TAG_MAX_LENGTH 15
#endif
#ifndef EOS_ANDROID_DIRECTORY_MAX_LENGTH
#define EOS_ANDROID_DIRECTORY_MAX_LENGTH 2048
#endif

extern char const EOS_ProductUserId_TypeName[];
extern char const EOS_EpicAccountId_TypeName[];

typedef EOSString_OpaqueId<
    EOS_ProductUserId_TypeName,
    EOS_ProductUserId,
    EOS_PRODUCTUSERID_MAX_LENGTH,
    EOS_ProductUserId_IsValid,
    EOS_ProductUserId_ToString,
    EOS_ProductUserId_FromString>
    EOSString_ProductUserId;
typedef EOSString_OpaqueId<
    EOS_EpicAccountId_TypeName,
    EOS_EpicAccountId,
    EOS_EPICACCOUNTID_MAX_LENGTH,
    EOS_EpicAccountId_IsValid,
    EOS_EpicAccountId_ToString,
    EOS_EpicAccountId_FromString>
    EOSString_EpicAccountId;

typedef EOSString_OpaqueToken<EOS_ContinuanceToken> EOSString_ContinuanceToken;

typedef EOSString_CharBasedId<EOS_LobbyId, EOS_UNLIMITED_MAX_LENGTH> EOSString_LobbyId;

typedef EOSString_AnsiString<EOS_UNLIMITED_MAX_LENGTH> EOSString_Achievements_AchievementId;
typedef EOSString_AnsiString<EOS_UNLIMITED_MAX_LENGTH> EOSString_AnsiUnlimited;
typedef EOSString_AnsiString<EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS> EOSString_AnsiStringForTests;
typedef EOSString_AnsiString<EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH> EOSString_Connect_ExternalAccountId;
typedef EOSString_AnsiString<EOS_LOBBY_INVITEID_MAX_LENGTH> EOSString_Lobby_InviteId;
typedef EOSString_AnsiString<EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH> EOSString_LobbyModification_AttributeKey;
typedef EOSString_AnsiString<EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH> EOSString_PlayerDataStorage_MD5Hash;
typedef EOSString_AnsiString<EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH> EOSString_PresenceModification_JoinInfo;
typedef EOSString_AnsiString<EOS_SESSIONS_INVITEID_MAX_LENGTH> EOSString_Session_InviteId;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
typedef EOSString_AnsiString<EOS_TITLESTORAGE_TAG_MAX_LENGTH> EOSString_TitleStorage_Tag;
#endif
typedef EOSString_AnsiString<EOS_ECOM_TRANSACTIONID_MAXIMUM_LENGTH> EOSString_Ecom_TransactionId;
typedef EOSString_AnsiString<EOS_UNLIMITED_MAX_LENGTH> EOSString_Ecom_CatalogOfferId;
typedef EOSString_AnsiString<EOS_UNLIMITED_MAX_LENGTH> EOSString_Ecom_CatalogNamespace;
typedef EOSString_AnsiString<EOS_P2P_SOCKET_NAME_MAX_LENGTH> EOSString_P2P_SocketName;
typedef EOSString_AnsiString<EOS_UNLIMITED_MAX_LENGTH> EOSString_Stats_StatName;
typedef EOSString_AnsiString<EOS_UNLIMITED_MAX_LENGTH> EOSString_Leaderboards_LeaderboardId;
typedef EOSString_AnsiString<EOS_PRESENCE_DATA_MAX_KEY_LENGTH> EOSString_Presence_DataRecord_Key;
typedef EOSString_AnsiString<EOS_PRESENCE_DATA_MAX_VALUE_LENGTH> EOSString_Presence_DataRecord_Value;
typedef EOSString_AnsiString<EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH> EOSString_SessionModification_AttributeKey;
typedef EOSString_AnsiString<EOS_ATTRIBUTE_MAX_LENGTH> EOSString_SessionModification_SessionName;
typedef EOSString_AnsiString<EOS_ATTRIBUTE_MAX_LENGTH> EOSString_SessionModification_HostAddress;
typedef EOSString_AnsiString<EOS_ATTRIBUTE_MAX_LENGTH> EOSString_SessionModification_BucketId;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
typedef EOSString_AnsiString<EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH>
    EOSString_SessionModification_SessionId;
#else
typedef EOSString_AnsiString<EOS_ATTRIBUTE_MAX_LENGTH> EOSString_SessionModification_SessionId;
#endif

typedef EOSString_Utf8String<EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS> EOSString_Utf8StringForTests;
typedef EOSString_Utf8String<EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH>
    EOSString_Connect_UserLoginInfo_DisplayName;
typedef EOSString_Utf8String<EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH>
    EOSString_Connect_CreateDeviceId_DeviceModel;
typedef EOSString_Utf8String<EOS_ATTRIBUTE_MAX_LENGTH> EOSString_LobbyModification_AttributeStringValue;
typedef EOSString_Utf8String<EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES> EOSString_PlayerDataStorage_Filename;
typedef EOSString_Utf8String<EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH> EOSString_UserInfo_DisplayName;
typedef EOSString_Utf8String<EOS_ATTRIBUTE_MAX_LENGTH> EOSString_SessionModification_AttributeStringValue;
typedef EOSString_Utf8String<EOS_ATTRIBUTE_MAX_LENGTH> EOSString_QueryUserInfoByDisplayName_DisplayName;
typedef EOSString_Utf8String<EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH> EOSString_Presence_RichTextValue;
typedef EOSString_Utf8String<EOS_ANDROID_DIRECTORY_MAX_LENGTH> EOSString_Android_InitializeOptions_Directory;
typedef EOSString_Utf8String<EOS_UNLIMITED_MAX_LENGTH> EOSString_AntiCheat_ActionReasonDetailsString;
typedef EOSString_Utf8String<EOS_UNLIMITED_MAX_LENGTH> EOSString_Utf8Unlimited;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
typedef EOSString_Utf8String<EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES> EOSString_TitleStorage_Filename;
typedef EOSString_AnsiString<EOS_TITLESTORAGE_TAG_MAX_LENGTH> EOSString_TitleStorage_Tag;
#endif

EOS_DISABLE_STRICT_WARNINGS
