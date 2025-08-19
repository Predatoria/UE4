// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"

const FOnlineError &ConvertError(EOS_EResult Result)
{
    switch (Result)
    {
    case EOS_EResult::EOS_Success:
        return OnlineRedpointEOS::Errors::Success();
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnexpectedError:
        return OnlineRedpointEOS::Errors::UnexpectedError();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoConnection:
        return OnlineRedpointEOS::Errors::NoConnection();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidParameters:
        return OnlineRedpointEOS::Errors::InvalidParameters();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidRequest:
        return OnlineRedpointEOS::Errors::InvalidRequest();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnrecognizedResponse:
        return OnlineRedpointEOS::Errors::UnrecognizedResponse();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_IncompatibleVersion:
        return OnlineRedpointEOS::Errors::IncompatibleVersion();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotConfigured:
        return OnlineRedpointEOS::Errors::NotConfigured();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyConfigured:
        return OnlineRedpointEOS::Errors::AlreadyConfigured();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotImplemented:
        return OnlineRedpointEOS::Errors::NotImplemented();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Canceled:
        return OnlineRedpointEOS::Errors::Canceled();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotFound:
        return OnlineRedpointEOS::Errors::NotFound();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredPatchAvailable:
        return OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredSystemUpdate:
        return OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AgeRestrictionFailure:
        return OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AccountTypeFailure:
        return OnlineRedpointEOS::Errors::Permission_AccountTypeFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_ChatRestriction:
        return OnlineRedpointEOS::Errors::Permission_ChatRestriction();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_UGCRestriction:
        return OnlineRedpointEOS::Errors::Permission_UGCRestriction();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_OnlinePlayRestricted:
        return OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_OperationWillRetry:
        return OnlineRedpointEOS::Errors::OperationWillRetry();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ApplicationNotBootstrapped:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotInstalled:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceStartFailed:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotRunning:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidCredentials:
        return OnlineRedpointEOS::Errors::InvalidCredentials();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoChange:
        return OnlineRedpointEOS::Errors::NoChange();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_VersionMismatch:
        return OnlineRedpointEOS::Errors::VersionMismatch();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_LimitExceeded:
        return OnlineRedpointEOS::Errors::LimitExceeded();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Disabled:
        return OnlineRedpointEOS::Errors::Disabled();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_DuplicateNotAllowed:
        return OnlineRedpointEOS::Errors::DuplicateNotAllowed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingParameters_DEPRECATED:
        return OnlineRedpointEOS::Errors::MissingParameters();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidSandboxId:
        return OnlineRedpointEOS::Errors::InvalidSandboxId();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TimedOut:
        return OnlineRedpointEOS::Errors::TimedOut();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PartialResult:
        return OnlineRedpointEOS::Errors::PartialResult();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Role:
        return OnlineRedpointEOS::Errors::Missing_Role();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidUser:
        return OnlineRedpointEOS::Errors::InvalidUser();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Feature:
        return OnlineRedpointEOS::Errors::Missing_Feature();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Sandbox:
        return OnlineRedpointEOS::Errors::Invalid_Sandbox();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Deployment:
        return OnlineRedpointEOS::Errors::Invalid_Deployment();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Product:
        return OnlineRedpointEOS::Errors::Invalid_Product();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_ProductUserID:
        return OnlineRedpointEOS::Errors::Invalid_ProductUserID();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_ServiceFailure:
        return OnlineRedpointEOS::Errors::ServiceFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryMissing:
        return OnlineRedpointEOS::Errors::CacheDirectoryMissing();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryInvalid:
        return OnlineRedpointEOS::Errors::CacheDirectoryInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidState:
        return OnlineRedpointEOS::Errors::InvalidState();
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_RequestInProgress:
        return OnlineRedpointEOS::Errors::RequestInProgress();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidAuth:
        return OnlineRedpointEOS::Errors::InvalidAuth();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_ApplicationSuspended:
        return OnlineRedpointEOS::Errors::ApplicationSuspended();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_NetworkDisconnected:
        return OnlineRedpointEOS::Errors::NetworkDisconnected();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AccessDenied:
        return OnlineRedpointEOS::Errors::AccessDenied();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingPermissions:
        return OnlineRedpointEOS::Errors::MissingPermissions();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Token_Not_Account:
        return OnlineRedpointEOS::Errors::Token_Not_Account();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TooManyRequests:
        return OnlineRedpointEOS::Errors::TooManyRequests();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyPending:
        return OnlineRedpointEOS::Errors::AlreadyPending();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLocked:
        return OnlineRedpointEOS::Errors::Auth::AccountLocked();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLockedForUpdate:
        return OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidRefreshToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidToken();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AuthenticationFailure:
        return OnlineRedpointEOS::Errors::Auth::AuthenticationFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidPlatformToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongAccount:
        return OnlineRedpointEOS::Errors::Auth::WrongAccount();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongClient:
        return OnlineRedpointEOS::Errors::Auth::WrongClient();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_FullAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::FullAccountRequired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_HeadlessAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordResetRequired:
        return OnlineRedpointEOS::Errors::Auth::PasswordResetRequired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordCannotBeReused:
        return OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_Expired:
        return OnlineRedpointEOS::Errors::Auth::Expired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeConsentRequired:
        return OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ApplicationNotFound:
        return OnlineRedpointEOS::Errors::Auth::ApplicationNotFound();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ScopeNotFound();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountFeatureRestricted:
        return OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountPortalLoadError:
        return OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_CorrectiveActionRequired:
        return OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantCode:
        return OnlineRedpointEOS::Errors::Auth::PinGrantCode();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantExpired:
        return OnlineRedpointEOS::Errors::Auth::PinGrantExpired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantPending:
        return OnlineRedpointEOS::Errors::Auth::PinGrantPending();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthNotLinked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRevoked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthInvalid:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRestricted:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthCannotLogin:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthExpired:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthIsLastLoginType:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExchangeCodeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_OriginatingExchangeCodeSessionExpired:
        return OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 14, 2)
    case EOS_EResult::EOS_Auth_PersistentAuth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_MFARequired:
        return OnlineRedpointEOS::Errors::Auth::MFARequired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ParentalControls:
        return OnlineRedpointEOS::Errors::Auth::ParentalControls();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_NoRealId:
        return OnlineRedpointEOS::Errors::Auth::NoRealId();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_InviteAwaitingAcceptance:
        return OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NoInvitation:
        return OnlineRedpointEOS::Errors::Friends::NoInvitation();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_AlreadyFriends:
        return OnlineRedpointEOS::Errors::Friends::AlreadyFriends();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NotFriends:
        return OnlineRedpointEOS::Errors::Friends::NotFriends();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataLengthInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_StatusInvalid:
        return OnlineRedpointEOS::Errors::Presence::StatusInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_EntitlementStale:
        return OnlineRedpointEOS::Errors::Ecom::EntitlementStale();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogItemStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogItemStale();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferPriceInvalid:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CheckoutLoadError:
        return OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionInProgress:
        return OnlineRedpointEOS::Errors::Sessions::SessionInProgress();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Sessions::TooManyPlayers();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NoPermission:
        return OnlineRedpointEOS::Errors::Sessions::NoPermission();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionAlreadyExists:
        return OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidLock:
        return OnlineRedpointEOS::Errors::Sessions::InvalidLock();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidSession:
        return OnlineRedpointEOS::Errors::Sessions::InvalidSession();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteFailed:
        return OnlineRedpointEOS::Errors::Sessions::InviteFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteNotFound:
        return OnlineRedpointEOS::Errors::Sessions::InviteNotFound();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_AggregationFailed:
        return OnlineRedpointEOS::Errors::Sessions::AggregationFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::HostAtCapacity();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionNotAnonymous:
        return OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_OutOfSync:
        return OnlineRedpointEOS::Errors::Sessions::OutOfSync();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyInvites:
        return OnlineRedpointEOS::Errors::Sessions::TooManyInvites();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_PresenceSessionExists:
        return OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::NotAllowed();
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 2)
    case EOS_EResult::EOS_Sessions_PlayerSanctioned:
        return OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalidChars:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeTooLarge:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHandleInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_StartIndexInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_RequestInProgress:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserThrottled:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalTokenValidationFailed:
        return OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UserAlreadyExists:
        return OnlineRedpointEOS::Errors::Connect::UserAlreadyExists();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_AuthExpired:
        return OnlineRedpointEOS::Errors::Connect::AuthExpired();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_InvalidToken:
        return OnlineRedpointEOS::Errors::Connect::InvalidToken();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UnsupportedTokenType:
        return OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailed:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceUnavailable:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceConfigurationFailure:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 8, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount_DEPRECATED:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UI_SocialOverlayLoadError:
        return OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotOwner:
        return OnlineRedpointEOS::Errors::Lobby::NotOwner();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidLock:
        return OnlineRedpointEOS::Errors::Lobby::InvalidLock();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_LobbyAlreadyExists:
        return OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SessionInProgress:
        return OnlineRedpointEOS::Errors::Lobby::SessionInProgress();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Lobby::TooManyPlayers();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NoPermission:
        return OnlineRedpointEOS::Errors::Lobby::NoPermission();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidSession:
        return OnlineRedpointEOS::Errors::Lobby::InvalidSession();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteFailed:
        return OnlineRedpointEOS::Errors::Lobby::InviteFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteNotFound:
        return OnlineRedpointEOS::Errors::Lobby::InviteNotFound();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_AggregationFailed:
        return OnlineRedpointEOS::Errors::Lobby::AggregationFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::HostAtCapacity();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyInvites:
        return OnlineRedpointEOS::Errors::Lobby::TooManyInvites();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::NotAllowed();
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_MemberUpdateOnly:
        return OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly();
#endif
#if EOS_VERSION_AT_LEAST(1, 7, 0)
    case EOS_EResult::EOS_Lobby_PresenceLobbyExists:
        return OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Lobby_VoiceNotEnabled:
        return OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled();
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback();
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet();
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted();
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessIsAlreadyRunning:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkCommandIsEmpty:
        return OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessCreationFailed:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CriticalError:
        return OnlineRedpointEOS::Errors::Mods::CriticalError();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ToolInternalError:
        return OnlineRedpointEOS::Errors::Mods::ToolInternalError();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_IPCFailure:
        return OnlineRedpointEOS::Errors::Mods::IPCFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidIPCResponse:
        return OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_URILaunchFailure:
        return OnlineRedpointEOS::Errors::Mods::URILaunchFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModIsNotInstalled:
        return OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UserDoesNotOwnTheGame:
        return OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdInvalidResult:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CouldNotFindOffer:
        return OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdFailure:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_PurchaseFailure:
        return OnlineRedpointEOS::Errors::Mods::PurchaseFailure();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidGameInstallInfo:
        return OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CannotGetManifestLocation:
        return OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation();
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UnsupportedOS:
        return OnlineRedpointEOS::Errors::Mods::UnsupportedOS();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProtectionNotAvailable:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_InvalidMode:
        return OnlineRedpointEOS::Errors::AntiCheat::InvalidMode();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProductIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientSandboxIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageSessionKeyRequired:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageValidationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageInitializationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerAlreadyRegistered:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotFound:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotProtected:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected();
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_ClientDeploymentIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch();
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_DeviceIdAuthIsNotSupported:
        return OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_TooManyParticipants:
        return OnlineRedpointEOS::Errors::RTC::TooManyParticipants();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomAlreadyExists:
        return OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserKicked:
        return OnlineRedpointEOS::Errors::RTC::UserKicked();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserBanned:
        return OnlineRedpointEOS::Errors::RTC::UserBanned();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomWasLeft:
        return OnlineRedpointEOS::Errors::RTC::RoomWasLeft();
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    case EOS_EResult::EOS_RTC_ReconnectionTimegateExpired:
        return OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_ShutdownInvoked:
        return OnlineRedpointEOS::Errors::RTC::ShutdownInvoked();
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_UserIsInBlocklist:
        return OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist();
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_ProgressionSnapshot_SnapshotIdUnavailable:
        return OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable();
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_KWS_ParentEmailMissing:
        return OnlineRedpointEOS::Errors::KWS::ParentEmailMissing();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_KWS_UserGraduated:
        return OnlineRedpointEOS::Errors::KWS::UserGraduated();
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_Android_JavaVMNotStored:
        return OnlineRedpointEOS::Errors::Android::JavaVMNotStored();
#endif
    default:
        return OnlineRedpointEOS::Errors::UnexpectedError();
    }
}

const FOnlineError ConvertError(const TCHAR* InCall, const FString& InContextMsg, EOS_EResult Result)
{
    switch (Result)
    {
    case EOS_EResult::EOS_Success:
        return OnlineRedpointEOS::Errors::Success();
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnexpectedError:
        return OnlineRedpointEOS::Errors::UnexpectedError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoConnection:
        return OnlineRedpointEOS::Errors::NoConnection(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidParameters:
        return OnlineRedpointEOS::Errors::InvalidParameters(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidRequest:
        return OnlineRedpointEOS::Errors::InvalidRequest(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnrecognizedResponse:
        return OnlineRedpointEOS::Errors::UnrecognizedResponse(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_IncompatibleVersion:
        return OnlineRedpointEOS::Errors::IncompatibleVersion(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotConfigured:
        return OnlineRedpointEOS::Errors::NotConfigured(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyConfigured:
        return OnlineRedpointEOS::Errors::AlreadyConfigured(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotImplemented:
        return OnlineRedpointEOS::Errors::NotImplemented(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Canceled:
        return OnlineRedpointEOS::Errors::Canceled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotFound:
        return OnlineRedpointEOS::Errors::NotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredPatchAvailable:
        return OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredSystemUpdate:
        return OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AgeRestrictionFailure:
        return OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AccountTypeFailure:
        return OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_ChatRestriction:
        return OnlineRedpointEOS::Errors::Permission_ChatRestriction(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_UGCRestriction:
        return OnlineRedpointEOS::Errors::Permission_UGCRestriction(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_OnlinePlayRestricted:
        return OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_OperationWillRetry:
        return OnlineRedpointEOS::Errors::OperationWillRetry(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ApplicationNotBootstrapped:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotInstalled:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceStartFailed:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotRunning:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidCredentials:
        return OnlineRedpointEOS::Errors::InvalidCredentials(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoChange:
        return OnlineRedpointEOS::Errors::NoChange(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_VersionMismatch:
        return OnlineRedpointEOS::Errors::VersionMismatch(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_LimitExceeded:
        return OnlineRedpointEOS::Errors::LimitExceeded(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Disabled:
        return OnlineRedpointEOS::Errors::Disabled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_DuplicateNotAllowed:
        return OnlineRedpointEOS::Errors::DuplicateNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingParameters_DEPRECATED:
        return OnlineRedpointEOS::Errors::MissingParameters(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidSandboxId:
        return OnlineRedpointEOS::Errors::InvalidSandboxId(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TimedOut:
        return OnlineRedpointEOS::Errors::TimedOut(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PartialResult:
        return OnlineRedpointEOS::Errors::PartialResult(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Role:
        return OnlineRedpointEOS::Errors::Missing_Role(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidUser:
        return OnlineRedpointEOS::Errors::InvalidUser(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Feature:
        return OnlineRedpointEOS::Errors::Missing_Feature(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Sandbox:
        return OnlineRedpointEOS::Errors::Invalid_Sandbox(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Deployment:
        return OnlineRedpointEOS::Errors::Invalid_Deployment(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Product:
        return OnlineRedpointEOS::Errors::Invalid_Product(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_ProductUserID:
        return OnlineRedpointEOS::Errors::Invalid_ProductUserID(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_ServiceFailure:
        return OnlineRedpointEOS::Errors::ServiceFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryMissing:
        return OnlineRedpointEOS::Errors::CacheDirectoryMissing(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryInvalid:
        return OnlineRedpointEOS::Errors::CacheDirectoryInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidState:
        return OnlineRedpointEOS::Errors::InvalidState(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_RequestInProgress:
        return OnlineRedpointEOS::Errors::RequestInProgress(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidAuth:
        return OnlineRedpointEOS::Errors::InvalidAuth(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_ApplicationSuspended:
        return OnlineRedpointEOS::Errors::ApplicationSuspended(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_NetworkDisconnected:
        return OnlineRedpointEOS::Errors::NetworkDisconnected(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AccessDenied:
        return OnlineRedpointEOS::Errors::AccessDenied(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingPermissions:
        return OnlineRedpointEOS::Errors::MissingPermissions(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Token_Not_Account:
        return OnlineRedpointEOS::Errors::Token_Not_Account(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TooManyRequests:
        return OnlineRedpointEOS::Errors::TooManyRequests(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyPending:
        return OnlineRedpointEOS::Errors::AlreadyPending(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLocked:
        return OnlineRedpointEOS::Errors::Auth::AccountLocked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLockedForUpdate:
        return OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidRefreshToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidToken(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AuthenticationFailure:
        return OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidPlatformToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongAccount:
        return OnlineRedpointEOS::Errors::Auth::WrongAccount(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongClient:
        return OnlineRedpointEOS::Errors::Auth::WrongClient(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_FullAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::FullAccountRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_HeadlessAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordResetRequired:
        return OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordCannotBeReused:
        return OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_Expired:
        return OnlineRedpointEOS::Errors::Auth::Expired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeConsentRequired:
        return OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ApplicationNotFound:
        return OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ScopeNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountFeatureRestricted:
        return OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountPortalLoadError:
        return OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_CorrectiveActionRequired:
        return OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantCode:
        return OnlineRedpointEOS::Errors::Auth::PinGrantCode(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantExpired:
        return OnlineRedpointEOS::Errors::Auth::PinGrantExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantPending:
        return OnlineRedpointEOS::Errors::Auth::PinGrantPending(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthNotLinked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRevoked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthInvalid:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRestricted:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthCannotLogin:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthExpired:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthIsLastLoginType:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExchangeCodeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_OriginatingExchangeCodeSessionExpired:
        return OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 14, 2)
    case EOS_EResult::EOS_Auth_PersistentAuth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_MFARequired:
        return OnlineRedpointEOS::Errors::Auth::MFARequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ParentalControls:
        return OnlineRedpointEOS::Errors::Auth::ParentalControls(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_NoRealId:
        return OnlineRedpointEOS::Errors::Auth::NoRealId(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_InviteAwaitingAcceptance:
        return OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NoInvitation:
        return OnlineRedpointEOS::Errors::Friends::NoInvitation(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_AlreadyFriends:
        return OnlineRedpointEOS::Errors::Friends::AlreadyFriends(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NotFriends:
        return OnlineRedpointEOS::Errors::Friends::NotFriends(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_StatusInvalid:
        return OnlineRedpointEOS::Errors::Presence::StatusInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_EntitlementStale:
        return OnlineRedpointEOS::Errors::Ecom::EntitlementStale(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogItemStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferPriceInvalid:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CheckoutLoadError:
        return OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionInProgress:
        return OnlineRedpointEOS::Errors::Sessions::SessionInProgress(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NoPermission:
        return OnlineRedpointEOS::Errors::Sessions::NoPermission(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionAlreadyExists:
        return OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidLock:
        return OnlineRedpointEOS::Errors::Sessions::InvalidLock(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidSession:
        return OnlineRedpointEOS::Errors::Sessions::InvalidSession(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteFailed:
        return OnlineRedpointEOS::Errors::Sessions::InviteFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteNotFound:
        return OnlineRedpointEOS::Errors::Sessions::InviteNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_AggregationFailed:
        return OnlineRedpointEOS::Errors::Sessions::AggregationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionNotAnonymous:
        return OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_OutOfSync:
        return OnlineRedpointEOS::Errors::Sessions::OutOfSync(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyInvites:
        return OnlineRedpointEOS::Errors::Sessions::TooManyInvites(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_PresenceSessionExists:
        return OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::NotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 2)
    case EOS_EResult::EOS_Sessions_PlayerSanctioned:
        return OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalidChars:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeTooLarge:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHandleInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_StartIndexInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_RequestInProgress:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserThrottled:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalTokenValidationFailed:
        return OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UserAlreadyExists:
        return OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_AuthExpired:
        return OnlineRedpointEOS::Errors::Connect::AuthExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_InvalidToken:
        return OnlineRedpointEOS::Errors::Connect::InvalidToken(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UnsupportedTokenType:
        return OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailed:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceUnavailable:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceConfigurationFailure:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 8, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount_DEPRECATED:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UI_SocialOverlayLoadError:
        return OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotOwner:
        return OnlineRedpointEOS::Errors::Lobby::NotOwner(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidLock:
        return OnlineRedpointEOS::Errors::Lobby::InvalidLock(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_LobbyAlreadyExists:
        return OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SessionInProgress:
        return OnlineRedpointEOS::Errors::Lobby::SessionInProgress(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NoPermission:
        return OnlineRedpointEOS::Errors::Lobby::NoPermission(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidSession:
        return OnlineRedpointEOS::Errors::Lobby::InvalidSession(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteFailed:
        return OnlineRedpointEOS::Errors::Lobby::InviteFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteNotFound:
        return OnlineRedpointEOS::Errors::Lobby::InviteNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_AggregationFailed:
        return OnlineRedpointEOS::Errors::Lobby::AggregationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyInvites:
        return OnlineRedpointEOS::Errors::Lobby::TooManyInvites(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::NotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_MemberUpdateOnly:
        return OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 7, 0)
    case EOS_EResult::EOS_Lobby_PresenceLobbyExists:
        return OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Lobby_VoiceNotEnabled:
        return OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessIsAlreadyRunning:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkCommandIsEmpty:
        return OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessCreationFailed:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CriticalError:
        return OnlineRedpointEOS::Errors::Mods::CriticalError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ToolInternalError:
        return OnlineRedpointEOS::Errors::Mods::ToolInternalError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_IPCFailure:
        return OnlineRedpointEOS::Errors::Mods::IPCFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidIPCResponse:
        return OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_URILaunchFailure:
        return OnlineRedpointEOS::Errors::Mods::URILaunchFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModIsNotInstalled:
        return OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UserDoesNotOwnTheGame:
        return OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdInvalidResult:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CouldNotFindOffer:
        return OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdFailure:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_PurchaseFailure:
        return OnlineRedpointEOS::Errors::Mods::PurchaseFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidGameInstallInfo:
        return OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CannotGetManifestLocation:
        return OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UnsupportedOS:
        return OnlineRedpointEOS::Errors::Mods::UnsupportedOS(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProtectionNotAvailable:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_InvalidMode:
        return OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProductIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientSandboxIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageSessionKeyRequired:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageValidationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageInitializationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerAlreadyRegistered:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotFound:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotProtected:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_ClientDeploymentIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_DeviceIdAuthIsNotSupported:
        return OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_TooManyParticipants:
        return OnlineRedpointEOS::Errors::RTC::TooManyParticipants(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomAlreadyExists:
        return OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserKicked:
        return OnlineRedpointEOS::Errors::RTC::UserKicked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserBanned:
        return OnlineRedpointEOS::Errors::RTC::UserBanned(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomWasLeft:
        return OnlineRedpointEOS::Errors::RTC::RoomWasLeft(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    case EOS_EResult::EOS_RTC_ReconnectionTimegateExpired:
        return OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_ShutdownInvoked:
        return OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_UserIsInBlocklist:
        return OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_ProgressionSnapshot_SnapshotIdUnavailable:
        return OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_KWS_ParentEmailMissing:
        return OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_KWS_UserGraduated:
        return OnlineRedpointEOS::Errors::KWS::UserGraduated(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_Android_JavaVMNotStored:
        return OnlineRedpointEOS::Errors::Android::JavaVMNotStored(InCall, InContextMsg);
#endif

    default:
        return OnlineRedpointEOS::Errors::UnexpectedError(InCall, InContextMsg);
    }
}

const FOnlineError ConvertError(const TCHAR* InCall, const TCHAR* InContextMsg, EOS_EResult Result)
{
    switch (Result)
    {
    case EOS_EResult::EOS_Success:
        return OnlineRedpointEOS::Errors::Success();
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnexpectedError:
        return OnlineRedpointEOS::Errors::UnexpectedError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoConnection:
        return OnlineRedpointEOS::Errors::NoConnection(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidParameters:
        return OnlineRedpointEOS::Errors::InvalidParameters(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidRequest:
        return OnlineRedpointEOS::Errors::InvalidRequest(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnrecognizedResponse:
        return OnlineRedpointEOS::Errors::UnrecognizedResponse(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_IncompatibleVersion:
        return OnlineRedpointEOS::Errors::IncompatibleVersion(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotConfigured:
        return OnlineRedpointEOS::Errors::NotConfigured(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyConfigured:
        return OnlineRedpointEOS::Errors::AlreadyConfigured(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotImplemented:
        return OnlineRedpointEOS::Errors::NotImplemented(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Canceled:
        return OnlineRedpointEOS::Errors::Canceled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotFound:
        return OnlineRedpointEOS::Errors::NotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredPatchAvailable:
        return OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredSystemUpdate:
        return OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AgeRestrictionFailure:
        return OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AccountTypeFailure:
        return OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_ChatRestriction:
        return OnlineRedpointEOS::Errors::Permission_ChatRestriction(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_UGCRestriction:
        return OnlineRedpointEOS::Errors::Permission_UGCRestriction(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_OnlinePlayRestricted:
        return OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_OperationWillRetry:
        return OnlineRedpointEOS::Errors::OperationWillRetry(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ApplicationNotBootstrapped:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotInstalled:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceStartFailed:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotRunning:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidCredentials:
        return OnlineRedpointEOS::Errors::InvalidCredentials(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoChange:
        return OnlineRedpointEOS::Errors::NoChange(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_VersionMismatch:
        return OnlineRedpointEOS::Errors::VersionMismatch(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_LimitExceeded:
        return OnlineRedpointEOS::Errors::LimitExceeded(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Disabled:
        return OnlineRedpointEOS::Errors::Disabled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_DuplicateNotAllowed:
        return OnlineRedpointEOS::Errors::DuplicateNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingParameters_DEPRECATED:
        return OnlineRedpointEOS::Errors::MissingParameters(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidSandboxId:
        return OnlineRedpointEOS::Errors::InvalidSandboxId(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TimedOut:
        return OnlineRedpointEOS::Errors::TimedOut(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PartialResult:
        return OnlineRedpointEOS::Errors::PartialResult(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Role:
        return OnlineRedpointEOS::Errors::Missing_Role(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidUser:
        return OnlineRedpointEOS::Errors::InvalidUser(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Feature:
        return OnlineRedpointEOS::Errors::Missing_Feature(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Sandbox:
        return OnlineRedpointEOS::Errors::Invalid_Sandbox(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Deployment:
        return OnlineRedpointEOS::Errors::Invalid_Deployment(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Product:
        return OnlineRedpointEOS::Errors::Invalid_Product(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_ProductUserID:
        return OnlineRedpointEOS::Errors::Invalid_ProductUserID(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_ServiceFailure:
        return OnlineRedpointEOS::Errors::ServiceFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryMissing:
        return OnlineRedpointEOS::Errors::CacheDirectoryMissing(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryInvalid:
        return OnlineRedpointEOS::Errors::CacheDirectoryInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidState:
        return OnlineRedpointEOS::Errors::InvalidState(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_RequestInProgress:
        return OnlineRedpointEOS::Errors::RequestInProgress(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidAuth:
        return OnlineRedpointEOS::Errors::InvalidAuth(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_ApplicationSuspended:
        return OnlineRedpointEOS::Errors::ApplicationSuspended(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_NetworkDisconnected:
        return OnlineRedpointEOS::Errors::NetworkDisconnected(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AccessDenied:
        return OnlineRedpointEOS::Errors::AccessDenied(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingPermissions:
        return OnlineRedpointEOS::Errors::MissingPermissions(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Token_Not_Account:
        return OnlineRedpointEOS::Errors::Token_Not_Account(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TooManyRequests:
        return OnlineRedpointEOS::Errors::TooManyRequests(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyPending:
        return OnlineRedpointEOS::Errors::AlreadyPending(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLocked:
        return OnlineRedpointEOS::Errors::Auth::AccountLocked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLockedForUpdate:
        return OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidRefreshToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidToken(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AuthenticationFailure:
        return OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidPlatformToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongAccount:
        return OnlineRedpointEOS::Errors::Auth::WrongAccount(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongClient:
        return OnlineRedpointEOS::Errors::Auth::WrongClient(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_FullAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::FullAccountRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_HeadlessAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordResetRequired:
        return OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordCannotBeReused:
        return OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_Expired:
        return OnlineRedpointEOS::Errors::Auth::Expired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeConsentRequired:
        return OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ApplicationNotFound:
        return OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ScopeNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountFeatureRestricted:
        return OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountPortalLoadError:
        return OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_CorrectiveActionRequired:
        return OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantCode:
        return OnlineRedpointEOS::Errors::Auth::PinGrantCode(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantExpired:
        return OnlineRedpointEOS::Errors::Auth::PinGrantExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantPending:
        return OnlineRedpointEOS::Errors::Auth::PinGrantPending(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthNotLinked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRevoked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthInvalid:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRestricted:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthCannotLogin:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthExpired:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthIsLastLoginType:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExchangeCodeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_OriginatingExchangeCodeSessionExpired:
        return OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 14, 2)
    case EOS_EResult::EOS_Auth_PersistentAuth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_MFARequired:
        return OnlineRedpointEOS::Errors::Auth::MFARequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ParentalControls:
        return OnlineRedpointEOS::Errors::Auth::ParentalControls(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_NoRealId:
        return OnlineRedpointEOS::Errors::Auth::NoRealId(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_InviteAwaitingAcceptance:
        return OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NoInvitation:
        return OnlineRedpointEOS::Errors::Friends::NoInvitation(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_AlreadyFriends:
        return OnlineRedpointEOS::Errors::Friends::AlreadyFriends(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NotFriends:
        return OnlineRedpointEOS::Errors::Friends::NotFriends(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_StatusInvalid:
        return OnlineRedpointEOS::Errors::Presence::StatusInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_EntitlementStale:
        return OnlineRedpointEOS::Errors::Ecom::EntitlementStale(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogItemStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferPriceInvalid:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CheckoutLoadError:
        return OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionInProgress:
        return OnlineRedpointEOS::Errors::Sessions::SessionInProgress(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NoPermission:
        return OnlineRedpointEOS::Errors::Sessions::NoPermission(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionAlreadyExists:
        return OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidLock:
        return OnlineRedpointEOS::Errors::Sessions::InvalidLock(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidSession:
        return OnlineRedpointEOS::Errors::Sessions::InvalidSession(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteFailed:
        return OnlineRedpointEOS::Errors::Sessions::InviteFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteNotFound:
        return OnlineRedpointEOS::Errors::Sessions::InviteNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_AggregationFailed:
        return OnlineRedpointEOS::Errors::Sessions::AggregationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionNotAnonymous:
        return OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_OutOfSync:
        return OnlineRedpointEOS::Errors::Sessions::OutOfSync(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyInvites:
        return OnlineRedpointEOS::Errors::Sessions::TooManyInvites(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_PresenceSessionExists:
        return OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::NotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 2)
    case EOS_EResult::EOS_Sessions_PlayerSanctioned:
        return OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalidChars:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeTooLarge:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHandleInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_StartIndexInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_RequestInProgress:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserThrottled:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalTokenValidationFailed:
        return OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UserAlreadyExists:
        return OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_AuthExpired:
        return OnlineRedpointEOS::Errors::Connect::AuthExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_InvalidToken:
        return OnlineRedpointEOS::Errors::Connect::InvalidToken(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UnsupportedTokenType:
        return OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailed:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceUnavailable:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceConfigurationFailure:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 8, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount_DEPRECATED:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UI_SocialOverlayLoadError:
        return OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotOwner:
        return OnlineRedpointEOS::Errors::Lobby::NotOwner(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidLock:
        return OnlineRedpointEOS::Errors::Lobby::InvalidLock(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_LobbyAlreadyExists:
        return OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SessionInProgress:
        return OnlineRedpointEOS::Errors::Lobby::SessionInProgress(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NoPermission:
        return OnlineRedpointEOS::Errors::Lobby::NoPermission(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidSession:
        return OnlineRedpointEOS::Errors::Lobby::InvalidSession(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteFailed:
        return OnlineRedpointEOS::Errors::Lobby::InviteFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteNotFound:
        return OnlineRedpointEOS::Errors::Lobby::InviteNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_AggregationFailed:
        return OnlineRedpointEOS::Errors::Lobby::AggregationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyInvites:
        return OnlineRedpointEOS::Errors::Lobby::TooManyInvites(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::NotAllowed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_MemberUpdateOnly:
        return OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 7, 0)
    case EOS_EResult::EOS_Lobby_PresenceLobbyExists:
        return OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Lobby_VoiceNotEnabled:
        return OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessIsAlreadyRunning:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkCommandIsEmpty:
        return OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessCreationFailed:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CriticalError:
        return OnlineRedpointEOS::Errors::Mods::CriticalError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ToolInternalError:
        return OnlineRedpointEOS::Errors::Mods::ToolInternalError(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_IPCFailure:
        return OnlineRedpointEOS::Errors::Mods::IPCFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidIPCResponse:
        return OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_URILaunchFailure:
        return OnlineRedpointEOS::Errors::Mods::URILaunchFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModIsNotInstalled:
        return OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UserDoesNotOwnTheGame:
        return OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdInvalidResult:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CouldNotFindOffer:
        return OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdFailure:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_PurchaseFailure:
        return OnlineRedpointEOS::Errors::Mods::PurchaseFailure(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidGameInstallInfo:
        return OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CannotGetManifestLocation:
        return OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UnsupportedOS:
        return OnlineRedpointEOS::Errors::Mods::UnsupportedOS(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProtectionNotAvailable:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_InvalidMode:
        return OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProductIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientSandboxIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageSessionKeyRequired:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageValidationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageInitializationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerAlreadyRegistered:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotFound:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotProtected:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_ClientDeploymentIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_DeviceIdAuthIsNotSupported:
        return OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_TooManyParticipants:
        return OnlineRedpointEOS::Errors::RTC::TooManyParticipants(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomAlreadyExists:
        return OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserKicked:
        return OnlineRedpointEOS::Errors::RTC::UserKicked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserBanned:
        return OnlineRedpointEOS::Errors::RTC::UserBanned(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomWasLeft:
        return OnlineRedpointEOS::Errors::RTC::RoomWasLeft(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    case EOS_EResult::EOS_RTC_ReconnectionTimegateExpired:
        return OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_ShutdownInvoked:
        return OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_UserIsInBlocklist:
        return OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_ProgressionSnapshot_SnapshotIdUnavailable:
        return OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_KWS_ParentEmailMissing:
        return OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_KWS_UserGraduated:
        return OnlineRedpointEOS::Errors::KWS::UserGraduated(InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_Android_JavaVMNotStored:
        return OnlineRedpointEOS::Errors::Android::JavaVMNotStored(InCall, InContextMsg);
#endif

    default:
        return OnlineRedpointEOS::Errors::UnexpectedError(InCall, InContextMsg);
    }
}

const FOnlineError ConvertError(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg, EOS_EResult Result)
{
    switch (Result)
    {
    case EOS_EResult::EOS_Success:
        return OnlineRedpointEOS::Errors::Success();
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnexpectedError:
        return OnlineRedpointEOS::Errors::UnexpectedError(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoConnection:
        return OnlineRedpointEOS::Errors::NoConnection(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidParameters:
        return OnlineRedpointEOS::Errors::InvalidParameters(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidRequest:
        return OnlineRedpointEOS::Errors::InvalidRequest(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnrecognizedResponse:
        return OnlineRedpointEOS::Errors::UnrecognizedResponse(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_IncompatibleVersion:
        return OnlineRedpointEOS::Errors::IncompatibleVersion(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotConfigured:
        return OnlineRedpointEOS::Errors::NotConfigured(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyConfigured:
        return OnlineRedpointEOS::Errors::AlreadyConfigured(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotImplemented:
        return OnlineRedpointEOS::Errors::NotImplemented(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Canceled:
        return OnlineRedpointEOS::Errors::Canceled(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotFound:
        return OnlineRedpointEOS::Errors::NotFound(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredPatchAvailable:
        return OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredSystemUpdate:
        return OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AgeRestrictionFailure:
        return OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AccountTypeFailure:
        return OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_ChatRestriction:
        return OnlineRedpointEOS::Errors::Permission_ChatRestriction(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_UGCRestriction:
        return OnlineRedpointEOS::Errors::Permission_UGCRestriction(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_OnlinePlayRestricted:
        return OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_OperationWillRetry:
        return OnlineRedpointEOS::Errors::OperationWillRetry(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ApplicationNotBootstrapped:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotInstalled:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceStartFailed:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotRunning:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidCredentials:
        return OnlineRedpointEOS::Errors::InvalidCredentials(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoChange:
        return OnlineRedpointEOS::Errors::NoChange(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_VersionMismatch:
        return OnlineRedpointEOS::Errors::VersionMismatch(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_LimitExceeded:
        return OnlineRedpointEOS::Errors::LimitExceeded(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Disabled:
        return OnlineRedpointEOS::Errors::Disabled(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_DuplicateNotAllowed:
        return OnlineRedpointEOS::Errors::DuplicateNotAllowed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingParameters_DEPRECATED:
        return OnlineRedpointEOS::Errors::MissingParameters(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidSandboxId:
        return OnlineRedpointEOS::Errors::InvalidSandboxId(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TimedOut:
        return OnlineRedpointEOS::Errors::TimedOut(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PartialResult:
        return OnlineRedpointEOS::Errors::PartialResult(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Role:
        return OnlineRedpointEOS::Errors::Missing_Role(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidUser:
        return OnlineRedpointEOS::Errors::InvalidUser(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Feature:
        return OnlineRedpointEOS::Errors::Missing_Feature(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Sandbox:
        return OnlineRedpointEOS::Errors::Invalid_Sandbox(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Deployment:
        return OnlineRedpointEOS::Errors::Invalid_Deployment(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Product:
        return OnlineRedpointEOS::Errors::Invalid_Product(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_ProductUserID:
        return OnlineRedpointEOS::Errors::Invalid_ProductUserID(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_ServiceFailure:
        return OnlineRedpointEOS::Errors::ServiceFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryMissing:
        return OnlineRedpointEOS::Errors::CacheDirectoryMissing(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryInvalid:
        return OnlineRedpointEOS::Errors::CacheDirectoryInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidState:
        return OnlineRedpointEOS::Errors::InvalidState(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_RequestInProgress:
        return OnlineRedpointEOS::Errors::RequestInProgress(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidAuth:
        return OnlineRedpointEOS::Errors::InvalidAuth(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_ApplicationSuspended:
        return OnlineRedpointEOS::Errors::ApplicationSuspended(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_NetworkDisconnected:
        return OnlineRedpointEOS::Errors::NetworkDisconnected(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AccessDenied:
        return OnlineRedpointEOS::Errors::AccessDenied(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingPermissions:
        return OnlineRedpointEOS::Errors::MissingPermissions(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Token_Not_Account:
        return OnlineRedpointEOS::Errors::Token_Not_Account(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TooManyRequests:
        return OnlineRedpointEOS::Errors::TooManyRequests(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyPending:
        return OnlineRedpointEOS::Errors::AlreadyPending(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLocked:
        return OnlineRedpointEOS::Errors::Auth::AccountLocked(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLockedForUpdate:
        return OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidRefreshToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidToken(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AuthenticationFailure:
        return OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidPlatformToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongAccount:
        return OnlineRedpointEOS::Errors::Auth::WrongAccount(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongClient:
        return OnlineRedpointEOS::Errors::Auth::WrongClient(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_FullAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::FullAccountRequired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_HeadlessAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordResetRequired:
        return OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordCannotBeReused:
        return OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_Expired:
        return OnlineRedpointEOS::Errors::Auth::Expired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeConsentRequired:
        return OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ApplicationNotFound:
        return OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ScopeNotFound(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountFeatureRestricted:
        return OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountPortalLoadError:
        return OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_CorrectiveActionRequired:
        return OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantCode:
        return OnlineRedpointEOS::Errors::Auth::PinGrantCode(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantExpired:
        return OnlineRedpointEOS::Errors::Auth::PinGrantExpired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantPending:
        return OnlineRedpointEOS::Errors::Auth::PinGrantPending(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthNotLinked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRevoked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthInvalid:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRestricted:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthCannotLogin:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthExpired:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthIsLastLoginType:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExchangeCodeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_OriginatingExchangeCodeSessionExpired:
        return OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 14, 2)
    case EOS_EResult::EOS_Auth_PersistentAuth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_MFARequired:
        return OnlineRedpointEOS::Errors::Auth::MFARequired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ParentalControls:
        return OnlineRedpointEOS::Errors::Auth::ParentalControls(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_NoRealId:
        return OnlineRedpointEOS::Errors::Auth::NoRealId(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_InviteAwaitingAcceptance:
        return OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NoInvitation:
        return OnlineRedpointEOS::Errors::Friends::NoInvitation(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_AlreadyFriends:
        return OnlineRedpointEOS::Errors::Friends::AlreadyFriends(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NotFriends:
        return OnlineRedpointEOS::Errors::Friends::NotFriends(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_StatusInvalid:
        return OnlineRedpointEOS::Errors::Presence::StatusInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_EntitlementStale:
        return OnlineRedpointEOS::Errors::Ecom::EntitlementStale(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogItemStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferPriceInvalid:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CheckoutLoadError:
        return OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionInProgress:
        return OnlineRedpointEOS::Errors::Sessions::SessionInProgress(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NoPermission:
        return OnlineRedpointEOS::Errors::Sessions::NoPermission(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionAlreadyExists:
        return OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidLock:
        return OnlineRedpointEOS::Errors::Sessions::InvalidLock(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidSession:
        return OnlineRedpointEOS::Errors::Sessions::InvalidSession(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteFailed:
        return OnlineRedpointEOS::Errors::Sessions::InviteFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteNotFound:
        return OnlineRedpointEOS::Errors::Sessions::InviteNotFound(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_AggregationFailed:
        return OnlineRedpointEOS::Errors::Sessions::AggregationFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionNotAnonymous:
        return OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_OutOfSync:
        return OnlineRedpointEOS::Errors::Sessions::OutOfSync(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyInvites:
        return OnlineRedpointEOS::Errors::Sessions::TooManyInvites(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_PresenceSessionExists:
        return OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::NotAllowed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 2)
    case EOS_EResult::EOS_Sessions_PlayerSanctioned:
        return OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalidChars:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeTooLarge:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHandleInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_StartIndexInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_RequestInProgress:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserThrottled:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalTokenValidationFailed:
        return OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UserAlreadyExists:
        return OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_AuthExpired:
        return OnlineRedpointEOS::Errors::Connect::AuthExpired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_InvalidToken:
        return OnlineRedpointEOS::Errors::Connect::InvalidToken(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UnsupportedTokenType:
        return OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailed:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceUnavailable:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceConfigurationFailure:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 8, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount_DEPRECATED:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UI_SocialOverlayLoadError:
        return OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotOwner:
        return OnlineRedpointEOS::Errors::Lobby::NotOwner(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidLock:
        return OnlineRedpointEOS::Errors::Lobby::InvalidLock(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_LobbyAlreadyExists:
        return OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SessionInProgress:
        return OnlineRedpointEOS::Errors::Lobby::SessionInProgress(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NoPermission:
        return OnlineRedpointEOS::Errors::Lobby::NoPermission(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidSession:
        return OnlineRedpointEOS::Errors::Lobby::InvalidSession(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteFailed:
        return OnlineRedpointEOS::Errors::Lobby::InviteFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteNotFound:
        return OnlineRedpointEOS::Errors::Lobby::InviteNotFound(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_AggregationFailed:
        return OnlineRedpointEOS::Errors::Lobby::AggregationFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyInvites:
        return OnlineRedpointEOS::Errors::Lobby::TooManyInvites(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::NotAllowed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_MemberUpdateOnly:
        return OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 7, 0)
    case EOS_EResult::EOS_Lobby_PresenceLobbyExists:
        return OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Lobby_VoiceNotEnabled:
        return OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessIsAlreadyRunning:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkCommandIsEmpty:
        return OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessCreationFailed:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CriticalError:
        return OnlineRedpointEOS::Errors::Mods::CriticalError(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ToolInternalError:
        return OnlineRedpointEOS::Errors::Mods::ToolInternalError(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_IPCFailure:
        return OnlineRedpointEOS::Errors::Mods::IPCFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidIPCResponse:
        return OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_URILaunchFailure:
        return OnlineRedpointEOS::Errors::Mods::URILaunchFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModIsNotInstalled:
        return OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UserDoesNotOwnTheGame:
        return OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdInvalidResult:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CouldNotFindOffer:
        return OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdFailure:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_PurchaseFailure:
        return OnlineRedpointEOS::Errors::Mods::PurchaseFailure(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidGameInstallInfo:
        return OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CannotGetManifestLocation:
        return OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UnsupportedOS:
        return OnlineRedpointEOS::Errors::Mods::UnsupportedOS(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProtectionNotAvailable:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_InvalidMode:
        return OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProductIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientSandboxIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageSessionKeyRequired:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageValidationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageInitializationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerAlreadyRegistered:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotFound:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotProtected:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_ClientDeploymentIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_DeviceIdAuthIsNotSupported:
        return OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_TooManyParticipants:
        return OnlineRedpointEOS::Errors::RTC::TooManyParticipants(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomAlreadyExists:
        return OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserKicked:
        return OnlineRedpointEOS::Errors::RTC::UserKicked(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserBanned:
        return OnlineRedpointEOS::Errors::RTC::UserBanned(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomWasLeft:
        return OnlineRedpointEOS::Errors::RTC::RoomWasLeft(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    case EOS_EResult::EOS_RTC_ReconnectionTimegateExpired:
        return OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_ShutdownInvoked:
        return OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_UserIsInBlocklist:
        return OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_ProgressionSnapshot_SnapshotIdUnavailable:
        return OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_KWS_ParentEmailMissing:
        return OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_KWS_UserGraduated:
        return OnlineRedpointEOS::Errors::KWS::UserGraduated(InActorId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_Android_JavaVMNotStored:
        return OnlineRedpointEOS::Errors::Android::JavaVMNotStored(InActorId, InCall, InContextMsg);
#endif

    default:
        return OnlineRedpointEOS::Errors::UnexpectedError(InActorId, InCall, InContextMsg);
    }
}

const FOnlineError ConvertError(const FUniqueNetId& InActorId, EOS_EResult Result)
{
    switch (Result)
    {
    case EOS_EResult::EOS_Success:
        return OnlineRedpointEOS::Errors::Success();
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnexpectedError:
        return OnlineRedpointEOS::Errors::UnexpectedError(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoConnection:
        return OnlineRedpointEOS::Errors::NoConnection(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidParameters:
        return OnlineRedpointEOS::Errors::InvalidParameters(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidRequest:
        return OnlineRedpointEOS::Errors::InvalidRequest(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnrecognizedResponse:
        return OnlineRedpointEOS::Errors::UnrecognizedResponse(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_IncompatibleVersion:
        return OnlineRedpointEOS::Errors::IncompatibleVersion(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotConfigured:
        return OnlineRedpointEOS::Errors::NotConfigured(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyConfigured:
        return OnlineRedpointEOS::Errors::AlreadyConfigured(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotImplemented:
        return OnlineRedpointEOS::Errors::NotImplemented(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Canceled:
        return OnlineRedpointEOS::Errors::Canceled(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotFound:
        return OnlineRedpointEOS::Errors::NotFound(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredPatchAvailable:
        return OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredSystemUpdate:
        return OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AgeRestrictionFailure:
        return OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AccountTypeFailure:
        return OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_ChatRestriction:
        return OnlineRedpointEOS::Errors::Permission_ChatRestriction(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_UGCRestriction:
        return OnlineRedpointEOS::Errors::Permission_UGCRestriction(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_OnlinePlayRestricted:
        return OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_OperationWillRetry:
        return OnlineRedpointEOS::Errors::OperationWillRetry(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ApplicationNotBootstrapped:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotInstalled:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceStartFailed:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotRunning:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidCredentials:
        return OnlineRedpointEOS::Errors::InvalidCredentials(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoChange:
        return OnlineRedpointEOS::Errors::NoChange(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_VersionMismatch:
        return OnlineRedpointEOS::Errors::VersionMismatch(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_LimitExceeded:
        return OnlineRedpointEOS::Errors::LimitExceeded(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Disabled:
        return OnlineRedpointEOS::Errors::Disabled(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_DuplicateNotAllowed:
        return OnlineRedpointEOS::Errors::DuplicateNotAllowed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingParameters_DEPRECATED:
        return OnlineRedpointEOS::Errors::MissingParameters(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidSandboxId:
        return OnlineRedpointEOS::Errors::InvalidSandboxId(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TimedOut:
        return OnlineRedpointEOS::Errors::TimedOut(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PartialResult:
        return OnlineRedpointEOS::Errors::PartialResult(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Role:
        return OnlineRedpointEOS::Errors::Missing_Role(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidUser:
        return OnlineRedpointEOS::Errors::InvalidUser(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Feature:
        return OnlineRedpointEOS::Errors::Missing_Feature(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Sandbox:
        return OnlineRedpointEOS::Errors::Invalid_Sandbox(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Deployment:
        return OnlineRedpointEOS::Errors::Invalid_Deployment(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Product:
        return OnlineRedpointEOS::Errors::Invalid_Product(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_ProductUserID:
        return OnlineRedpointEOS::Errors::Invalid_ProductUserID(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_ServiceFailure:
        return OnlineRedpointEOS::Errors::ServiceFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryMissing:
        return OnlineRedpointEOS::Errors::CacheDirectoryMissing(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryInvalid:
        return OnlineRedpointEOS::Errors::CacheDirectoryInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidState:
        return OnlineRedpointEOS::Errors::InvalidState(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_RequestInProgress:
        return OnlineRedpointEOS::Errors::RequestInProgress(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidAuth:
        return OnlineRedpointEOS::Errors::InvalidAuth(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_ApplicationSuspended:
        return OnlineRedpointEOS::Errors::ApplicationSuspended(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_NetworkDisconnected:
        return OnlineRedpointEOS::Errors::NetworkDisconnected(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AccessDenied:
        return OnlineRedpointEOS::Errors::AccessDenied(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingPermissions:
        return OnlineRedpointEOS::Errors::MissingPermissions(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Token_Not_Account:
        return OnlineRedpointEOS::Errors::Token_Not_Account(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TooManyRequests:
        return OnlineRedpointEOS::Errors::TooManyRequests(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyPending:
        return OnlineRedpointEOS::Errors::AlreadyPending(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLocked:
        return OnlineRedpointEOS::Errors::Auth::AccountLocked(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLockedForUpdate:
        return OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidRefreshToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidToken(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AuthenticationFailure:
        return OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidPlatformToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongAccount:
        return OnlineRedpointEOS::Errors::Auth::WrongAccount(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongClient:
        return OnlineRedpointEOS::Errors::Auth::WrongClient(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_FullAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::FullAccountRequired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_HeadlessAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordResetRequired:
        return OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordCannotBeReused:
        return OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_Expired:
        return OnlineRedpointEOS::Errors::Auth::Expired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeConsentRequired:
        return OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ApplicationNotFound:
        return OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ScopeNotFound(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountFeatureRestricted:
        return OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountPortalLoadError:
        return OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_CorrectiveActionRequired:
        return OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantCode:
        return OnlineRedpointEOS::Errors::Auth::PinGrantCode(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantExpired:
        return OnlineRedpointEOS::Errors::Auth::PinGrantExpired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantPending:
        return OnlineRedpointEOS::Errors::Auth::PinGrantPending(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthNotLinked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRevoked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthInvalid:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRestricted:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthCannotLogin:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthExpired:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthIsLastLoginType:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExchangeCodeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_OriginatingExchangeCodeSessionExpired:
        return OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 14, 2)
    case EOS_EResult::EOS_Auth_PersistentAuth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_MFARequired:
        return OnlineRedpointEOS::Errors::Auth::MFARequired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ParentalControls:
        return OnlineRedpointEOS::Errors::Auth::ParentalControls(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_NoRealId:
        return OnlineRedpointEOS::Errors::Auth::NoRealId(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_InviteAwaitingAcceptance:
        return OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NoInvitation:
        return OnlineRedpointEOS::Errors::Friends::NoInvitation(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_AlreadyFriends:
        return OnlineRedpointEOS::Errors::Friends::AlreadyFriends(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NotFriends:
        return OnlineRedpointEOS::Errors::Friends::NotFriends(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_StatusInvalid:
        return OnlineRedpointEOS::Errors::Presence::StatusInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_EntitlementStale:
        return OnlineRedpointEOS::Errors::Ecom::EntitlementStale(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogItemStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferPriceInvalid:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CheckoutLoadError:
        return OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionInProgress:
        return OnlineRedpointEOS::Errors::Sessions::SessionInProgress(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NoPermission:
        return OnlineRedpointEOS::Errors::Sessions::NoPermission(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionAlreadyExists:
        return OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidLock:
        return OnlineRedpointEOS::Errors::Sessions::InvalidLock(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidSession:
        return OnlineRedpointEOS::Errors::Sessions::InvalidSession(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteFailed:
        return OnlineRedpointEOS::Errors::Sessions::InviteFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteNotFound:
        return OnlineRedpointEOS::Errors::Sessions::InviteNotFound(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_AggregationFailed:
        return OnlineRedpointEOS::Errors::Sessions::AggregationFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionNotAnonymous:
        return OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_OutOfSync:
        return OnlineRedpointEOS::Errors::Sessions::OutOfSync(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyInvites:
        return OnlineRedpointEOS::Errors::Sessions::TooManyInvites(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_PresenceSessionExists:
        return OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::NotAllowed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 2)
    case EOS_EResult::EOS_Sessions_PlayerSanctioned:
        return OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalidChars:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeTooLarge:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHandleInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_StartIndexInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_RequestInProgress:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserThrottled:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalTokenValidationFailed:
        return OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UserAlreadyExists:
        return OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_AuthExpired:
        return OnlineRedpointEOS::Errors::Connect::AuthExpired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_InvalidToken:
        return OnlineRedpointEOS::Errors::Connect::InvalidToken(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UnsupportedTokenType:
        return OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailed:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceUnavailable:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceConfigurationFailure:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 8, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount_DEPRECATED:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UI_SocialOverlayLoadError:
        return OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotOwner:
        return OnlineRedpointEOS::Errors::Lobby::NotOwner(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidLock:
        return OnlineRedpointEOS::Errors::Lobby::InvalidLock(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_LobbyAlreadyExists:
        return OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SessionInProgress:
        return OnlineRedpointEOS::Errors::Lobby::SessionInProgress(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NoPermission:
        return OnlineRedpointEOS::Errors::Lobby::NoPermission(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidSession:
        return OnlineRedpointEOS::Errors::Lobby::InvalidSession(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteFailed:
        return OnlineRedpointEOS::Errors::Lobby::InviteFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteNotFound:
        return OnlineRedpointEOS::Errors::Lobby::InviteNotFound(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_AggregationFailed:
        return OnlineRedpointEOS::Errors::Lobby::AggregationFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyInvites:
        return OnlineRedpointEOS::Errors::Lobby::TooManyInvites(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::NotAllowed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_MemberUpdateOnly:
        return OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 7, 0)
    case EOS_EResult::EOS_Lobby_PresenceLobbyExists:
        return OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Lobby_VoiceNotEnabled:
        return OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessIsAlreadyRunning:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkCommandIsEmpty:
        return OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessCreationFailed:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CriticalError:
        return OnlineRedpointEOS::Errors::Mods::CriticalError(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ToolInternalError:
        return OnlineRedpointEOS::Errors::Mods::ToolInternalError(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_IPCFailure:
        return OnlineRedpointEOS::Errors::Mods::IPCFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidIPCResponse:
        return OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_URILaunchFailure:
        return OnlineRedpointEOS::Errors::Mods::URILaunchFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModIsNotInstalled:
        return OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UserDoesNotOwnTheGame:
        return OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdInvalidResult:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CouldNotFindOffer:
        return OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdFailure:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_PurchaseFailure:
        return OnlineRedpointEOS::Errors::Mods::PurchaseFailure(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidGameInstallInfo:
        return OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CannotGetManifestLocation:
        return OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UnsupportedOS:
        return OnlineRedpointEOS::Errors::Mods::UnsupportedOS(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProtectionNotAvailable:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_InvalidMode:
        return OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProductIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientSandboxIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageSessionKeyRequired:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageValidationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageInitializationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerAlreadyRegistered:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotFound:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotProtected:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_ClientDeploymentIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_DeviceIdAuthIsNotSupported:
        return OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_TooManyParticipants:
        return OnlineRedpointEOS::Errors::RTC::TooManyParticipants(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomAlreadyExists:
        return OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserKicked:
        return OnlineRedpointEOS::Errors::RTC::UserKicked(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserBanned:
        return OnlineRedpointEOS::Errors::RTC::UserBanned(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomWasLeft:
        return OnlineRedpointEOS::Errors::RTC::RoomWasLeft(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    case EOS_EResult::EOS_RTC_ReconnectionTimegateExpired:
        return OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_ShutdownInvoked:
        return OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_UserIsInBlocklist:
        return OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_ProgressionSnapshot_SnapshotIdUnavailable:
        return OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_KWS_ParentEmailMissing:
        return OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_KWS_UserGraduated:
        return OnlineRedpointEOS::Errors::KWS::UserGraduated(InActorId);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_Android_JavaVMNotStored:
        return OnlineRedpointEOS::Errors::Android::JavaVMNotStored(InActorId);
#endif

    default:
        return OnlineRedpointEOS::Errors::UnexpectedError(InActorId);
    }
}

const FOnlineError ConvertError(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg, EOS_EResult Result)
{
    switch (Result)
    {
    case EOS_EResult::EOS_Success:
        return OnlineRedpointEOS::Errors::Success();
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnexpectedError:
        return OnlineRedpointEOS::Errors::UnexpectedError(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoConnection:
        return OnlineRedpointEOS::Errors::NoConnection(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidParameters:
        return OnlineRedpointEOS::Errors::InvalidParameters(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidRequest:
        return OnlineRedpointEOS::Errors::InvalidRequest(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UnrecognizedResponse:
        return OnlineRedpointEOS::Errors::UnrecognizedResponse(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_IncompatibleVersion:
        return OnlineRedpointEOS::Errors::IncompatibleVersion(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotConfigured:
        return OnlineRedpointEOS::Errors::NotConfigured(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyConfigured:
        return OnlineRedpointEOS::Errors::AlreadyConfigured(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotImplemented:
        return OnlineRedpointEOS::Errors::NotImplemented(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Canceled:
        return OnlineRedpointEOS::Errors::Canceled(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NotFound:
        return OnlineRedpointEOS::Errors::NotFound(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredPatchAvailable:
        return OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_RequiredSystemUpdate:
        return OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AgeRestrictionFailure:
        return OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_AccountTypeFailure:
        return OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_ChatRestriction:
        return OnlineRedpointEOS::Errors::Permission_ChatRestriction(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_UGCRestriction:
        return OnlineRedpointEOS::Errors::Permission_UGCRestriction(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Permission_OnlinePlayRestricted:
        return OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_OperationWillRetry:
        return OnlineRedpointEOS::Errors::OperationWillRetry(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ApplicationNotBootstrapped:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotInstalled:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceStartFailed:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_DesktopCrossplay_ServiceNotRunning:
        return OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidCredentials:
        return OnlineRedpointEOS::Errors::InvalidCredentials(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_NoChange:
        return OnlineRedpointEOS::Errors::NoChange(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_VersionMismatch:
        return OnlineRedpointEOS::Errors::VersionMismatch(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_LimitExceeded:
        return OnlineRedpointEOS::Errors::LimitExceeded(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Disabled:
        return OnlineRedpointEOS::Errors::Disabled(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_DuplicateNotAllowed:
        return OnlineRedpointEOS::Errors::DuplicateNotAllowed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingParameters_DEPRECATED:
        return OnlineRedpointEOS::Errors::MissingParameters(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidSandboxId:
        return OnlineRedpointEOS::Errors::InvalidSandboxId(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TimedOut:
        return OnlineRedpointEOS::Errors::TimedOut(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PartialResult:
        return OnlineRedpointEOS::Errors::PartialResult(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Role:
        return OnlineRedpointEOS::Errors::Missing_Role(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidUser:
        return OnlineRedpointEOS::Errors::InvalidUser(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Missing_Feature:
        return OnlineRedpointEOS::Errors::Missing_Feature(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Sandbox:
        return OnlineRedpointEOS::Errors::Invalid_Sandbox(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Deployment:
        return OnlineRedpointEOS::Errors::Invalid_Deployment(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_Product:
        return OnlineRedpointEOS::Errors::Invalid_Product(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Invalid_ProductUserID:
        return OnlineRedpointEOS::Errors::Invalid_ProductUserID(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_ServiceFailure:
        return OnlineRedpointEOS::Errors::ServiceFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryMissing:
        return OnlineRedpointEOS::Errors::CacheDirectoryMissing(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_CacheDirectoryInvalid:
        return OnlineRedpointEOS::Errors::CacheDirectoryInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidState:
        return OnlineRedpointEOS::Errors::InvalidState(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_RequestInProgress:
        return OnlineRedpointEOS::Errors::RequestInProgress(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_InvalidAuth:
        return OnlineRedpointEOS::Errors::InvalidAuth(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_ApplicationSuspended:
        return OnlineRedpointEOS::Errors::ApplicationSuspended(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_NetworkDisconnected:
        return OnlineRedpointEOS::Errors::NetworkDisconnected(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AccessDenied:
        return OnlineRedpointEOS::Errors::AccessDenied(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_MissingPermissions:
        return OnlineRedpointEOS::Errors::MissingPermissions(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Token_Not_Account:
        return OnlineRedpointEOS::Errors::Token_Not_Account(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_TooManyRequests:
        return OnlineRedpointEOS::Errors::TooManyRequests(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_AlreadyPending:
        return OnlineRedpointEOS::Errors::AlreadyPending(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLocked:
        return OnlineRedpointEOS::Errors::Auth::AccountLocked(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountLockedForUpdate:
        return OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidRefreshToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidToken(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AuthenticationFailure:
        return OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_InvalidPlatformToken:
        return OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongAccount:
        return OnlineRedpointEOS::Errors::Auth::WrongAccount(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_WrongClient:
        return OnlineRedpointEOS::Errors::Auth::WrongClient(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_FullAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::FullAccountRequired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_HeadlessAccountRequired:
        return OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordResetRequired:
        return OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PasswordCannotBeReused:
        return OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_Expired:
        return OnlineRedpointEOS::Errors::Auth::Expired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeConsentRequired:
        return OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ApplicationNotFound:
        return OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ScopeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ScopeNotFound(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_AccountFeatureRestricted:
        return OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountPortalLoadError:
        return OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_CorrectiveActionRequired:
        return OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantCode:
        return OnlineRedpointEOS::Errors::Auth::PinGrantCode(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantExpired:
        return OnlineRedpointEOS::Errors::Auth::PinGrantExpired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_PinGrantPending:
        return OnlineRedpointEOS::Errors::Auth::PinGrantPending(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthNotLinked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRevoked:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthInvalid:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthRestricted:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthCannotLogin:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthExpired:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExternalAuthIsLastLoginType:
        return OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ExchangeCodeNotFound:
        return OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_OriginatingExchangeCodeSessionExpired:
        return OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Auth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 14, 2)
    case EOS_EResult::EOS_Auth_PersistentAuth_AccountNotActive:
        return OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_MFARequired:
        return OnlineRedpointEOS::Errors::Auth::MFARequired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_ParentalControls:
        return OnlineRedpointEOS::Errors::Auth::ParentalControls(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Auth_NoRealId:
        return OnlineRedpointEOS::Errors::Auth::NoRealId(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_InviteAwaitingAcceptance:
        return OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NoInvitation:
        return OnlineRedpointEOS::Errors::Friends::NoInvitation(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_AlreadyFriends:
        return OnlineRedpointEOS::Errors::Friends::AlreadyFriends(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_NotFriends:
        return OnlineRedpointEOS::Errors::Friends::NotFriends(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserTooManyInvites:
        return OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_TargetUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Friends_LocalUserFriendLimitExceeded:
        return OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataKeyLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_DataValueLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_RichTextLengthInvalid:
        return OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Presence_StatusInvalid:
        return OnlineRedpointEOS::Errors::Presence::StatusInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_EntitlementStale:
        return OnlineRedpointEOS::Errors::Ecom::EntitlementStale(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogItemStale:
        return OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CatalogOfferPriceInvalid:
        return OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Ecom_CheckoutLoadError:
        return OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionInProgress:
        return OnlineRedpointEOS::Errors::Sessions::SessionInProgress(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NoPermission:
        return OnlineRedpointEOS::Errors::Sessions::NoPermission(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionAlreadyExists:
        return OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidLock:
        return OnlineRedpointEOS::Errors::Sessions::InvalidLock(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InvalidSession:
        return OnlineRedpointEOS::Errors::Sessions::InvalidSession(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteFailed:
        return OnlineRedpointEOS::Errors::Sessions::InviteFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_InviteNotFound:
        return OnlineRedpointEOS::Errors::Sessions::InviteNotFound(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_AggregationFailed:
        return OnlineRedpointEOS::Errors::Sessions::AggregationFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_SessionNotAnonymous:
        return OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_OutOfSync:
        return OnlineRedpointEOS::Errors::Sessions::OutOfSync(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_TooManyInvites:
        return OnlineRedpointEOS::Errors::Sessions::TooManyInvites(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_PresenceSessionExists:
        return OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Sessions_NotAllowed:
        return OnlineRedpointEOS::Errors::Sessions::NotAllowed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 2)
    case EOS_EResult::EOS_Sessions_PlayerSanctioned:
        return OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FilenameInvalidChars:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeTooLarge:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileSizeInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHandleInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_DataLengthInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_StartIndexInvalid:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_RequestInProgress:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserThrottled:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_PlayerDataStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalTokenValidationFailed:
        return OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UserAlreadyExists:
        return OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_AuthExpired:
        return OnlineRedpointEOS::Errors::Connect::AuthExpired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_InvalidToken:
        return OnlineRedpointEOS::Errors::Connect::InvalidToken(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_UnsupportedTokenType:
        return OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailed:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceUnavailable:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Connect_ExternalServiceConfigurationFailure:
        return OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0) && EOS_VERSION_AT_MOST(1, 8, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Connect_LinkAccountFailedMissingNintendoIdAccount_DEPRECATED:
        return OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_UI_SocialOverlayLoadError:
        return OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotOwner:
        return OnlineRedpointEOS::Errors::Lobby::NotOwner(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidLock:
        return OnlineRedpointEOS::Errors::Lobby::InvalidLock(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_LobbyAlreadyExists:
        return OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SessionInProgress:
        return OnlineRedpointEOS::Errors::Lobby::SessionInProgress(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyPlayers:
        return OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NoPermission:
        return OnlineRedpointEOS::Errors::Lobby::NoPermission(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InvalidSession:
        return OnlineRedpointEOS::Errors::Lobby::InvalidSession(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteFailed:
        return OnlineRedpointEOS::Errors::Lobby::InviteFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_InviteNotFound:
        return OnlineRedpointEOS::Errors::Lobby::InviteNotFound(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_UpsertNotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_AggregationFailed:
        return OnlineRedpointEOS::Errors::Lobby::AggregationFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_HostAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_SandboxAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_TooManyInvites:
        return OnlineRedpointEOS::Errors::Lobby::TooManyInvites(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_DeploymentAtCapacity:
        return OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_NotAllowed:
        return OnlineRedpointEOS::Errors::Lobby::NotAllowed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 6, 0)
    case EOS_EResult::EOS_Lobby_MemberUpdateOnly:
        return OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 7, 0)
    case EOS_EResult::EOS_Lobby_PresenceLobbyExists:
        return OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_Lobby_VoiceNotEnabled:
        return OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_UserErrorFromDataCallback:
        return OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_EncryptionKeyNotSet:
        return OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileCorrupted:
        return OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    case EOS_EResult::EOS_TitleStorage_FileHeaderHasNewerVersion:
        return OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessIsAlreadyRunning:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkCommandIsEmpty:
        return OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModSdkProcessCreationFailed:
        return OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CriticalError:
        return OnlineRedpointEOS::Errors::Mods::CriticalError(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ToolInternalError:
        return OnlineRedpointEOS::Errors::Mods::ToolInternalError(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_IPCFailure:
        return OnlineRedpointEOS::Errors::Mods::IPCFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidIPCResponse:
        return OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_URILaunchFailure:
        return OnlineRedpointEOS::Errors::Mods::URILaunchFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_ModIsNotInstalled:
        return OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UserDoesNotOwnTheGame:
        return OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdInvalidResult:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CouldNotFindOffer:
        return OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_OfferRequestByIdFailure:
        return OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_PurchaseFailure:
        return OnlineRedpointEOS::Errors::Mods::PurchaseFailure(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_InvalidGameInstallInfo:
        return OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_CannotGetManifestLocation:
        return OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 9, 0)
    case EOS_EResult::EOS_Mods_UnsupportedOS:
        return OnlineRedpointEOS::Errors::Mods::UnsupportedOS(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProtectionNotAvailable:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_InvalidMode:
        return OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientProductIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ClientSandboxIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageSessionKeyRequired:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageValidationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_ProtectMessageInitializationFailed:
        return OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerAlreadyRegistered:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotFound:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_AntiCheat_PeerNotProtected:
        return OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_ClientDeploymentIdMismatch:
        return OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 14, 0)
    case EOS_EResult::EOS_AntiCheat_DeviceIdAuthIsNotSupported:
        return OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_TooManyParticipants:
        return OnlineRedpointEOS::Errors::RTC::TooManyParticipants(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomAlreadyExists:
        return OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserKicked:
        return OnlineRedpointEOS::Errors::RTC::UserKicked(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_UserBanned:
        return OnlineRedpointEOS::Errors::RTC::UserBanned(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_RTC_RoomWasLeft:
        return OnlineRedpointEOS::Errors::RTC::RoomWasLeft(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    case EOS_EResult::EOS_RTC_ReconnectionTimegateExpired:
        return OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_ShutdownInvoked:
        return OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    case EOS_EResult::EOS_RTC_UserIsInBlocklist:
        return OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_ProgressionSnapshot_SnapshotIdUnavailable:
        return OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    case EOS_EResult::EOS_KWS_ParentEmailMissing:
        return OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_KWS_UserGraduated:
        return OnlineRedpointEOS::Errors::KWS::UserGraduated(InActorId, InObjectId, InCall, InContextMsg);
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    case EOS_EResult::EOS_Android_JavaVMNotStored:
        return OnlineRedpointEOS::Errors::Android::JavaVMNotStored(InActorId, InObjectId, InCall, InContextMsg);
#endif

    default:
        return OnlineRedpointEOS::Errors::UnexpectedError(InActorId, InObjectId, InCall, InContextMsg);
    }
}

ECreatePartyCompletionResult ConvertErrorTo_ECreatePartyCompletionResult(const FOnlineError& InError)
{
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.session_in_progress")))
    {
        return ECreatePartyCompletionResult::AlreadyInParty;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.voice_not_enabled")))
    {
        return ECreatePartyCompletionResult::AlreadyInPartyOfSpecifiedType;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.rtc.room_already_exists")))
    {
        return ECreatePartyCompletionResult::FailedToCreateMucRoom;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.rtc.user_kicked")))
    {
        return ECreatePartyCompletionResult::FailedToCreateMucRoom;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.rtc.user_banned")))
    {
        return ECreatePartyCompletionResult::FailedToCreateMucRoom;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.rtc.room_was_left")))
    {
        return ECreatePartyCompletionResult::FailedToCreateMucRoom;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.rtc.reconnection_timegate_expired")))
    {
        return ECreatePartyCompletionResult::FailedToCreateMucRoom;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.rtc.shutdown_invoked")))
    {
        return ECreatePartyCompletionResult::FailedToCreateMucRoom;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.missing_permissions")))
    {
        return ECreatePartyCompletionResult::LoggedOut;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.missing_feature")))
    {
        return ECreatePartyCompletionResult::LoggedOut;
    }
    return ECreatePartyCompletionResult::UnknownClientFailure;
}

EJoinPartyCompletionResult ConvertErrorTo_EJoinPartyCompletionResult(const FOnlineError& InError)
{
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.voice_not_enabled")))
    {
        return EJoinPartyCompletionResult::AlreadyInPartyOfSpecifiedType;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.missing_permissions")))
    {
        return EJoinPartyCompletionResult::LoggedOut;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.missing_feature")))
    {
        return EJoinPartyCompletionResult::LoggedOut;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.no_permission")))
    {
        return EJoinPartyCompletionResult::NoSpace;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.sandbox_at_capacity")))
    {
        return EJoinPartyCompletionResult::NoSpace;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.too_many_invites")))
    {
        return EJoinPartyCompletionResult::NoSpace;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.not_allowed")))
    {
        return EJoinPartyCompletionResult::NoSpace;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.invalid_session")))
    {
        return EJoinPartyCompletionResult::NotApproved;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.too_many_players")))
    {
        return EJoinPartyCompletionResult::UnableToRejoin;
    }
    return EJoinPartyCompletionResult::UnknownClientFailure;
}

ELeavePartyCompletionResult ConvertErrorTo_ELeavePartyCompletionResult(const FOnlineError& InError)
{
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.missing_permissions")))
    {
        return ELeavePartyCompletionResult::LoggedOut;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.missing_feature")))
    {
        return ELeavePartyCompletionResult::LoggedOut;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.sandbox_not_allowed")))
    {
        return ELeavePartyCompletionResult::UnknownParty;
    }
    return ELeavePartyCompletionResult::UnknownClientFailure;
}

EOnJoinSessionCompleteResult::Type ConvertErrorTo_EOnJoinSessionCompleteResult(const FOnlineError& InError)
{
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.sessions.sandbox_not_allowed")))
    {
        return EOnJoinSessionCompleteResult::SessionDoesNotExist;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.permission_required_patch_available")))
    {
        return EOnJoinSessionCompleteResult::SessionDoesNotExist;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.sessions.no_permission")))
    {
        return EOnJoinSessionCompleteResult::SessionIsFull;
    }
    return EOnJoinSessionCompleteResult::UnknownError;
}

ESendPartyInvitationCompletionResult ConvertErrorTo_ESendPartyInvitationCompletionResult(const FOnlineError& InError)
{
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.invalid_session")))
    {
        return ESendPartyInvitationCompletionResult::NoPermission;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.member_update_only")))
    {
        return ESendPartyInvitationCompletionResult::NoPermission;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.missing_feature")))
    {
        return ESendPartyInvitationCompletionResult::NotLoggedIn;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.no_permission")))
    {
        return ESendPartyInvitationCompletionResult::PartyFull;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.already_pending")))
    {
        return ESendPartyInvitationCompletionResult::RateLimited;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.disabled")))
    {
        return ESendPartyInvitationCompletionResult::RateLimited;
    }
    return ESendPartyInvitationCompletionResult::UnknownInternalFailure;
}

EUpdateConfigCompletionResult ConvertErrorTo_EUpdateConfigCompletionResult(const FOnlineError& InError)
{
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.invalid_lock")))
    {
        return EUpdateConfigCompletionResult::LocalMemberNotLeader;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.invalid_session")))
    {
        return EUpdateConfigCompletionResult::LocalMemberNotMember;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.missing_feature")))
    {
        return EUpdateConfigCompletionResult::LocalMemberNotMember;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.invalid_parameters")))
    {
        return EUpdateConfigCompletionResult::MessagingFailure;
    }
    if (InError.ErrorCode.EndsWith(TEXT("errors.com.redpoint.eos.lobby.sandbox_not_allowed")))
    {
        return EUpdateConfigCompletionResult::UnknownParty;
    }
    return EUpdateConfigCompletionResult::UnknownClientFailure;
}

