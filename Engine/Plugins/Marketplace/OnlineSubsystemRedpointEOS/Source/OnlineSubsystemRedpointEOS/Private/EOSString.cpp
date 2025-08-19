// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include <typeinfo>

EOS_ENABLE_STRICT_WARNINGS

char const EOS_ProductUserId_TypeName[] = "EOS_ProductUserId";
char const EOS_EpicAccountId_TypeName[] = "EOS_EpicAccountId";

// ===== opaque pointer based tokens =====

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
const TId EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::None = nullptr;

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
bool EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::IsNone(const TId InId)
{
    return InId == nullptr;
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
bool EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::IsValid(const TId InId)
{
    if (InId == nullptr)
    {
        return false;
    }

    return Func_IsValid(InId) == EOS_TRUE;
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
bool EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::IsEqual(const TId InA, const TId InB)
{
    FString StrA, StrB;
    EOS_EResult ResultA, ResultB;
    ResultA = ToString(InA, StrA);
    ResultB = ToString(InB, StrB);
    if (ResultA != EOS_EResult::EOS_Success || ResultB != EOS_EResult::EOS_Success)
    {
        return ResultA == ResultB;
    }

    return StrA == StrB;
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
EOS_EResult
EOSString_OpaqueId<TIdTypeName, TId, MaxLengthExcludingNullTerminator, Func_IsValid, Func_ToString, Func_FromString>::
    ToString(const TId InId, FString &OutString)
{
    char Buffer[MaxLengthExcludingNullTerminator + 1];
    FMemory::Memzero(Buffer, MaxLengthExcludingNullTerminator + 1);
    int32_t BufferLen = MaxLengthExcludingNullTerminator + 1;
    EOS_EResult Result = Func_ToString(InId, Buffer, &BufferLen);
    if (Result != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EOSString_OpaqueId::ToString failed when converting '%s', got result: %s"),
            ANSI_TO_TCHAR(TIdTypeName),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        OutString = TEXT("");
        return Result;
    }

    OutString = ANSI_TO_TCHAR(Buffer);
    return Result;
}

// StrTCHAR gets indirectly initialized via the ::ToString call.
// NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
EOSString_OpaqueId<TIdTypeName, TId, MaxLengthExcludingNullTerminator, Func_IsValid, Func_ToString, Func_FromString>::
    AnsiString::AnsiString(const TId InId)
    : Result(EOSString_OpaqueId<
             TIdTypeName,
             TId,
             MaxLengthExcludingNullTerminator,
             Func_IsValid,
             Func_ToString,
             Func_FromString>::ToString(InId, this->StrTCHAR))
    , Ptr(StringCast<ANSICHAR>(*this->StrTCHAR))
{
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
typename EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::AnsiString
EOSString_OpaqueId<TIdTypeName, TId, MaxLengthExcludingNullTerminator, Func_IsValid, Func_ToString, Func_FromString>::
    ToAnsiString(const TId InId)
{
    return AnsiString(InId);
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
EOS_EResult
EOSString_OpaqueId<TIdTypeName, TId, MaxLengthExcludingNullTerminator, Func_IsValid, Func_ToString, Func_FromString>::
    FromString(const FString &InString, TId &OutId)
{
    OutId = Func_FromString(TCHAR_TO_ANSI(*InString));
    if (!Func_IsValid(OutId))
    {
        OutId = nullptr;
        return EOS_EResult::EOS_InvalidParameters;
    }

    return EOS_EResult::EOS_Success;
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
EOS_EResult
EOSString_OpaqueId<TIdTypeName, TId, MaxLengthExcludingNullTerminator, Func_IsValid, Func_ToString, Func_FromString>::
    AllocateToCharBuffer(const TId InId, const char *&OutCharPtr)
{
    check(OutCharPtr == nullptr);
    OutCharPtr = (char *)FMemory::MallocZeroed(MaxLengthExcludingNullTerminator + 1);
    int32_t BufferLen = MaxLengthExcludingNullTerminator + 1;
    EOS_EResult Result = Func_ToString(InId, (char *&)OutCharPtr, &BufferLen);
    if (Result != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EOSString_OpaqueId::AllocateToCharBuffer failed when converting '%s', got result: %s"),
            ANSI_TO_TCHAR(TIdTypeName),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        FMemory::Free((void *)OutCharPtr);
        OutCharPtr = nullptr;
        return Result;
    }

    return Result;
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
void EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::FreeFromCharBuffer(const char *&CharPtr)
{
    if (CharPtr != nullptr)
    {
        FMemory::Free((void *)CharPtr);
        CharPtr = nullptr;
    }
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
void EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::AllocateToCharList(const TArray<TId> &InIds, uint32_t &OutIdCount, const char **&OutIds)
{
    if (InIds.Num() == 0)
    {
        OutIdCount = 0;
        OutIds = nullptr;
        return;
    }

    OutIdCount = InIds.Num();
    OutIds = (const char **)FMemory::MallocZeroed(sizeof(const char *) * OutIdCount, 0);
    for (uint32_t i = 0; i < OutIdCount; i++)
    {
        if (AllocateToCharBuffer(InIds[i], OutIds[i]) != EOS_EResult::EOS_Success)
        {
            continue;
        }
    }
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
void EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::FreeFromCharListConst(uint32_t IdCount, const char **const Ids)
{
    if (IdCount == 0 && Ids == nullptr)
    {
        return;
    }

    for (uint32_t i = 0; i < IdCount; i++)
    {
        FreeFromCharBuffer(Ids[i]);
    }
    FMemory::Free((void *)Ids);
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
void EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::FreeFromCharList(uint32_t IdCount, const char **&Ids)
{
    if (IdCount == 0 && Ids == nullptr)
    {
        return;
    }

    for (uint32_t i = 0; i < IdCount; i++)
    {
        FreeFromCharBuffer(Ids[i]);
    }
    FMemory::Free((void *)Ids);
    Ids = nullptr;
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
void EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::AllocateToIdList(const TArray<TId> &InIds, uint32_t &OutIdCount, TId *&OutIds)
{
    OutIdCount = InIds.Num();
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    OutIds = (TId *)FMemory::MallocZeroed(sizeof(TId) * OutIdCount, 0);
    for (uint32_t i = 0; i < OutIdCount; i++)
    {
        OutIds[i] = InIds[i];
    }
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
void EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::FreeFromIdListConst(uint32_t IdCount, TId *const Ids)
{
    FMemory::Free((void *)Ids);
}

template <
    char const *TIdTypeName,
    typename TId,
    int32_t MaxLengthExcludingNullTerminator,
    EOS_Bool (*Func_IsValid)(TId InId),
    EOS_EResult (*Func_ToString)(TId InId, char *OutBuffer, int32_t *InOutBufferLength),
    TId (*Func_FromString)(const char *InString)>
void EOSString_OpaqueId<
    TIdTypeName,
    TId,
    MaxLengthExcludingNullTerminator,
    Func_IsValid,
    Func_ToString,
    Func_FromString>::FreeFromIdList(uint32_t IdCount, TId *&Ids)
{
    FMemory::Free((void *)Ids);
    Ids = nullptr;
}

// ===== const char* based Ids =====

template <typename TId, int32_t MaxLengthExcludingNullTerminator>
const TId EOSString_CharBasedId<TId, MaxLengthExcludingNullTerminator>::None = nullptr;

template <typename TId, int32_t MaxLengthExcludingNullTerminator>
bool EOSString_CharBasedId<TId, MaxLengthExcludingNullTerminator>::IsNone(const TId InId)
{
    return InId == nullptr;
}

template <typename TId, int32_t MaxLengthExcludingNullTerminator>
bool EOSString_CharBasedId<TId, MaxLengthExcludingNullTerminator>::IsValid(const TId InId)
{
    if (InId == nullptr)
    {
        return false;
    }

    FString IdAsString = ANSI_TO_TCHAR(InId);
    if (IdAsString.Len() == 0)
    {
        return false;
    }
    if (MaxLengthExcludingNullTerminator != EOS_UNLIMITED_MAX_LENGTH &&
        IdAsString.Len() > MaxLengthExcludingNullTerminator)
    {
        return false;
    }

    return true;
}

template <typename TId, int32_t MaxLengthExcludingNullTerminator>
FString EOSString_CharBasedId<TId, MaxLengthExcludingNullTerminator>::ToString(const TId InId)
{
    return ANSI_TO_TCHAR(InId);
}

template <typename TId, int32_t MaxLengthExcludingNullTerminator>
EOS_EResult EOSString_CharBasedId<TId, MaxLengthExcludingNullTerminator>::AllocateToCharBuffer(
    const TId InId,
    const char *&OutCharPtr,
    int32 &OutLen)
{
    if (OutCharPtr != nullptr)
    {
        return EOS_EResult::EOS_InvalidParameters;
    }
    if (!IsValid(InId))
    {
        return EOS_EResult::EOS_InvalidParameters;
    }

    const int ScanLen =
        MaxLengthExcludingNullTerminator == EOS_UNLIMITED_MAX_LENGTH ? 100000 : MaxLengthExcludingNullTerminator;
    int IdLen = strnlen(InId, ScanLen);
    OutCharPtr = (char *)FMemory::MallocZeroed(IdLen + 1);
    OutLen = IdLen + 1;
    FMemory::Memcpy((void *)OutCharPtr, (void *)InId, IdLen);
    return EOS_EResult::EOS_Success;
}

template <typename TId, int32_t MaxLengthExcludingNullTerminator>
void EOSString_CharBasedId<TId, MaxLengthExcludingNullTerminator>::FreeFromCharBuffer(const char *&CharPtr)
{
    if (CharPtr != nullptr)
    {
        FMemory::Free((void *)CharPtr);
        CharPtr = nullptr;
    }
}

// ===== const char* based tokens =====

template <typename TToken> const TToken EOSString_OpaqueToken<TToken>::None = nullptr;

template <typename TToken> bool EOSString_OpaqueToken<TToken>::IsNone(const TToken InToken)
{
    return InToken == nullptr;
}

// ===== ANSI data values =====

template <int32_t MaxLengthExcludingNullTerminator>
bool EOSString_AnsiString<MaxLengthExcludingNullTerminator>::IsValid(const FString &InString)
{
    return InString.Len() > 0 && (MaxLengthExcludingNullTerminator == EOS_UNLIMITED_MAX_LENGTH ||
                                  InString.Len() <= MaxLengthExcludingNullTerminator);
}

template <int32_t MaxLengthExcludingNullTerminator>
EOSString_AnsiString<MaxLengthExcludingNullTerminator>::AnsiString::AnsiString(const FString &InString)
    : StrTCHAR(InString)
    , Result(
          (MaxLengthExcludingNullTerminator == EOS_UNLIMITED_MAX_LENGTH ||
           this->StrTCHAR.Len() <= MaxLengthExcludingNullTerminator)
              ? EOS_EResult::EOS_Success
              : EOS_EResult::EOS_LimitExceeded)
    , Ptr(StringCast<ANSICHAR>(*this->StrTCHAR))
{
}

template <int32_t MaxLengthExcludingNullTerminator>
FString EOSString_AnsiString<MaxLengthExcludingNullTerminator>::FromAnsiString(const char *InAnsiString)
{
    return ANSI_TO_TCHAR(InAnsiString);
}

template <int32_t MaxLengthExcludingNullTerminator>
typename EOSString_AnsiString<MaxLengthExcludingNullTerminator>::AnsiString EOSString_AnsiString<
    MaxLengthExcludingNullTerminator>::ToAnsiString(const FString &InString)
{
    return AnsiString(InString);
}

template <int32_t MaxLengthExcludingNullTerminator>
void EOSString_AnsiString<MaxLengthExcludingNullTerminator>::AllocateEmptyCharBuffer(char *&OutCharPtr, int32 &OutLen)
{
    check(OutCharPtr == nullptr);
    check(MaxLengthExcludingNullTerminator > 0);

    OutCharPtr = (char *)FMemory::MallocZeroed(MaxLengthExcludingNullTerminator + 1);
    OutLen = MaxLengthExcludingNullTerminator + 1;
}

template <int32_t MaxLengthExcludingNullTerminator>
EOS_EResult EOSString_AnsiString<MaxLengthExcludingNullTerminator>::AllocateToCharBuffer(
    const FString &InString,
    const char *&OutCharPtr)
{
    int32 Len;
    return AllocateToCharBuffer(InString, OutCharPtr, Len);
}

template <int32_t MaxLengthExcludingNullTerminator>
EOS_EResult EOSString_AnsiString<MaxLengthExcludingNullTerminator>::AllocateToCharBuffer(
    const FString &InString,
    const char *&OutCharPtr,
    int32 &OutLen)
{
    if (OutCharPtr != nullptr)
    {
        return EOS_EResult::EOS_InvalidParameters;
    }

    auto StrBuf = StringCast<ANSICHAR>(*InString);
    if (MaxLengthExcludingNullTerminator != EOS_UNLIMITED_MAX_LENGTH &&
        StrBuf.Length() > MaxLengthExcludingNullTerminator)
    {
        OutLen = 0;
        return EOS_EResult::EOS_LimitExceeded;
    }
    OutCharPtr = (char *)FMemory::MallocZeroed(StrBuf.Length() + 1);
    FMemory::Memcpy((void *)OutCharPtr, (const void *)StrBuf.Get(), StrBuf.Length());
    OutLen = StrBuf.Length() + 1;
    return EOS_EResult::EOS_Success;
}

template <int32_t MaxLengthExcludingNullTerminator>
EOS_EResult EOSString_AnsiString<MaxLengthExcludingNullTerminator>::CopyToExistingBufferPtr(
    const FString &InString,
    char *ExistingPtr)
{
    if (MaxLengthExcludingNullTerminator == EOS_UNLIMITED_MAX_LENGTH)
    {
        UE_LOG(LogEOS, Error, TEXT("CopyToExistingBuffer called on string type with unlimited length."));
        return EOS_EResult::EOS_LimitExceeded;
    }

    auto StrBuf = StringCast<ANSICHAR>(*InString);
    if (InString.Len() > MaxLengthExcludingNullTerminator)
    {
        return EOS_EResult::EOS_LimitExceeded;
    }
    FMemory::Memzero(ExistingPtr, MaxLengthExcludingNullTerminator + 1);
    FMemory::Memcpy(ExistingPtr, StrBuf.Get(), StrBuf.Length());
    return EOS_EResult::EOS_Success;
}

template <int32_t MaxLengthExcludingNullTerminator>
void EOSString_AnsiString<MaxLengthExcludingNullTerminator>::FreeFromCharBuffer(char *&CharPtr)
{
    if (CharPtr != nullptr)
    {
        FMemory::Free((void *)CharPtr);
        CharPtr = nullptr;
    }
}

template <int32_t MaxLengthExcludingNullTerminator>
void EOSString_AnsiString<MaxLengthExcludingNullTerminator>::FreeFromCharBuffer(const char *&CharPtr)
{
    if (CharPtr != nullptr)
    {
        FMemory::Free((void *)CharPtr);
        CharPtr = nullptr;
    }
}

template <int32_t MaxLengthExcludingNullTerminator>
void EOSString_AnsiString<MaxLengthExcludingNullTerminator>::FreeFromCharBufferConst(const char *const CharPtr)
{
    if (CharPtr != nullptr)
    {
        FMemory::Free((void *)CharPtr);
    }
}

template <int32_t MaxLengthExcludingNullTerminator>
void EOSString_AnsiString<MaxLengthExcludingNullTerminator>::AllocateToCharList(
    const TArray<FString> &InIds,
    uint32_t &OutIdCount,
    const char **&OutIds)
{
    if (InIds.Num() == 0)
    {
        OutIdCount = 0;
        OutIds = nullptr;
        return;
    }

    OutIdCount = InIds.Num();
    OutIds = (const char **)FMemory::MallocZeroed(OutIdCount * sizeof(const char *), 0);
    for (uint32_t i = 0; i < OutIdCount; i++)
    {
        if (AllocateToCharBuffer(InIds[i], OutIds[i]) != EOS_EResult::EOS_Success)
        {
            continue;
        }
    }
}

template <int32_t MaxLengthExcludingNullTerminator>
void EOSString_AnsiString<MaxLengthExcludingNullTerminator>::FreeFromCharList(uint32_t IdCount, const char **&Ids)
{
    if (IdCount == 0 && Ids == nullptr)
    {
        return;
    }

    for (uint32_t i = 0; i < IdCount; i++)
    {
        FreeFromCharBuffer(Ids[i]);
    }
    FMemory::Free((void *)Ids);
    Ids = nullptr;
}

template <int32_t MaxLengthExcludingNullTerminator>
void EOSString_AnsiString<MaxLengthExcludingNullTerminator>::FreeFromCharListConst(
    uint32_t IdCount,
    const char **const Ids)
{
    if (IdCount == 0 && Ids == nullptr)
    {
        return;
    }

    for (uint32_t i = 0; i < IdCount; i++)
    {
        FreeFromCharBuffer(Ids[i]);
    }
    FMemory::Free((void *)Ids);
}

// ===== UTF8 data values =====

template <int32_t MaxLengthExcludingNullTerminator>
bool EOSString_Utf8String<MaxLengthExcludingNullTerminator>::IsValid(const FString &InString)
{
    return InString.Len() > 0 && (MaxLengthExcludingNullTerminator == EOS_UNLIMITED_MAX_LENGTH ||
                                  InString.Len() <= MaxLengthExcludingNullTerminator);
}

template <int32_t MaxLengthExcludingNullTerminator>
EOSString_Utf8String<MaxLengthExcludingNullTerminator>::Utf8String::Utf8String(const FString &InString)
    : StrTCHAR(InString)
    , Result(
          (MaxLengthExcludingNullTerminator == EOS_UNLIMITED_MAX_LENGTH ||
           this->StrTCHAR.Len() <= MaxLengthExcludingNullTerminator)
              ? EOS_EResult::EOS_Success
              : EOS_EResult::EOS_LimitExceeded)
    , Ptr(FTCHARToUTF8(*this->StrTCHAR))
{
}

template <int32_t MaxLengthExcludingNullTerminator>
FString EOSString_Utf8String<MaxLengthExcludingNullTerminator>::FromUtf8String(const char *InPtr)
{
    return FromUtf8String((const UTF8CHAR *)InPtr);
}

template <int32_t MaxLengthExcludingNullTerminator>
FString EOSString_Utf8String<MaxLengthExcludingNullTerminator>::FromUtf8String(const UTF8CHAR *InPtr)
{
    return UTF8_TO_TCHAR(InPtr);
}

template <int32_t MaxLengthExcludingNullTerminator>
typename EOSString_Utf8String<MaxLengthExcludingNullTerminator>::Utf8String EOSString_Utf8String<
    MaxLengthExcludingNullTerminator>::ToUtf8String(const FString &InString)
{
    return Utf8String(InString);
}

template <int32_t MaxLengthExcludingNullTerminator>
EOS_EResult EOSString_Utf8String<MaxLengthExcludingNullTerminator>::AllocateToCharBuffer(
    const FString &InString,
    const char *&OutCharPtr)
{
    if (OutCharPtr != nullptr)
    {
        return EOS_EResult::EOS_InvalidParameters;
    }

    auto StrBuf = FTCHARToUTF8(*InString);
    if (MaxLengthExcludingNullTerminator != EOS_UNLIMITED_MAX_LENGTH &&
        StrBuf.Length() > MaxLengthExcludingNullTerminator)
    {
        return EOS_EResult::EOS_LimitExceeded;
    }
    OutCharPtr = (char *)FMemory::MallocZeroed(StrBuf.Length() + 1);
    FMemory::Memcpy((void *)OutCharPtr, (const void *)StrBuf.Get(), StrBuf.Length());
    return EOS_EResult::EOS_Success;
}

template <int32_t MaxLengthExcludingNullTerminator>
void EOSString_Utf8String<MaxLengthExcludingNullTerminator>::FreeFromCharBuffer(const char *&CharPtr)
{
    if (CharPtr != nullptr)
    {
        FMemory::Free((void *)CharPtr);
        CharPtr = nullptr;
    }
}

// ===== implementation declarations =====

template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_OpaqueId<
    EOS_ProductUserId_TypeName,
    EOS_ProductUserId,
    EOS_PRODUCTUSERID_MAX_LENGTH,
    EOS_ProductUserId_IsValid,
    EOS_ProductUserId_ToString,
    EOS_ProductUserId_FromString>;
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_OpaqueId<
    EOS_EpicAccountId_TypeName,
    EOS_EpicAccountId,
    EOS_EPICACCOUNTID_MAX_LENGTH,
    EOS_EpicAccountId_IsValid,
    EOS_EpicAccountId_ToString,
    EOS_EpicAccountId_FromString>;
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_OpaqueToken<EOS_ContinuanceToken>;

template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_CharBasedId<EOS_LobbyId, EOS_UNLIMITED_MAX_LENGTH>;

// Clang forces us to not have duplicate instantiates of explicit templates, hence this
// horrific #if/#endif nonsense.
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_UNLIMITED_MAX_LENGTH>;
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS>;
#if EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH>;
#endif
#if EOS_LOBBY_INVITEID_MAX_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                                  \
    EOS_LOBBY_INVITEID_MAX_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_LOBBY_INVITEID_MAX_LENGTH>;
#endif
#if EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                     \
    EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                        \
    EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH>;
#endif
#if EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                   \
    EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                      \
    EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                                   \
    EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH>;
#endif
#if EOS_SESSIONS_INVITEID_MAX_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                               \
    EOS_SESSIONS_INVITEID_MAX_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                                  \
    EOS_SESSIONS_INVITEID_MAX_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                                               \
    EOS_SESSIONS_INVITEID_MAX_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH &&                                  \
    EOS_SESSIONS_INVITEID_MAX_LENGTH != EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_SESSIONS_INVITEID_MAX_LENGTH>;
#endif
#if EOS_P2P_SOCKET_NAME_MAX_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                                 \
    EOS_P2P_SOCKET_NAME_MAX_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                                    \
    EOS_P2P_SOCKET_NAME_MAX_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                                                 \
    EOS_P2P_SOCKET_NAME_MAX_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH &&                                    \
    EOS_P2P_SOCKET_NAME_MAX_LENGTH != EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH &&                                  \
    EOS_P2P_SOCKET_NAME_MAX_LENGTH != EOS_SESSIONS_INVITEID_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_P2P_SOCKET_NAME_MAX_LENGTH>;
#endif
#if EOS_PRESENCE_DATA_MAX_KEY_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                               \
    EOS_PRESENCE_DATA_MAX_KEY_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                                  \
    EOS_PRESENCE_DATA_MAX_KEY_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                                               \
    EOS_PRESENCE_DATA_MAX_KEY_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH &&                                  \
    EOS_PRESENCE_DATA_MAX_KEY_LENGTH != EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH &&                                \
    EOS_PRESENCE_DATA_MAX_KEY_LENGTH != EOS_SESSIONS_INVITEID_MAX_LENGTH &&                                            \
    EOS_PRESENCE_DATA_MAX_KEY_LENGTH != EOS_P2P_SOCKET_NAME_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_PRESENCE_DATA_MAX_KEY_LENGTH>;
#endif
#if EOS_PRESENCE_DATA_MAX_VALUE_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                             \
    EOS_PRESENCE_DATA_MAX_VALUE_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                                \
    EOS_PRESENCE_DATA_MAX_VALUE_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                                             \
    EOS_PRESENCE_DATA_MAX_VALUE_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH &&                                \
    EOS_PRESENCE_DATA_MAX_VALUE_LENGTH != EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH &&                              \
    EOS_PRESENCE_DATA_MAX_VALUE_LENGTH != EOS_SESSIONS_INVITEID_MAX_LENGTH &&                                          \
    EOS_PRESENCE_DATA_MAX_VALUE_LENGTH != EOS_P2P_SOCKET_NAME_MAX_LENGTH &&                                            \
    EOS_PRESENCE_DATA_MAX_VALUE_LENGTH != EOS_PRESENCE_DATA_MAX_KEY_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_PRESENCE_DATA_MAX_VALUE_LENGTH>;
#endif
#if EOS_ATTRIBUTE_MAX_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                                       \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                                          \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                                                       \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH &&                                          \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH &&                                        \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_SESSIONS_INVITEID_MAX_LENGTH &&                                                    \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_P2P_SOCKET_NAME_MAX_LENGTH &&                                                      \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_PRESENCE_DATA_MAX_KEY_LENGTH &&                                                    \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_PRESENCE_DATA_MAX_VALUE_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_ATTRIBUTE_MAX_LENGTH>;
#endif
#if EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                           \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                              \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                                           \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH &&                              \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH &&                            \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_SESSIONS_INVITEID_MAX_LENGTH &&                                        \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_P2P_SOCKET_NAME_MAX_LENGTH &&                                          \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_PRESENCE_DATA_MAX_KEY_LENGTH &&                                        \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_PRESENCE_DATA_MAX_VALUE_LENGTH &&                                      \
    EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH != EOS_ATTRIBUTE_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH>;
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
#if EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&           \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&              \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                           \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH &&              \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH &&            \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_SESSIONS_INVITEID_MAX_LENGTH &&                        \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_P2P_SOCKET_NAME_MAX_LENGTH &&                          \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_PRESENCE_DATA_MAX_KEY_LENGTH &&                        \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_PRESENCE_DATA_MAX_VALUE_LENGTH &&                      \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_ATTRIBUTE_MAX_LENGTH &&                                \
    EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH != EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API
    EOSString_AnsiString<EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH>;
#endif
#if EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_STRING_TESTS_ANSI_STRING_LENGTH_FOR_TESTS &&                                \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH &&                                   \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_LOBBY_INVITEID_MAX_LENGTH &&                                                \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_LOBBYMODIFICATION_MAX_ATTRIBUTE_LENGTH &&                                   \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH &&                                 \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_SESSIONS_INVITEID_MAX_LENGTH &&                                             \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_P2P_SOCKET_NAME_MAX_LENGTH &&                                               \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_PRESENCE_DATA_MAX_KEY_LENGTH &&                                             \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_PRESENCE_DATA_MAX_VALUE_LENGTH &&                                           \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_ATTRIBUTE_MAX_LENGTH &&                                                     \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_PLAYERDATASTORAGE_MD5_MAX_LENGTH &&                                         \
    EOS_TITLESTORAGE_TAG_MAX_LENGTH != EOS_SESSIONMODIFICATION_MAX_SESSIONIDOVERRIDE_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_AnsiString<EOS_TITLESTORAGE_TAG_MAX_LENGTH>;
#endif
#endif

template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_UNLIMITED_MAX_LENGTH>;
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS>;
#if EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH != EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH>;
#endif
#if EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH != EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS &&              \
    EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH != EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH>;
#endif
#if EOS_ATTRIBUTE_MAX_LENGTH != EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS &&                                       \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH &&                                    \
    EOS_ATTRIBUTE_MAX_LENGTH != EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_ATTRIBUTE_MAX_LENGTH>;
#endif
#if EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH != EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS &&                        \
    EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH != EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH &&                     \
    EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH != EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH &&                    \
    EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH != EOS_ATTRIBUTE_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH>;
#endif
#if EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS &&                \
    EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH &&             \
    EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH &&            \
    EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_ATTRIBUTE_MAX_LENGTH &&                                     \
    EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES>;
#endif
#if EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH != EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS &&                       \
    EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH != EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH &&                    \
    EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH != EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH &&                   \
    EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH != EOS_ATTRIBUTE_MAX_LENGTH &&                                            \
    EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH != EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH &&                             \
    EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH != EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH>;
#endif
#if EOS_ANDROID_DIRECTORY_MAX_LENGTH != EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS &&                               \
    EOS_ANDROID_DIRECTORY_MAX_LENGTH != EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH &&                            \
    EOS_ANDROID_DIRECTORY_MAX_LENGTH != EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH &&                           \
    EOS_ANDROID_DIRECTORY_MAX_LENGTH != EOS_ATTRIBUTE_MAX_LENGTH &&                                                    \
    EOS_ANDROID_DIRECTORY_MAX_LENGTH != EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH &&                                     \
    EOS_ANDROID_DIRECTORY_MAX_LENGTH != EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES &&                             \
    EOS_ANDROID_DIRECTORY_MAX_LENGTH != EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_ANDROID_DIRECTORY_MAX_LENGTH>;
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
#if EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_STRING_TESTS_UTF8_STRING_LENGTH_FOR_TESTS &&                     \
    EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_CONNECT_USERLOGININFO_DISPLAYNAME_MAX_LENGTH &&                  \
    EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_CONNECT_CREATEDEVICEID_DEVICEMODEL_MAX_LENGTH &&                 \
    EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_ATTRIBUTE_MAX_LENGTH &&                                          \
    EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_PRESENCE_RICH_TEXT_MAX_VALUE_LENGTH &&                           \
    EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_PLAYERDATASTORAGE_FILENAME_MAX_LENGTH_BYTES &&                   \
    EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_USERINFO_MAX_DISPLAYNAME_UTF8_LENGTH &&                          \
    EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES != EOS_ANDROID_DIRECTORY_MAX_LENGTH
template class ONLINESUBSYSTEMREDPOINTEOS_API EOSString_Utf8String<EOS_TITLESTORAGE_FILENAME_MAX_LENGTH_BYTES>;
#endif
#endif

EOS_DISABLE_STRICT_WARNINGS