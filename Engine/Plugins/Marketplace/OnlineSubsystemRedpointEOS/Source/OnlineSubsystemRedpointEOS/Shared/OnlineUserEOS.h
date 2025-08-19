// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemTypes.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * Storage for all of the resolved user data once it has been fetched from EOS. Once this is constructed, it
 * owns the EOS structures and releases them when it is freed.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FEOSUserData
{
public:
    TMap<EOS_EExternalAccountType, EOS_Connect_ExternalAccountInfo *> ExternalInfo;
    TArray<TSharedRef<const FUniqueNetId>> ExternalIds;
    EOS_EExternalAccountType DefaultPlatformType;
    EOS_UserInfo *EpicAccountInfo;

    UE_NONCOPYABLE(FEOSUserData);
    FEOSUserData(
        const TArray<EOS_Connect_ExternalAccountInfo *> &InExternalInfo,
        const TArray<TSharedRef<const FUniqueNetId>> &InExternalIds,
        EOS_EExternalAccountType InDefaultPlatformType,
        EOS_UserInfo *InEpicAccountInfo = nullptr);
    ~FEOSUserData();
};

/**
 * Common APIs shared by all EOS implementations of online subsystem user objects. Used by FEOSUserFactory to inject the
 * resolved data once it's available.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API IEOSUser
{
    friend class FEOSUserFactory;
    friend class FEOSUserFactoryOperation;

private:
    TSharedPtr<const FEOSUserData> Data;
    void SetData(const TSharedRef<const FEOSUserData> &);

protected:
    /** Get the associated data; only callable if IsReady returns true. */
    const FEOSUserData &GetData() const;

public:
    virtual ~IEOSUser(){};

    /** Returns true if this user has it's data resolved. */
    bool IsReady() const;

    /** Returns the EOS user ID. */
    virtual TSharedRef<const FUniqueNetIdEOS> GetUserIdEOS() const = 0;

    /** Returns the wrapped IDs for other subsystems. */
    virtual TArray<TSharedRef<const FUniqueNetId>> GetExternalUserIds() const;
};

ONLINESUBSYSTEMREDPOINTEOS_API EOS_EExternalAccountType StrToExternalAccountType(const FString &InType);
ONLINESUBSYSTEMREDPOINTEOS_API FString ExternalAccountTypeToStr(EOS_EExternalAccountType InType);

/**
 * A factory for obtaining one or more FOnlineUserEOS or related classes, either immediately or waiting for data
 * resolution to complete.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FEOSUserFactory : public TSharedFromThis<FEOSUserFactory, ESPMode::ThreadSafe>
{
private:
    FName InstanceName;
    EOS_HPlatform Platform;
    TSharedRef<IEOSRuntimePlatform> RuntimePlatform;

    TUserIdMap<TSharedPtr<IEOSUser>> GetInternal(
        const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId,
        const TArray<TSharedRef<const FUniqueNetIdEOS>> &InUserIds,
        TSharedRef<IEOSUser> (*ConstructUser)(const TSharedRef<const FUniqueNetIdEOS> &InUserId),
        std::function<void(const TUserIdMap<TSharedPtr<IEOSUser>> &)> OnResolved);

public:
    UE_NONCOPYABLE(FEOSUserFactory);
    FEOSUserFactory() = delete;
    FEOSUserFactory(FName InInstanceName, EOS_HPlatform InPlatform, TSharedRef<IEOSRuntimePlatform> InRuntimePlatform)
        : InstanceName(InInstanceName)
        , Platform(InPlatform)
        , RuntimePlatform(MoveTemp(InRuntimePlatform)){};
    virtual ~FEOSUserFactory(){};

    TUserIdMap<TSharedPtr<IEOSUser>> GetUnresolved(
        const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId,
        const TArray<TSharedRef<const FUniqueNetIdEOS>> &InUserIds,
        TSharedRef<IEOSUser> (*ConstructUser)(const TSharedRef<const FUniqueNetIdEOS> &InUserId));
    void Get(
        const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId,
        const TArray<TSharedRef<const FUniqueNetIdEOS>> &InUserIds,
        TSharedRef<IEOSUser> (*ConstructUser)(const TSharedRef<const FUniqueNetIdEOS> &InUserId),
        std::function<void(const TUserIdMap<TSharedPtr<IEOSUser>> &)> OnResolved);
};

template <typename TDerivedClass, typename TBaseClass>
class TBaseUserImplementation : public TBaseClass, public TSharedFromThis<TDerivedClass>, public IEOSUser
{
private:
    TSharedPtr<const FUniqueNetIdEOS> UserId;

    static TSharedRef<IEOSUser> Construct(const TSharedRef<const FUniqueNetIdEOS> &InUserId)
    {
        TDerivedClass *Inst = new TDerivedClass();
        Inst->UserId = InUserId;
        return StaticCastSharedRef<IEOSUser>(TSharedRef<TDerivedClass>(MakeShareable(Inst)));
    }

protected:
    TBaseUserImplementation()
        : UserId(){};

public:
    UE_NONCOPYABLE(TBaseUserImplementation)
    ~TBaseUserImplementation(){};

    /**
     * Get a bunch of users of this type, without waiting for their online information to be ready.
     */
    static TUserIdMap<TSharedPtr<TDerivedClass>> GetUnresolved(
        const TSharedRef<FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory,
        const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId,
        const TArray<TSharedRef<const FUniqueNetIdEOS>> &InUserIds)
    {
        TUserIdMap<TSharedPtr<IEOSUser>> OriginalMap =
            InUserFactory->GetUnresolved(InQueryingUserId, InUserIds, &Construct);
        TUserIdMap<TSharedPtr<TDerivedClass>> NewMap;
        for (const auto &KV : OriginalMap)
        {
            NewMap.Add(*KV.Key, StaticCastSharedPtr<TDerivedClass>(KV.Value));
        }
        return NewMap;
    }

    /**
     * Get a bunch of users of this type, waiting for their online information to be ready.
     */
    static void Get(
        const TSharedRef<FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory,
        const TSharedRef<const FUniqueNetIdEOS> &InQueryingUserId,
        const TArray<TSharedRef<const FUniqueNetIdEOS>> &InUserIds,
        std::function<void(const TUserIdMap<TSharedPtr<TDerivedClass>> &)> OnResolved)
    {
        InUserFactory->Get(
            InQueryingUserId,
            InUserIds,
            &Construct,
            [OnResolved](const TUserIdMap<TSharedPtr<IEOSUser>> &UserIdMap) {
                TUserIdMap<TSharedPtr<TDerivedClass>> NewMap;
                for (const auto &KV : UserIdMap)
                {
                    NewMap.Add(*KV.Key, StaticCastSharedPtr<TDerivedClass>(KV.Value));
                }
                OnResolved(NewMap);
            });
    }

    static TSharedPtr<TDerivedClass> NewInvalid(const TSharedRef<const FUniqueNetIdEOS> &InUserId)
    {
        TSharedPtr<TDerivedClass> NewMember = MakeShareable(new TDerivedClass());
        NewMember->UserId = InUserId;
        // It's resolution never gets scheduled by FEOSUserFactory, so it will remain unready.
        return NewMember;
    }

    bool IsValid()
    {
        if (!this->IsReady())
        {
            // Still waiting on results.
            return true;
        }

        return !EOSString_ProductUserId::IsNone(this->UserId->GetProductUserId());
    }

    virtual TSharedRef<const FUniqueNetIdEOS> GetUserIdEOS() const override
    {
        return this->UserId.ToSharedRef();
    }

    TSharedRef<const FUniqueNetId> GetUserId() const override
    {
        return this->UserId.ToSharedRef();
    }

    FString GetRealName() const override
    {
        return TEXT("");
    }

    FString GetDisplayName(const FString &Platform = FString()) const override
    {
        if (!this->IsReady())
        {
            return TEXT("");
        }

        EOS_EExternalAccountType DefaultPlatformType = this->GetData().DefaultPlatformType;
        if (DefaultPlatformType == (EOS_EExternalAccountType)-1)
        {
            // If we don't have a default platform type, it's because we don't have any platforms associated with this
            // account at all.
            return TEXT("");
        }

        return EOSString_Connect_UserLoginInfo_DisplayName::FromUtf8String(
            this->GetData().ExternalInfo[DefaultPlatformType]->DisplayName);
    }

    bool GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const override
    {
        if (AttrName == USER_ATTR_ID)
        {
            OutAttrValue = this->UserId->ToString();
            return true;
        }
        else if (AttrName == TEXT("ready"))
        {
            OutAttrValue = this->IsReady() ? TEXT("true") : TEXT("false");
            return true;
        }
        else if (AttrName == TEXT("productUserId"))
        {
            OutAttrValue = this->UserId->GetProductUserIdString();
            return true;
        }
        else if (AttrName == USER_ATTR_DISPLAYNAME || AttrName == USER_ATTR_PREFERRED_DISPLAYNAME)
        {
            OutAttrValue = this->GetDisplayName();
            return OutAttrValue != TEXT("");
        }
        else if (AttrName == TEXT("externalAccountTypes") && this->IsReady())
        {
            TArray<FString> Types;
            for (auto KV : this->GetData().ExternalInfo)
            {
                FString ExternalAccountType = ExternalAccountTypeToStr(KV.Key);
                if (!ExternalAccountType.IsEmpty())
                {
                    Types.Add(ExternalAccountType.ToLower());
                }
            }
            OutAttrValue = FString::Join(Types, TEXT(","));
            return true;
        }
        else if (AttrName.StartsWith(TEXT("externalAccount.")) && this->IsReady())
        {
            FString TrimmedAttributeName = AttrName.Mid(FString(TEXT("externalAccount.")).Len());
            int NextAttributeIndex = TrimmedAttributeName.Find(TEXT("."));
            if (NextAttributeIndex == -1)
            {
                // Expected a sub-attribute.
                return false;
            }
            FString ExternalAccountType = TrimmedAttributeName.Mid(0, NextAttributeIndex);
            FString SubAttributeName = TrimmedAttributeName.Mid(NextAttributeIndex + 1);
            EOS_EExternalAccountType ExternalType = StrToExternalAccountType(ExternalAccountType);
            if (ExternalType == (EOS_EExternalAccountType)-1)
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("Invalid external account provider in external account attribute: '%s'."),
                    *ExternalAccountType);
                return false;
            }

            if (!this->GetData().ExternalInfo.Contains(ExternalType))
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("No external account data for platform on this user: '%s'. Use the 'externalAccountTypes' "
                         "attribute to determine the available external platforms before trying to access user "
                         "attributes."),
                    *ExternalAccountType);
                return false;
            }

            EOS_Connect_ExternalAccountInfo *Info = this->GetData().ExternalInfo[ExternalType];

            if (SubAttributeName == TEXT("id"))
            {
                OutAttrValue = EOSString_Connect_ExternalAccountId::FromAnsiString(Info->AccountId);
                return true;
            }
            else if (SubAttributeName == TEXT("displayName"))
            {
                if (ExternalType == EOS_EExternalAccountType::EOS_EAT_EPIC &&
                    this->GetData().EpicAccountInfo != nullptr)
                {
                    if (this->GetData().EpicAccountInfo->DisplayName != nullptr)
                    {
                        // Prefer the Epic level display name over the EOS Connect value.
#if EOS_VERSION_AT_LEAST(1, 15, 0)
                        OutAttrValue = EOSString_UserInfo_DisplayName::FromUtf8String(
                            this->GetData().EpicAccountInfo->DisplayNameSanitized);
#else
                        OutAttrValue = EOSString_UserInfo_DisplayName::FromUtf8String(
                            this->GetData().EpicAccountInfo->DisplayName);
#endif
                        return true;
                    }
                }

                if (Info->DisplayName == nullptr)
                {
                    // This platform didn't provide a display name.
                    return false;
                }

                OutAttrValue = EOSString_Connect_UserLoginInfo_DisplayName::FromUtf8String(Info->DisplayName);
                return true;
            }
            else if (SubAttributeName == TEXT("lastLoginTime.unixTimestampUtc"))
            {
                if (Info->LastLoginTime == EOS_CONNECT_TIME_UNDEFINED)
                {
                    // The user hasn't logged into this platform.
                    return false;
                }

                OutAttrValue = FString::Printf(TEXT("%lld"), Info->LastLoginTime);
                return true;
            }
            else if (
                ExternalType == EOS_EExternalAccountType::EOS_EAT_EPIC && this->GetData().EpicAccountInfo != nullptr)
            {
                if (SubAttributeName == TEXT("country"))
                {
                    if (this->GetData().EpicAccountInfo->Country == nullptr)
                    {
                        // There is no country information.
                        return false;
                    }

                    // There is no maximum defined for country, but assume it can be up to 64 characters.
                    OutAttrValue =
                        EOSString_UserInfo_DisplayName::FromUtf8String(this->GetData().EpicAccountInfo->Country);
                    return true;
                }
                else if (SubAttributeName == TEXT("nickname"))
                {
                    if (this->GetData().EpicAccountInfo->Nickname == nullptr)
                    {
                        // There is no nickname set.
                        return false;
                    }

                    // There is no maximum defined for nickname, but assume it can be the same length as the display
                    // name.
                    OutAttrValue =
                        EOSString_UserInfo_DisplayName::FromUtf8String(this->GetData().EpicAccountInfo->Nickname);
                    return true;
                }
                else if (SubAttributeName == TEXT("preferredLanguage"))
                {
                    if (this->GetData().EpicAccountInfo->PreferredLanguage == nullptr)
                    {
                        // There is no preferred language set.
                        return false;
                    }

                    // There is no maximum defined for preferred language, but assume it can be the same length as the
                    // display name.
                    OutAttrValue = EOSString_UserInfo_DisplayName::FromUtf8String(
                        this->GetData().EpicAccountInfo->PreferredLanguage);
                    return true;
                }
            }
        }
        else if (AttrName == TEXT("epicAccountId"))
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("The 'epicAccountId' user attribute is deprecated. Use 'externalAccount.epic.id' instead, after "
                     "checking that Epic is a connected account type in 'externalAccountTypes'."));
            return GetUserAttribute(TEXT("externalAccount.epic.id"), OutAttrValue);
        }
        else if (AttrName == "country")
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("The 'country' user attribute is deprecated. Use 'externalAccount.epic.country' instead, after "
                     "checking that Epic is a connected account type in 'externalAccountTypes'."));
            return GetUserAttribute(TEXT("externalAccount.epic.country"), OutAttrValue);
        }

        return false;
    }
};

class ONLINESUBSYSTEMREDPOINTEOS_API FUserOnlineAccountEOS
    : public TBaseUserImplementation<FUserOnlineAccountEOS, FUserOnlineAccount>
{
    friend class TBaseUserImplementation<FUserOnlineAccountEOS, FUserOnlineAccount>;
    friend class FOnlineIdentityInterfaceEOS;

private:
    FUserOnlineAccountEOS()
        : TBaseUserImplementation<FUserOnlineAccountEOS, FUserOnlineAccount>(){};

    TWeakPtr<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity;
    TMap<FString, FString> AuthAttributes;

public:
    UE_NONCOPYABLE(FUserOnlineAccountEOS);

    virtual FString GetAccessToken() const override;
    virtual bool GetAuthAttribute(const FString &AttrName, FString &OutAttrValue) const override;
    virtual bool SetUserAttribute(const FString &AttrName, const FString &AttrValue) override;
};

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineUserEOS : public TBaseUserImplementation<FOnlineUserEOS, FOnlineUser>
{
    friend class TBaseUserImplementation<FOnlineUserEOS, FOnlineUser>;

private:
    FOnlineUserEOS()
        : TBaseUserImplementation<FOnlineUserEOS, FOnlineUser>(){};

public:
    UE_NONCOPYABLE(FOnlineUserEOS);
};

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineRecentPlayerEOS
    : public TBaseUserImplementation<FOnlineRecentPlayerEOS, FOnlineRecentPlayer>
{
    friend class TBaseUserImplementation<FOnlineRecentPlayerEOS, FOnlineRecentPlayer>;

private:
    FOnlineRecentPlayerEOS();

public:
    UE_NONCOPYABLE(FOnlineRecentPlayerEOS);

    FDateTime LastSeen;
    virtual FDateTime GetLastSeen() const override;
};

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineBlockedPlayerEOS
    : public TBaseUserImplementation<FOnlineBlockedPlayerEOS, FOnlineBlockedPlayer>
{
    friend class TBaseUserImplementation<FOnlineBlockedPlayerEOS, FOnlineBlockedPlayer>;

private:
    FOnlineBlockedPlayerEOS()
        : TBaseUserImplementation<FOnlineBlockedPlayerEOS, FOnlineBlockedPlayer>(){};

public:
    UE_NONCOPYABLE(FOnlineBlockedPlayerEOS);
};

EOS_DISABLE_STRICT_WARNINGS