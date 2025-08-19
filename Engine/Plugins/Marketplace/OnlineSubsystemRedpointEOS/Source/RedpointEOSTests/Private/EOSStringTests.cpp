// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

// ===== opaque pointer based tokens =====

// NOTE: EOSString_EpicAccountId isn't tested here since it uses the exact same
// code as EOSString_ProductUserId (from a template).

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueId_IsNone,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueId.IsNone",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueId_IsNone::RunTest(const FString &Parameters)
{
    TestTrue("IsNone returns true for nullptr", EOSString_ProductUserId::IsNone(nullptr));
    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueId_IsValid,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueId.IsValid",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueId_IsValid::RunTest(const FString &Parameters)
{
    EOS_ProductUserId InvalidPUID = EOS_ProductUserId_FromString("");
    EOS_ProductUserId ValidPUID = EOS_ProductUserId_FromString("0002c0286b8545c888e44617a1f1d0ff");

    // @todo: Figure out what input string returns an invalid PUID.
    // TestFalse("IsValid returns false for invalid PUID", EOSString_ProductUserId::IsValid(InvalidPUID));
    TestFalse("IsNone returns false for invalid, but present PUID", EOSString_ProductUserId::IsNone(InvalidPUID));
    TestFalse("IsValid returns false for nullptr", EOSString_ProductUserId::IsValid(nullptr));

    TestTrue("IsValid returns true for valid PUID", EOSString_ProductUserId::IsValid(ValidPUID));
    TestFalse("IsNone returns false for valid PUID", EOSString_ProductUserId::IsNone(ValidPUID));

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueId_ToString,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueId.ToString",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueId_ToString::RunTest(const FString &Parameters)
{
    EOS_ProductUserId ValidPUID = EOS_ProductUserId_FromString("0002c0286b8545c888e44617a1f1d0ff");

    FString PUIDToString;
    EOS_EResult Result = EOSString_ProductUserId::ToString(ValidPUID, PUIDToString);

    TestEqual("ToString returns EOS_Success for serialize call", Result, EOS_EResult::EOS_Success);
    TestEqual("ToString returns original PUID string", PUIDToString, TEXT("0002c0286b8545c888e44617a1f1d0ff"));

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueId_ToAnsiString,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueId.ToAnsiString",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueId_ToAnsiString::RunTest(const FString &Parameters)
{
    EOS_ProductUserId ValidPUID = EOS_ProductUserId_FromString("0002c0286b8545c888e44617a1f1d0ff");

    EOSString_ProductUserId::AnsiString Str = EOSString_ProductUserId::ToAnsiString(ValidPUID);

    TestEqual("ToAnsiString returns EOS_Success for serialize call", Str.Result, EOS_EResult::EOS_Success);
    TestTrue(
        "ToAnsiString returns original PUID string",
        strcmp(Str.Ptr.Get(), "0002c0286b8545c888e44617a1f1d0ff") == 0);

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueId_FromString,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueId.FromString",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueId_FromString::RunTest(const FString &Parameters)
{
    EOS_ProductUserId ValidPUID = {};
    EOS_EResult ValidResult = EOSString_ProductUserId::FromString(TEXT("0002c0286b8545c888e44617a1f1d0ff"), ValidPUID);

    TestEqual(
        "FromString returns EOS_Success for deserialize call on valid PUID string",
        ValidResult,
        EOS_EResult::EOS_Success);
    TestTrue("FromString returns a valid PUID for valid PUID string", EOS_ProductUserId_IsValid(ValidPUID) == EOS_TRUE);

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueId_AllocAndFree,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueId.AllocAndFree",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueId_AllocAndFree::RunTest(const FString &Parameters)
{
    EOS_ProductUserId PUID = {};
    EOS_EResult PUIDResult = EOSString_ProductUserId::FromString(TEXT("0002c0286b8545c888e44617a1f1d0ff"), PUID);

    TestEqual("AllocAndFree test could obtain a valid PUID", PUIDResult, EOS_EResult::EOS_Success);
    if (PUIDResult != EOS_EResult::EOS_Success)
    {
        return false;
    }

    FGenericMemoryStats MemoryStats;
    GMalloc->GetAllocatorStats(MemoryStats);
    int PrevAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int PrevFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int PrevDiff = PrevAllocateCalls - PrevFreeCalls;

    AddExpectedError("EOSString_OpaqueId::AllocateToCharBuffer failed when converting 'EOS_ProductUserId', got result: "
                     "EOS_InvalidUser");

    const char *Buffer = nullptr;
    EOS_EResult InvalidAllocResult = EOSString_ProductUserId::AllocateToCharBuffer(nullptr, Buffer);
    TestNotEqual(
        "AllocateToCharBuffer with nullptr returns invalid result",
        InvalidAllocResult,
        EOS_EResult::EOS_Success);
    if (InvalidAllocResult == EOS_EResult::EOS_Success)
    {
        EOSString_ProductUserId::FreeFromCharBuffer(Buffer);
    }
    TestEqual("Buffer is null after calling AllocateToCharBuffer with invalid ID", Buffer, (const char *)nullptr);

    EOS_EResult AllocResult = EOSString_ProductUserId::AllocateToCharBuffer(PUID, Buffer);
    TestEqual("AllocateToCharBuffer was able to allocate to a char buffer", AllocResult, EOS_EResult::EOS_Success);
    if (AllocResult == EOS_EResult::EOS_Success)
    {
        TestNotEqual("Resulting buffer was not null", Buffer, (const char *)nullptr);
        TestTrue("Resulting buffer matches string", strcmp(Buffer, "0002c0286b8545c888e44617a1f1d0ff") == 0);

        EOSString_ProductUserId::FreeFromCharBuffer(Buffer);
        TestEqual("Resulting buffer is null after calling FreeFromCharBuffer", Buffer, (const char *)nullptr);
    }

    GMalloc->GetAllocatorStats(MemoryStats);
    int NextAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int NextFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int NextDiff = PrevAllocateCalls - PrevFreeCalls;

    TestEqual("All allocated memory was freed", PrevDiff, NextDiff);

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueId_AllocAndFreeCharList,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueId.AllocAndFreeCharList",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueId_AllocAndFreeCharList::RunTest(const FString &Parameters)
{
    TArray<FString> PUIDStrings;
    PUIDStrings.Add(TEXT("0002c0286b8545c888e44617a1f1d0ff"));
    PUIDStrings.Add(TEXT("0002c0286b8545c888e44617a1f1d020"));
    PUIDStrings.Add(TEXT("0002c0286b8545c888e44617a1f1d060"));

    TArray<EOS_ProductUserId> PUIDList;
    for (const auto &Str : PUIDStrings)
    {
        EOS_ProductUserId PUID = {};
        EOS_EResult PUIDResult = EOSString_ProductUserId::FromString(Str, PUID);
        TestEqual(
            FString::Printf(TEXT("AllocAndFreeList test could obtain a valid PUID (%s)"), *Str),
            PUIDResult,
            EOS_EResult::EOS_Success);
        if (PUIDResult != EOS_EResult::EOS_Success)
        {
            return false;
        }
        PUIDList.Add(PUID);
    }

    FGenericMemoryStats MemoryStats;
    GMalloc->GetAllocatorStats(MemoryStats);
    int PrevAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int PrevFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int PrevDiff = PrevAllocateCalls - PrevFreeCalls;

    const char **List = nullptr;
    uint32_t ListCount = 0;
    EOSString_ProductUserId::AllocateToCharList(PUIDList, ListCount, List);
    TestEqual("AllocateToCharList was able to allocate all PUIDs to list", ListCount, PUIDStrings.Num());
    TestNotEqual("Resulting list was not null", List, (const char **)nullptr);
    if (List != nullptr)
    {
        for (uint32_t i = 0; i < ListCount; i++)
        {
            TestNotEqual(FString::Printf(TEXT("List entry at %d was not null"), i), List[i], (const char *)nullptr);
            if (List[i] != nullptr)
            {
                TestTrue(
                    FString::Printf(TEXT("List entry at %d matches expected string"), i),
                    strcmp(List[i], TCHAR_TO_ANSI(*PUIDStrings[i])) == 0);
            }
        }
    }
    EOSString_ProductUserId::FreeFromCharList(ListCount, List);
    TestEqual("Resulting list is null after calling FreeFromCharList", List, (const char **)nullptr);

    List = nullptr;
    ListCount = 0;
    EOSString_ProductUserId::AllocateToCharList(PUIDList, ListCount, List);
    TestEqual("AllocateToCharList was able to allocate all PUIDs to list", ListCount, PUIDStrings.Num());
    TestNotEqual("Resulting list was not null", List, (const char **)nullptr);
    if (List != nullptr)
    {
        for (uint32_t i = 0; i < ListCount; i++)
        {
            TestNotEqual(FString::Printf(TEXT("List entry at %d was not null"), i), List[i], (const char *)nullptr);
            if (List[i] != nullptr)
            {
                TestTrue(
                    FString::Printf(TEXT("List entry at %d matches expected string"), i),
                    strcmp(List[i], TCHAR_TO_ANSI(*PUIDStrings[i])) == 0);
            }
        }
    }
    EOSString_ProductUserId::FreeFromCharListConst(ListCount, List);
    TestNotEqual("Resulting list is not null after calling FreeFromCharListConst", List, (const char **)nullptr);

    GMalloc->GetAllocatorStats(MemoryStats);
    int NextAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int NextFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int NextDiff = PrevAllocateCalls - PrevFreeCalls;

    TestEqual("All allocated memory was freed", PrevDiff, NextDiff);

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueId_AllocAndFreeIdList,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueId.AllocAndFreeIdList",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueId_AllocAndFreeIdList::RunTest(const FString &Parameters)
{
    TArray<FString> PUIDStrings;
    PUIDStrings.Add(TEXT("0002c0286b8545c888e44617a1f1d0ff"));
    PUIDStrings.Add(TEXT("0002c0286b8545c888e44617a1f1d020"));
    PUIDStrings.Add(TEXT("0002c0286b8545c888e44617a1f1d060"));

    TArray<EOS_ProductUserId> PUIDList;
    for (const auto &Str : PUIDStrings)
    {
        EOS_ProductUserId PUID = {};
        EOS_EResult PUIDResult = EOSString_ProductUserId::FromString(Str, PUID);
        TestEqual(
            FString::Printf(TEXT("AllocAndFreeList test could obtain a valid PUID (%s)"), *Str),
            PUIDResult,
            EOS_EResult::EOS_Success);
        if (PUIDResult != EOS_EResult::EOS_Success)
        {
            return false;
        }
        PUIDList.Add(PUID);
    }

    FGenericMemoryStats MemoryStats;
    GMalloc->GetAllocatorStats(MemoryStats);
    int PrevAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int PrevFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int PrevDiff = PrevAllocateCalls - PrevFreeCalls;

    EOS_ProductUserId *List = nullptr;
    uint32_t ListCount = 0;
    EOSString_ProductUserId::AllocateToIdList(PUIDList, ListCount, List);
    TestEqual("AllocateToIdList was able to allocate all PUIDs to list", ListCount, PUIDStrings.Num());
    TestNotEqual("Resulting list was not null", List, (EOS_ProductUserId *)nullptr);
    if (List != nullptr)
    {
        for (uint32_t i = 0; i < ListCount; i++)
        {
            TestNotEqual(
                FString::Printf(TEXT("List entry at %d was not null"), i),
                List[i],
                (EOS_ProductUserId) nullptr);
            if (List[i] != nullptr)
            {
                FString ConvertedPUID;
                EOS_EResult ConvertResult = EOSString_ProductUserId::ToString(List[i], ConvertedPUID);
                TestEqual(
                    FString::Printf(TEXT("List entry at %d can be serialized with ToString"), i),
                    ConvertResult,
                    EOS_EResult::EOS_Success);
                TestEqual(
                    FString::Printf(TEXT("List entry at %d matches expected string"), i),
                    ConvertedPUID,
                    PUIDStrings[i]);
            }
        }
    }
    EOSString_ProductUserId::FreeFromIdList(ListCount, List);
    TestEqual("Resulting list is null after calling FreeFromIdList", List, (EOS_ProductUserId *)nullptr);

    List = nullptr;
    ListCount = 0;
    EOSString_ProductUserId::AllocateToIdList(PUIDList, ListCount, List);
    TestEqual("AllocateToIdList was able to allocate all PUIDs to list", ListCount, PUIDStrings.Num());
    TestNotEqual("Resulting list was not null", List, (EOS_ProductUserId *)nullptr);
    if (List != nullptr)
    {
        for (uint32_t i = 0; i < ListCount; i++)
        {
            TestNotEqual(
                FString::Printf(TEXT("List entry at %d was not null"), i),
                List[i],
                (EOS_ProductUserId) nullptr);
            if (List[i] != nullptr)
            {
                FString ConvertedPUID;
                EOS_EResult ConvertResult = EOSString_ProductUserId::ToString(List[i], ConvertedPUID);
                TestEqual(
                    FString::Printf(TEXT("List entry at %d can be serialized with ToString"), i),
                    ConvertResult,
                    EOS_EResult::EOS_Success);
                TestEqual(
                    FString::Printf(TEXT("List entry at %d matches expected string"), i),
                    ConvertedPUID,
                    PUIDStrings[i]);
            }
        }
    }
    EOSString_ProductUserId::FreeFromIdListConst(ListCount, List);
    TestNotEqual("Resulting list is not null after calling FreeFromIdListConst", List, (EOS_ProductUserId *)nullptr);

    GMalloc->GetAllocatorStats(MemoryStats);
    int NextAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int NextFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int NextDiff = PrevAllocateCalls - PrevFreeCalls;

    TestEqual("All allocated memory was freed", PrevDiff, NextDiff);

    return true;
}

// ===== const char* based Ids =====

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_CharBasedId_IsNone,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.CharBasedId.IsNone",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_CharBasedId_IsNone::RunTest(const FString &Parameters)
{
    TestTrue("IsNone returns true for nullptr", EOSString_LobbyId::IsNone(nullptr));
    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_CharBasedId_IsValid,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.CharBasedId.IsValid",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_CharBasedId_IsValid::RunTest(const FString &Parameters)
{
    EOS_LobbyId EmptyLobbyId = "";
    EOS_LobbyId ValidLobbyId = "SomeLobbyId";

    TestFalse("IsNone returns false when provided an invalid lobby ID", EOSString_LobbyId::IsNone(EmptyLobbyId));
    TestFalse("IsNone returns false when provided a valid lobby ID", EOSString_LobbyId::IsNone(ValidLobbyId));

    TestFalse("IsValid returns false when provided a nullptr", EOSString_LobbyId::IsValid(nullptr));
    TestFalse("IsValid returns false when provided an invalid lobby ID", EOSString_LobbyId::IsValid(EmptyLobbyId));
    TestTrue("IsValid returns true when provided a valid lobby ID", EOSString_LobbyId::IsValid(ValidLobbyId));

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_CharBasedId_ToString,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.CharBasedId.ToString",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_CharBasedId_ToString::RunTest(const FString &Parameters)
{
    EOS_LobbyId ValidLobbyId = "SomeLobbyId";

    TestEqual(
        "ToString returns correct value for valid lobby ID",
        EOSString_LobbyId::ToString(ValidLobbyId),
        TEXT("SomeLobbyId"));

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_CharBasedId_AllocAndFree,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.CharBasedId.AllocAndFree",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_CharBasedId_AllocAndFree::RunTest(const FString &Parameters)
{
    EOS_LobbyId InvalidLobbyId = "";
    EOS_LobbyId ValidLobbyId = "SomeLobbyId";

    FGenericMemoryStats MemoryStats;
    GMalloc->GetAllocatorStats(MemoryStats);
    int PrevAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int PrevFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int PrevDiff = PrevAllocateCalls - PrevFreeCalls;

    const char *Buffer = nullptr;
    int32 BufferLen = 0;

    {
        EOS_EResult NullptrAllocResult = EOSString_LobbyId::AllocateToCharBuffer(nullptr, Buffer, BufferLen);
        TestNotEqual(
            "AllocateToCharBuffer with nullptr returns EOS_InvalidParameters",
            NullptrAllocResult,
            EOS_EResult::EOS_Success);
        if (NullptrAllocResult == EOS_EResult::EOS_Success)
        {
            EOSString_LobbyId::FreeFromCharBuffer(Buffer);
        }
    }

    {
        EOS_EResult InvalidAllocResult = EOSString_LobbyId::AllocateToCharBuffer(InvalidLobbyId, Buffer, BufferLen);
        TestNotEqual(
            "AllocateToCharBuffer with invalid lobby ID returns EOS_InvalidParameters",
            InvalidAllocResult,
            EOS_EResult::EOS_Success);
        if (InvalidAllocResult == EOS_EResult::EOS_Success)
        {
            EOSString_LobbyId::FreeFromCharBuffer(Buffer);
        }
    }

    {
        // Set to non-null for this test.
        Buffer = (const char *)1;
        EOS_EResult ExistingAllocResult = EOSString_LobbyId::AllocateToCharBuffer(ValidLobbyId, Buffer, BufferLen);
        TestNotEqual(
            "AllocateToCharBuffer with already allocated buffer returns EOS_InvalidParameters",
            ExistingAllocResult,
            EOS_EResult::EOS_Success);
        if (ExistingAllocResult == EOS_EResult::EOS_Success)
        {
            EOSString_LobbyId::FreeFromCharBuffer(Buffer);
        }
    }

    {
        Buffer = nullptr;
        BufferLen = 0;
        EOS_EResult ValidAllocResult = EOSString_LobbyId::AllocateToCharBuffer(ValidLobbyId, Buffer, BufferLen);
        TestEqual(
            "AllocateToCharBuffer with valid lobby ID returns EOS_Success",
            ValidAllocResult,
            EOS_EResult::EOS_Success);
        if (ValidAllocResult == EOS_EResult::EOS_Success)
        {
            TestNotEqual("AllocateToCharBuffer with valid lobby ID returns non-nullptr", Buffer, (const char *)nullptr);
            TestEqual("AllocateToCharBuffer with valid lobby ID allocates buffer of correct size", BufferLen, 12);
            TestTrue(
                "AllocateToCharBuffer with valid lobby ID allocates buffer with correct data",
                strcmp(Buffer, "SomeLobbyId") == 0);

            EOSString_LobbyId::FreeFromCharBuffer(Buffer);
            TestEqual("FreeFromCharBuffer sets buffer back to nullptr", Buffer, (const char *)nullptr);
        }
    }

    GMalloc->GetAllocatorStats(MemoryStats);
    int NextAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int NextFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int NextDiff = PrevAllocateCalls - PrevFreeCalls;

    TestEqual("All allocated memory was freed", PrevDiff, NextDiff);

    return true;
}

// ===== const char* based tokens =====

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_OpaqueToken_IsNone,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.OpaqueToken.IsNone",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_OpaqueToken_IsNone::RunTest(const FString &Parameters)
{
    EOS_ContinuanceToken NoneToken = nullptr;
    EOS_ContinuanceToken PresentToken = (EOS_ContinuanceToken)1;

    TestTrue("IsNone returns true for nullptr", EOSString_ContinuanceToken::IsNone(NoneToken));
    TestFalse("IsNone returns false for present value", EOSString_ContinuanceToken::IsNone(PresentToken));

    return true;
}

// ===== ANSI data values =====

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_AnsiString_IsValid,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.AnsiString.IsValid",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_AnsiString_IsValid::RunTest(const FString &Parameters)
{
    TestFalse("IsValid returns false for empty string", EOSString_AnsiStringForTests::IsValid(TEXT("")));
    TestTrue(
        "IsValid returns true for string within bounds (<32 chars)",
        EOSString_AnsiStringForTests::IsValid(TEXT("LessThan32Characters")));
    TestTrue(
        "IsValid returns true for string within bounds (=32 chars)",
        EOSString_AnsiStringForTests::IsValid(TEXT("Exactly32CharactersLong_12345678")));
    TestFalse(
        "IsValid returns false for string too long (>32 chars)",
        EOSString_AnsiStringForTests::IsValid(TEXT("MoreThan32CharactersLong_12345678")));

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_AnsiString_ToAnsiString,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.AnsiString.ToAnsiString",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_AnsiString_ToAnsiString::RunTest(const FString &Parameters)
{
    EOSString_AnsiStringForTests::AnsiString EmptyStr = EOSString_AnsiStringForTests::ToAnsiString(TEXT(""));
    TestEqual("ToAnsiString returns EOS_Success for empty string", EmptyStr.Result, EOS_EResult::EOS_Success);
    TestEqual("ToAnsiString returns empty AnsiString with original string in StrTCHAR", EmptyStr.StrTCHAR, TEXT(""));
    TestTrue("ToAnsiString returns empty AnsiString with correct const char* ptr", strcmp(EmptyStr.Ptr.Get(), "") == 0);

    EOSString_AnsiStringForTests::AnsiString TooLongStr =
        EOSString_AnsiStringForTests::ToAnsiString(TEXT("MoreThan32CharactersLong_12345678"));
    TestEqual(
        "ToAnsiString returns EOS_LimitExceeded for string that is too long",
        TooLongStr.Result,
        EOS_EResult::EOS_LimitExceeded);

    EOSString_AnsiStringForTests::AnsiString ValidStr =
        EOSString_AnsiStringForTests::ToAnsiString(TEXT("LessThan32Characters"));
    TestEqual("ToAnsiString returns EOS_Success for valid string", ValidStr.Result, EOS_EResult::EOS_Success);
    TestEqual(
        "ToAnsiString returns AnsiString with original string in StrTCHAR",
        ValidStr.StrTCHAR,
        TEXT("LessThan32Characters"));
    TestTrue(
        "ToAnsiString returns AnsiString with correct const char* ptr",
        strcmp(ValidStr.Ptr.Get(), "LessThan32Characters") == 0);

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_AnsiString_AllocAndFree,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.AnsiString.AllocAndFree",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_AnsiString_AllocAndFree::RunTest(const FString &Parameters)
{
    FGenericMemoryStats MemoryStats;
    GMalloc->GetAllocatorStats(MemoryStats);
    int PrevAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int PrevFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int PrevDiff = PrevAllocateCalls - PrevFreeCalls;

    const char *Buffer = nullptr;
    int32 BufferLen = 0;

    {
        Buffer = nullptr;
        BufferLen = 0;
        EOS_EResult EmptyAllocResult = EOSString_AnsiStringForTests::AllocateToCharBuffer(TEXT(""), Buffer, BufferLen);
        TestEqual(
            "AllocateToCharBuffer returns EOS_Success for empty string",
            EmptyAllocResult,
            EOS_EResult::EOS_Success);
        if (EmptyAllocResult == EOS_EResult::EOS_Success)
        {
            TestNotEqual("AllocateToCharBuffer with empty string returns non-nullptr", Buffer, (const char *)nullptr);
            TestTrue("AllocateToCharBuffer with empty string allocates empty buffer", strcmp(Buffer, "") == 0);
            TestEqual("AllocateToCharBuffer with empty string allocates buffer of correct size", BufferLen, 1);

            EOSString_AnsiStringForTests::FreeFromCharBuffer(Buffer);
            TestEqual("FreeFromCharBuffer sets buffer back to nullptr", Buffer, (const char *)nullptr);
        }
    }

    {
        Buffer = nullptr;
        BufferLen = 0;
        EOS_EResult TooLongAllocResult =
            EOSString_AnsiStringForTests::AllocateToCharBuffer(TEXT("MoreThan32CharactersLong_12345678"), Buffer);
        TestEqual(
            "AllocateToCharBuffer returns EOS_LimitExceeded for string too long",
            TooLongAllocResult,
            EOS_EResult::EOS_LimitExceeded);
        if (TooLongAllocResult == EOS_EResult::EOS_Success)
        {
            EOSString_AnsiStringForTests::FreeFromCharBuffer(Buffer);
        }
    }

    {
        Buffer = (const char *)1;
        BufferLen = 0;
        EOS_EResult ExistingBufferAllocResult =
            EOSString_AnsiStringForTests::AllocateToCharBuffer(TEXT("Exactly32CharactersLong_12345678"), Buffer);
        TestEqual(
            "AllocateToCharBuffer returns EOS_InvalidParameters for existing buffer",
            ExistingBufferAllocResult,
            EOS_EResult::EOS_InvalidParameters);
        if (ExistingBufferAllocResult == EOS_EResult::EOS_Success)
        {
            EOSString_AnsiStringForTests::FreeFromCharBuffer(Buffer);
        }
    }

    {
        Buffer = nullptr;
        BufferLen = 0;
        EOS_EResult ValidAllocResult = EOSString_AnsiStringForTests::AllocateToCharBuffer(
            TEXT("Exactly32CharactersLong_12345678"),
            Buffer,
            BufferLen);
        TestEqual(
            "AllocateToCharBuffer returns EOS_Success for valid string",
            ValidAllocResult,
            EOS_EResult::EOS_Success);
        if (ValidAllocResult == EOS_EResult::EOS_Success)
        {
            TestNotEqual("AllocateToCharBuffer with valid string returns non-nullptr", Buffer, (const char *)nullptr);
            TestTrue(
                "AllocateToCharBuffer with valid string allocates correct buffer",
                strcmp(Buffer, "Exactly32CharactersLong_12345678") == 0);
            TestEqual("AllocateToCharBuffer with valid string allocates buffer of correct size", BufferLen, 33);

            EOSString_AnsiStringForTests::FreeFromCharBuffer(Buffer);
            TestEqual("FreeFromCharBuffer sets buffer back to nullptr", Buffer, (const char *)nullptr);
        }
    }

    {
        Buffer = nullptr;
        EOS_EResult ValidAllocResult =
            EOSString_AnsiStringForTests::AllocateToCharBuffer(TEXT("Exactly32CharactersLong_12345678"), Buffer);
        TestEqual(
            "AllocateToCharBuffer (without Len argument) returns EOS_Success for valid string",
            ValidAllocResult,
            EOS_EResult::EOS_Success);
        if (ValidAllocResult == EOS_EResult::EOS_Success)
        {
            TestNotEqual(
                "AllocateToCharBuffer (without Len argument) with valid string returns non-nullptr",
                Buffer,
                (const char *)nullptr);
            TestTrue(
                "AllocateToCharBuffer (without Len argument) with valid string allocates correct buffer",
                strcmp(Buffer, "Exactly32CharactersLong_12345678") == 0);

            EOSString_AnsiStringForTests::FreeFromCharBuffer(Buffer);
            TestEqual("FreeFromCharBuffer sets buffer back to nullptr", Buffer, (const char *)nullptr);
        }
    }

    GMalloc->GetAllocatorStats(MemoryStats);
    int NextAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int NextFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int NextDiff = PrevAllocateCalls - PrevFreeCalls;

    TestEqual("All allocated memory was freed", PrevDiff, NextDiff);

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_AnsiString_CopyToExistingBuffer,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.AnsiString.CopyToExistingBuffer",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_AnsiString_CopyToExistingBuffer::RunTest(const FString &Parameters)
{
    char TargetBuffer[33];

    {
        EOS_EResult TooLongAllocResult = EOSString_AnsiStringForTests::CopyToExistingBuffer<33>(
            TEXT("MoreThan32CharactersLong_12345678"),
            TargetBuffer);
        TestEqual(
            "CopyToExistingBuffer returns EOS_LimitExceeded for too long string",
            TooLongAllocResult,
            EOS_EResult::EOS_LimitExceeded);
    }

    {
        EOS_EResult EmptyAllocResult = EOSString_AnsiStringForTests::CopyToExistingBuffer<33>(TEXT(""), TargetBuffer);
        TestEqual(
            "CopyToExistingBuffer returns EOS_Success for empty string",
            EmptyAllocResult,
            EOS_EResult::EOS_Success);
        TestTrue("CopyToExistingBuffer fills buffer with empty string", strcmp(TargetBuffer, "") == 0);
    }

    {
        EOS_EResult ValidAllocResult = EOSString_AnsiStringForTests::CopyToExistingBuffer<33>(
            TEXT("Exactly32CharactersLong_12345678"),
            TargetBuffer);
        TestEqual(
            "CopyToExistingBuffer returns EOS_Success for valid string",
            ValidAllocResult,
            EOS_EResult::EOS_Success);
        TestTrue(
            "CopyToExistingBuffer fills buffer with valid string",
            strcmp(TargetBuffer, "Exactly32CharactersLong_12345678") == 0);
    }

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_AnsiString_AllocAndFreeCharList,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.AnsiString.AllocAndFreeCharList",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_AnsiString_AllocAndFreeCharList::RunTest(const FString &Parameters)
{
    TArray<FString> InputStrings;
    InputStrings.Add("Str1");
    InputStrings.Add("Str2");
    InputStrings.Add("Str3");

    FGenericMemoryStats MemoryStats;
    GMalloc->GetAllocatorStats(MemoryStats);
    int PrevAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int PrevFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int PrevDiff = PrevAllocateCalls - PrevFreeCalls;

    const char **Ids = nullptr;
    uint32_t IdCount = 0;

    {
        EOSString_AnsiStringForTests::AllocateToCharList(InputStrings, IdCount, Ids);
        TestEqual("AllocateToCharList allocates the right number of IDs", IdCount, InputStrings.Num());
        if (IdCount == InputStrings.Num())
        {
            for (uint32_t i = 0; i < (uint32_t)InputStrings.Num(); i++)
            {
                TestNotEqual("AllocateToCharList allocated a non-null buffer", Ids[i], (const char *)nullptr);
                TestTrue(
                    "AllocateToCharList allocated a correct buffer",
                    strcmp(Ids[i], TCHAR_TO_ANSI(*InputStrings[i])) == 0);
            }
        }
        EOSString_AnsiStringForTests::FreeFromCharList(IdCount, Ids);
        TestEqual("FreeFromCharList sets list back to nullptr", Ids, (const char **)nullptr);
    }

    {
        EOSString_AnsiStringForTests::AllocateToCharList(InputStrings, IdCount, Ids);
        TestEqual("AllocateToCharList allocates the right number of IDs", IdCount, InputStrings.Num());
        if (IdCount == InputStrings.Num())
        {
            for (uint32_t i = 0; i < (uint32_t)InputStrings.Num(); i++)
            {
                TestNotEqual("AllocateToCharList allocated a non-null buffer", Ids[i], (const char *)nullptr);
                TestTrue(
                    "AllocateToCharList allocated a correct buffer",
                    strcmp(Ids[i], TCHAR_TO_ANSI(*InputStrings[i])) == 0);
            }
        }
        EOSString_AnsiStringForTests::FreeFromCharListConst(IdCount, Ids);
        TestNotEqual("FreeFromCharList does not set list back to nullptr", Ids, (const char **)nullptr);
    }

    GMalloc->GetAllocatorStats(MemoryStats);
    int NextAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int NextFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int NextDiff = PrevAllocateCalls - PrevFreeCalls;

    TestEqual("All allocated memory was freed", PrevDiff, NextDiff);

    return true;
}

// ===== UTF8 data values =====

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_Utf8String_IsValid,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.Utf8String.IsValid",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_Utf8String_IsValid::RunTest(const FString &Parameters)
{
    const TCHAR *Utf16Emoji = TEXT("\xD83D\xDE0A");
    const char *Utf8Emoji = "\xf0\x9f\x98\x8a";

    TestFalse("IsValid returns false for empty string", EOSString_Utf8StringForTests::IsValid(TEXT("")));
    TestTrue(
        "IsValid returns true for string within bounds (<32 chars)",
        EOSString_Utf8StringForTests::IsValid(TEXT("LessThan32Characters")));
    TestTrue(
        "IsValid returns true for string within bounds (=32 chars)",
        EOSString_Utf8StringForTests::IsValid(TEXT("Exactly32CharactersLong_12345678")));
    TestFalse(
        "IsValid returns false for string too long (>32 chars)",
        EOSString_Utf8StringForTests::IsValid(TEXT("MoreThan32CharactersLong_12345678")));

    TestTrue(
        "IsValid returns true for UTF-8 encodable emoji sequence (from UTF-16)",
        EOSString_Utf8StringForTests::IsValid(Utf16Emoji));
    TestTrue(
        "IsValid returns true for UTF-8 encodable emoji sequence (from UTF-8 bytes)",
        EOSString_Utf8StringForTests::IsValid(UTF8_TO_TCHAR(Utf8Emoji)));

    return true;
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_Utf8String_FromUtf8String,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.Utf8String.FromUtf8String",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_Utf8String_FromUtf8String::RunTest(const FString &Parameters)
{
    const TCHAR *Utf16Emoji = TEXT("\xD83D\xDE0A");
    const char *Utf8Emoji = "\xf0\x9f\x98\x8a";

    FString DecodedStr = EOSString_Utf8StringForTests::FromUtf8String(Utf8Emoji);
    TestEqual("FromUtf8String decodes emoji correctly", FString(Utf16Emoji), DecodedStr);

    AddInfo(
        FString::Printf(TEXT("The result of FromUtf8String should render a smiling face emoji here: %s"), *DecodedStr));

    return true;
}

#if !defined(UE_5_0_OR_LATER)

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_Utf8String_ToUtf8String,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.Utf8String.ToUtf8String",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_Utf8String_ToUtf8String::RunTest(const FString &Parameters)
{
    {
        const char *Utf8Emoji = "\xf0\x9f\x98\x8a";
        const UTF8CHAR Utf8EmojiBytes[] = {0xf0, 0x9f, 0x98, 0x8a, 0x00};

        FString DecodedStr = EOSString_Utf8StringForTests::FromUtf8String(Utf8Emoji);
        EOSString_Utf8StringForTests::Utf8String ReencodedStr = EOSString_Utf8StringForTests::ToUtf8String(DecodedStr);

        TestEqual("ToUtf8String returns EOS_Success for valid string", ReencodedStr.Result, EOS_EResult::EOS_Success);

        TestTrue("ToUtf8String re-encodes emoji correctly (strcmp)", strcmp(ReencodedStr.GetAsChar(), Utf8Emoji) == 0);
        TestEqual("ToUtf8String re-encodes emoji correctly (byte compare length)", ReencodedStr.Ptr.Length(), 4);
        if (ReencodedStr.Ptr.Length() == 4)
        {
            for (int i = 0; i < 4; i++)
            {
                TestEqual(
                    FString::Printf(TEXT("ToUtf8String re-encodes emoji correctly (byte compare index %d)"), i),
                    (uint8_t)ReencodedStr.Ptr.Get()[i],
                    (uint8_t)Utf8EmojiBytes[i]);
            }
        }
    }

    {
        EOSString_Utf8StringForTests::Utf8String EmptyStr = EOSString_Utf8StringForTests::ToUtf8String("");
        TestEqual("ToUtf8String returns EOS_Success for empty string", EmptyStr.Result, EOS_EResult::EOS_Success);
    }

    {
        EOSString_Utf8StringForTests::Utf8String TooLongStr =
            EOSString_Utf8StringForTests::ToUtf8String("MoreThan32CharactersLong_12345678");
        TestEqual(
            "ToUtf8String returns EOS_LimitExceeded for string that is too long",
            TooLongStr.Result,
            EOS_EResult::EOS_LimitExceeded);
    }

    return true;
}

#endif // #if !defined(UE_5_0_OR_LATER)

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSString_Utf8String_AllocAndFree,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSString.Utf8String.AllocAndFree",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSString_Utf8String_AllocAndFree::RunTest(const FString &Parameters)
{
    FGenericMemoryStats MemoryStats;
    GMalloc->GetAllocatorStats(MemoryStats);
    int PrevAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int PrevFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int PrevDiff = PrevAllocateCalls - PrevFreeCalls;

    const char *Buffer = nullptr;

    {
        Buffer = nullptr;
        EOS_EResult EmptyAllocResult = EOSString_Utf8StringForTests::AllocateToCharBuffer(TEXT(""), Buffer);
        TestEqual(
            "AllocateToCharBuffer returns EOS_Success for empty string",
            EmptyAllocResult,
            EOS_EResult::EOS_Success);
        if (EmptyAllocResult == EOS_EResult::EOS_Success)
        {
            TestNotEqual("AllocateToCharBuffer with empty string returns non-nullptr", Buffer, (const char *)nullptr);
            TestTrue("AllocateToCharBuffer with empty string allocates empty buffer", strcmp(Buffer, "") == 0);

            EOSString_Utf8StringForTests::FreeFromCharBuffer(Buffer);
            TestEqual("FreeFromCharBuffer sets buffer back to nullptr", Buffer, (const char *)nullptr);
        }
    }

    {
        Buffer = nullptr;
        EOS_EResult TooLongAllocResult =
            EOSString_Utf8StringForTests::AllocateToCharBuffer(TEXT("MoreThan32CharactersLong_12345678"), Buffer);
        TestEqual(
            "AllocateToCharBuffer returns EOS_LimitExceeded for string too long",
            TooLongAllocResult,
            EOS_EResult::EOS_LimitExceeded);
        if (TooLongAllocResult == EOS_EResult::EOS_Success)
        {
            EOSString_Utf8StringForTests::FreeFromCharBuffer(Buffer);
        }
    }

    {
        Buffer = (const char *)1;
        EOS_EResult ExistingBufferAllocResult =
            EOSString_Utf8StringForTests::AllocateToCharBuffer(TEXT("Exactly32CharactersLong_12345678"), Buffer);
        TestEqual(
            "AllocateToCharBuffer returns EOS_InvalidParameters for existing buffer",
            ExistingBufferAllocResult,
            EOS_EResult::EOS_InvalidParameters);
        if (ExistingBufferAllocResult == EOS_EResult::EOS_Success)
        {
            EOSString_Utf8StringForTests::FreeFromCharBuffer(Buffer);
        }
    }

    {
        Buffer = nullptr;
        EOS_EResult ValidAllocResult =
            EOSString_Utf8StringForTests::AllocateToCharBuffer(TEXT("Exactly32CharactersLong_12345678"), Buffer);
        TestEqual(
            "AllocateToCharBuffer returns EOS_Success for valid string",
            ValidAllocResult,
            EOS_EResult::EOS_Success);
        if (ValidAllocResult == EOS_EResult::EOS_Success)
        {
            TestNotEqual("AllocateToCharBuffer with valid string returns non-nullptr", Buffer, (const char *)nullptr);
            TestTrue(
                "AllocateToCharBuffer with valid string allocates correct buffer",
                strcmp(Buffer, "Exactly32CharactersLong_12345678") == 0);

            EOSString_Utf8StringForTests::FreeFromCharBuffer(Buffer);
            TestEqual("FreeFromCharBuffer sets buffer back to nullptr", Buffer, (const char *)nullptr);
        }
    }

    GMalloc->GetAllocatorStats(MemoryStats);
    int NextAllocateCalls = MemoryStats.Data[TEXT("Malloc calls")];
    int NextFreeCalls = MemoryStats.Data[TEXT("Free calls")];
    int NextDiff = PrevAllocateCalls - PrevFreeCalls;

    TestEqual("All allocated memory was freed", PrevDiff, NextDiff);

    return true;
}