// Copyright June Rhodes. All Rights Reserved.

#include "EOSError.h"

#define ONLINE_ERROR_NAMESPACE "errors.com.redpoint.eos"

const FOnlineError& OnlineRedpointEOS::Errors::Success()
{
    static FOnlineError Err = FOnlineError::Success();
    return Err;
}
const FOnlineError& OnlineRedpointEOS::Errors::UnexpectedError()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unexpected_error"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_UnexpectedError", "An unexpected error that we cannot identify has occurred."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::UnexpectedError(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unexpected_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("An unexpected error that we cannot identify has occurred."))));
}
const FOnlineError OnlineRedpointEOS::Errors::UnexpectedError(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unexpected_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("An unexpected error that we cannot identify has occurred."))));
}
const FOnlineError OnlineRedpointEOS::Errors::UnexpectedError(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unexpected_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("An unexpected error that we cannot identify has occurred."))));
}
const FOnlineError OnlineRedpointEOS::Errors::UnexpectedError(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unexpected_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("An unexpected error that we cannot identify has occurred."))));
}
const FOnlineError OnlineRedpointEOS::Errors::UnexpectedError(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unexpected_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("An unexpected error that we cannot identify has occurred."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::NoConnection()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::NoConnection, TEXT("errors.com.redpoint.eos.no_connection"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_NoConnection", "Failed due to no connection"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::NoConnection(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NoConnection, TEXT("errors.com.redpoint.eos.no_connection"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Failed due to no connection"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NoConnection(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NoConnection, TEXT("errors.com.redpoint.eos.no_connection"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Failed due to no connection"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NoConnection(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NoConnection, TEXT("errors.com.redpoint.eos.no_connection"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Failed due to no connection"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NoConnection(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::NoConnection, TEXT("errors.com.redpoint.eos.no_connection"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Failed due to no connection"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NoConnection(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NoConnection, TEXT("errors.com.redpoint.eos.no_connection"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Failed due to no connection"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::InvalidParameters()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::InvalidParams, TEXT("errors.com.redpoint.eos.invalid_parameters"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_InvalidParameters", "Invalid parameters specified for request"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidParameters(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidParams, TEXT("errors.com.redpoint.eos.invalid_parameters"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invalid parameters specified for request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidParameters(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidParams, TEXT("errors.com.redpoint.eos.invalid_parameters"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invalid parameters specified for request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidParameters(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidParams, TEXT("errors.com.redpoint.eos.invalid_parameters"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invalid parameters specified for request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidParameters(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidParams, TEXT("errors.com.redpoint.eos.invalid_parameters"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invalid parameters specified for request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidParameters(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidParams, TEXT("errors.com.redpoint.eos.invalid_parameters"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invalid parameters specified for request"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::InvalidRequest()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::RequestFailure, TEXT("errors.com.redpoint.eos.invalid_request"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_InvalidRequest", "Invalid request"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidRequest(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::RequestFailure, TEXT("errors.com.redpoint.eos.invalid_request"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invalid request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidRequest(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::RequestFailure, TEXT("errors.com.redpoint.eos.invalid_request"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invalid request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidRequest(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::RequestFailure, TEXT("errors.com.redpoint.eos.invalid_request"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invalid request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidRequest(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::RequestFailure, TEXT("errors.com.redpoint.eos.invalid_request"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invalid request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidRequest(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::RequestFailure, TEXT("errors.com.redpoint.eos.invalid_request"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invalid request"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::UnrecognizedResponse()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unrecognized_response"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_UnrecognizedResponse", "Failed due to unable to parse or recognize a backend response"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::UnrecognizedResponse(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unrecognized_response"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Failed due to unable to parse or recognize a backend response"))));
}
const FOnlineError OnlineRedpointEOS::Errors::UnrecognizedResponse(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unrecognized_response"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Failed due to unable to parse or recognize a backend response"))));
}
const FOnlineError OnlineRedpointEOS::Errors::UnrecognizedResponse(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unrecognized_response"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Failed due to unable to parse or recognize a backend response"))));
}
const FOnlineError OnlineRedpointEOS::Errors::UnrecognizedResponse(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unrecognized_response"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Failed due to unable to parse or recognize a backend response"))));
}
const FOnlineError OnlineRedpointEOS::Errors::UnrecognizedResponse(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.unrecognized_response"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Failed due to unable to parse or recognize a backend response"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::IncompatibleVersion()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::IncompatibleVersion, TEXT("errors.com.redpoint.eos.incompatible_version"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_IncompatibleVersion", "Incompatible client for backend version"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::IncompatibleVersion(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::IncompatibleVersion, TEXT("errors.com.redpoint.eos.incompatible_version"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Incompatible client for backend version"))));
}
const FOnlineError OnlineRedpointEOS::Errors::IncompatibleVersion(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::IncompatibleVersion, TEXT("errors.com.redpoint.eos.incompatible_version"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Incompatible client for backend version"))));
}
const FOnlineError OnlineRedpointEOS::Errors::IncompatibleVersion(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::IncompatibleVersion, TEXT("errors.com.redpoint.eos.incompatible_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Incompatible client for backend version"))));
}
const FOnlineError OnlineRedpointEOS::Errors::IncompatibleVersion(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::IncompatibleVersion, TEXT("errors.com.redpoint.eos.incompatible_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Incompatible client for backend version"))));
}
const FOnlineError OnlineRedpointEOS::Errors::IncompatibleVersion(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::IncompatibleVersion, TEXT("errors.com.redpoint.eos.incompatible_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Incompatible client for backend version"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::NotConfigured()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::NotConfigured, TEXT("errors.com.redpoint.eos.not_configured"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_NotConfigured", "Not configured correctly for use"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::NotConfigured(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotConfigured, TEXT("errors.com.redpoint.eos.not_configured"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Not configured correctly for use"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotConfigured(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotConfigured, TEXT("errors.com.redpoint.eos.not_configured"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Not configured correctly for use"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotConfigured(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotConfigured, TEXT("errors.com.redpoint.eos.not_configured"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Not configured correctly for use"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotConfigured(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotConfigured, TEXT("errors.com.redpoint.eos.not_configured"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Not configured correctly for use"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotConfigured(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotConfigured, TEXT("errors.com.redpoint.eos.not_configured"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Not configured correctly for use"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AlreadyConfigured()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.already_configured"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AlreadyConfigured", "Already configured for use."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyConfigured(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.already_configured"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Already configured for use."))));
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyConfigured(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.already_configured"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Already configured for use."))));
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyConfigured(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.already_configured"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Already configured for use."))));
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyConfigured(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.already_configured"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Already configured for use."))));
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyConfigured(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.already_configured"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Already configured for use."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::NotImplemented()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::NotImplemented, TEXT("errors.com.redpoint.eos.not_implemented"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_NotImplemented", "Feature not available on this implementation"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::NotImplemented(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotImplemented, TEXT("errors.com.redpoint.eos.not_implemented"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Feature not available on this implementation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotImplemented(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotImplemented, TEXT("errors.com.redpoint.eos.not_implemented"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Feature not available on this implementation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotImplemented(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotImplemented, TEXT("errors.com.redpoint.eos.not_implemented"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Feature not available on this implementation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotImplemented(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotImplemented, TEXT("errors.com.redpoint.eos.not_implemented"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Feature not available on this implementation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotImplemented(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::NotImplemented, TEXT("errors.com.redpoint.eos.not_implemented"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Feature not available on this implementation"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Canceled()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::Canceled, TEXT("errors.com.redpoint.eos.canceled"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Canceled", "Operation was canceled (likely by user)"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Canceled(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::Canceled, TEXT("errors.com.redpoint.eos.canceled"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Operation was canceled (likely by user)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Canceled(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::Canceled, TEXT("errors.com.redpoint.eos.canceled"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Operation was canceled (likely by user)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Canceled(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::Canceled, TEXT("errors.com.redpoint.eos.canceled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Operation was canceled (likely by user)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Canceled(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::Canceled, TEXT("errors.com.redpoint.eos.canceled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Operation was canceled (likely by user)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Canceled(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::Canceled, TEXT("errors.com.redpoint.eos.canceled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Operation was canceled (likely by user)"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::NotFound()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.not_found"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_NotFound", "The requested information was not found"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::NotFound(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The requested information was not found"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotFound(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The requested information was not found"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotFound(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The requested information was not found"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotFound(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The requested information was not found"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NotFound(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The requested information was not found"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_patch_available"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Permission_RequiredPatchAvailable", "Patch required before the user can use the privilege"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_patch_available"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Patch required before the user can use the privilege"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_patch_available"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Patch required before the user can use the privilege"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_patch_available"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Patch required before the user can use the privilege"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_patch_available"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Patch required before the user can use the privilege"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredPatchAvailable(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_patch_available"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Patch required before the user can use the privilege"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_system_update"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Permission_RequiredSystemUpdate", "System update required before the user can use the privilege"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_system_update"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("System update required before the user can use the privilege"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_system_update"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("System update required before the user can use the privilege"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_system_update"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("System update required before the user can use the privilege"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_system_update"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("System update required before the user can use the privilege"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_RequiredSystemUpdate(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_required_system_update"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("System update required before the user can use the privilege"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_age_restriction_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Permission_AgeRestrictionFailure", "Parental control failure usually"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_age_restriction_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Parental control failure usually"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_age_restriction_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Parental control failure usually"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_age_restriction_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Parental control failure usually"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_age_restriction_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Parental control failure usually"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AgeRestrictionFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_age_restriction_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Parental control failure usually"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Permission_AccountTypeFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_account_type_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Permission_AccountTypeFailure", "Premium Account Subscription required but not available"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_account_type_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Premium Account Subscription required but not available"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_account_type_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Premium Account Subscription required but not available"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_account_type_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Premium Account Subscription required but not available"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_account_type_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Premium Account Subscription required but not available"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_AccountTypeFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_account_type_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Premium Account Subscription required but not available"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Permission_ChatRestriction()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_chat_restriction"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Permission_ChatRestriction", "User restricted from chat"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_ChatRestriction(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_chat_restriction"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User restricted from chat"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_ChatRestriction(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_chat_restriction"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User restricted from chat"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_ChatRestriction(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_chat_restriction"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User restricted from chat"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_ChatRestriction(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_chat_restriction"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User restricted from chat"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_ChatRestriction(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_chat_restriction"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User restricted from chat"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Permission_UGCRestriction()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_ugc_restriction"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Permission_UGCRestriction", "User restricted from User Generated Content"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_UGCRestriction(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_ugc_restriction"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User restricted from User Generated Content"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_UGCRestriction(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_ugc_restriction"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User restricted from User Generated Content"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_UGCRestriction(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_ugc_restriction"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User restricted from User Generated Content"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_UGCRestriction(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_ugc_restriction"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User restricted from User Generated Content"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_UGCRestriction(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_ugc_restriction"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User restricted from User Generated Content"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_online_play_restricted"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Permission_OnlinePlayRestricted", "Online play is restricted"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_online_play_restricted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Online play is restricted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_online_play_restricted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Online play is restricted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_online_play_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Online play is restricted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_online_play_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Online play is restricted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Permission_OnlinePlayRestricted(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.permission_online_play_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Online play is restricted"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::OperationWillRetry()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.operation_will_retry"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_OperationWillRetry", "An error occurred during an asynchronous operation, and it will be retried. Callbacks receiving this result will be called again in the future."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::OperationWillRetry(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.operation_will_retry"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("An error occurred during an asynchronous operation, and it will be retried. Callbacks receiving this result will be called again in the future."))));
}
const FOnlineError OnlineRedpointEOS::Errors::OperationWillRetry(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.operation_will_retry"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("An error occurred during an asynchronous operation, and it will be retried. Callbacks receiving this result will be called again in the future."))));
}
const FOnlineError OnlineRedpointEOS::Errors::OperationWillRetry(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.operation_will_retry"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("An error occurred during an asynchronous operation, and it will be retried. Callbacks receiving this result will be called again in the future."))));
}
const FOnlineError OnlineRedpointEOS::Errors::OperationWillRetry(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.operation_will_retry"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("An error occurred during an asynchronous operation, and it will be retried. Callbacks receiving this result will be called again in the future."))));
}
const FOnlineError OnlineRedpointEOS::Errors::OperationWillRetry(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.operation_will_retry"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("An error occurred during an asynchronous operation, and it will be retried. Callbacks receiving this result will be called again in the future."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_application_not_bootstrapped"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_DesktopCrossplay_ApplicationNotBootstrapped", "The application was not launched through the Bootstrapper. Desktop crossplay functionality is unavailable."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_application_not_bootstrapped"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The application was not launched through the Bootstrapper. Desktop crossplay functionality is unavailable."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_application_not_bootstrapped"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The application was not launched through the Bootstrapper. Desktop crossplay functionality is unavailable."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_application_not_bootstrapped"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The application was not launched through the Bootstrapper. Desktop crossplay functionality is unavailable."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_application_not_bootstrapped"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The application was not launched through the Bootstrapper. Desktop crossplay functionality is unavailable."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ApplicationNotBootstrapped(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_application_not_bootstrapped"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The application was not launched through the Bootstrapper. Desktop crossplay functionality is unavailable."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_installed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_DesktopCrossplay_ServiceNotInstalled", "The redistributable service is not installed."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_installed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The redistributable service is not installed."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_installed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The redistributable service is not installed."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_installed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The redistributable service is not installed."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_installed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The redistributable service is not installed."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotInstalled(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_installed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The redistributable service is not installed."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_start_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_DesktopCrossplay_ServiceStartFailed", "The desktop crossplay service failed to start."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_start_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The desktop crossplay service failed to start."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_start_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The desktop crossplay service failed to start."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_start_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The desktop crossplay service failed to start."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_start_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The desktop crossplay service failed to start."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceStartFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_start_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The desktop crossplay service failed to start."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_running"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_DesktopCrossplay_ServiceNotRunning", "The desktop crossplay service is no longer running for an unknown reason."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_running"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The desktop crossplay service is no longer running for an unknown reason."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_running"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The desktop crossplay service is no longer running for an unknown reason."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_running"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The desktop crossplay service is no longer running for an unknown reason."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_running"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The desktop crossplay service is no longer running for an unknown reason."))));
}
const FOnlineError OnlineRedpointEOS::Errors::DesktopCrossplay_ServiceNotRunning(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.desktop_crossplay_service_not_running"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The desktop crossplay service is no longer running for an unknown reason."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::InvalidCredentials()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::InvalidCreds, TEXT("errors.com.redpoint.eos.invalid_credentials"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_InvalidCredentials", "Failed login due to invalid credentials"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidCredentials(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidCreds, TEXT("errors.com.redpoint.eos.invalid_credentials"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Failed login due to invalid credentials"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidCredentials(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidCreds, TEXT("errors.com.redpoint.eos.invalid_credentials"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Failed login due to invalid credentials"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidCredentials(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidCreds, TEXT("errors.com.redpoint.eos.invalid_credentials"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Failed login due to invalid credentials"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidCredentials(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidCreds, TEXT("errors.com.redpoint.eos.invalid_credentials"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Failed login due to invalid credentials"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidCredentials(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidCreds, TEXT("errors.com.redpoint.eos.invalid_credentials"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Failed login due to invalid credentials"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::NoChange()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.no_change"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_NoChange", "The request had no effect"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::NoChange(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.no_change"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The request had no effect"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NoChange(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.no_change"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The request had no effect"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NoChange(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.no_change"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The request had no effect"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NoChange(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.no_change"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The request had no effect"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NoChange(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.no_change"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The request had no effect"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::VersionMismatch()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.version_mismatch"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_VersionMismatch", "The request attempted to use multiple or inconsistent API versions"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::VersionMismatch(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.version_mismatch"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The request attempted to use multiple or inconsistent API versions"))));
}
const FOnlineError OnlineRedpointEOS::Errors::VersionMismatch(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.version_mismatch"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The request attempted to use multiple or inconsistent API versions"))));
}
const FOnlineError OnlineRedpointEOS::Errors::VersionMismatch(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.version_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The request attempted to use multiple or inconsistent API versions"))));
}
const FOnlineError OnlineRedpointEOS::Errors::VersionMismatch(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.version_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The request attempted to use multiple or inconsistent API versions"))));
}
const FOnlineError OnlineRedpointEOS::Errors::VersionMismatch(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.version_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The request attempted to use multiple or inconsistent API versions"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::LimitExceeded()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.limit_exceeded"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_LimitExceeded", "A maximum limit was exceeded on the client, different from EOS_TooManyRequests"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::LimitExceeded(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("A maximum limit was exceeded on the client, different from EOS_TooManyRequests"))));
}
const FOnlineError OnlineRedpointEOS::Errors::LimitExceeded(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("A maximum limit was exceeded on the client, different from EOS_TooManyRequests"))));
}
const FOnlineError OnlineRedpointEOS::Errors::LimitExceeded(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("A maximum limit was exceeded on the client, different from EOS_TooManyRequests"))));
}
const FOnlineError OnlineRedpointEOS::Errors::LimitExceeded(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("A maximum limit was exceeded on the client, different from EOS_TooManyRequests"))));
}
const FOnlineError OnlineRedpointEOS::Errors::LimitExceeded(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("A maximum limit was exceeded on the client, different from EOS_TooManyRequests"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Disabled()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.disabled"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Disabled", "Feature or client ID performing the operation has been disabled."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Disabled(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.disabled"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Feature or client ID performing the operation has been disabled."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Disabled(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.disabled"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Feature or client ID performing the operation has been disabled."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Disabled(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.disabled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Feature or client ID performing the operation has been disabled."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Disabled(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.disabled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Feature or client ID performing the operation has been disabled."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Disabled(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.disabled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Feature or client ID performing the operation has been disabled."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::DuplicateNotAllowed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.duplicate_not_allowed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_DuplicateNotAllowed", "Duplicate entry not allowed"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::DuplicateNotAllowed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.duplicate_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Duplicate entry not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::DuplicateNotAllowed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.duplicate_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Duplicate entry not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::DuplicateNotAllowed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.duplicate_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Duplicate entry not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::DuplicateNotAllowed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.duplicate_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Duplicate entry not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::DuplicateNotAllowed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.duplicate_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Duplicate entry not allowed"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::MissingParameters()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_parameters"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_MissingParameters", "Required parameters are missing. DEPRECATED: This error is no longer used."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::MissingParameters(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_parameters"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Required parameters are missing. DEPRECATED: This error is no longer used."))));
}
const FOnlineError OnlineRedpointEOS::Errors::MissingParameters(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_parameters"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Required parameters are missing. DEPRECATED: This error is no longer used."))));
}
const FOnlineError OnlineRedpointEOS::Errors::MissingParameters(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_parameters"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Required parameters are missing. DEPRECATED: This error is no longer used."))));
}
const FOnlineError OnlineRedpointEOS::Errors::MissingParameters(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_parameters"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Required parameters are missing. DEPRECATED: This error is no longer used."))));
}
const FOnlineError OnlineRedpointEOS::Errors::MissingParameters(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_parameters"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Required parameters are missing. DEPRECATED: This error is no longer used."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::InvalidSandboxId()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox_id"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_InvalidSandboxId", "Sandbox id is invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidSandboxId(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox_id"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Sandbox id is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidSandboxId(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox_id"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Sandbox id is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidSandboxId(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Sandbox id is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidSandboxId(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Sandbox id is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidSandboxId(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Sandbox id is invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::TimedOut()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.timed_out"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_TimedOut", "Request timed out"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::TimedOut(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.timed_out"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request timed out"))));
}
const FOnlineError OnlineRedpointEOS::Errors::TimedOut(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.timed_out"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request timed out"))));
}
const FOnlineError OnlineRedpointEOS::Errors::TimedOut(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.timed_out"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request timed out"))));
}
const FOnlineError OnlineRedpointEOS::Errors::TimedOut(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.timed_out"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request timed out"))));
}
const FOnlineError OnlineRedpointEOS::Errors::TimedOut(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.timed_out"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request timed out"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PartialResult()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.partial_result"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PartialResult", "A query returned some but not all of the requested results."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PartialResult(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.partial_result"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("A query returned some but not all of the requested results."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PartialResult(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.partial_result"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("A query returned some but not all of the requested results."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PartialResult(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.partial_result"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("A query returned some but not all of the requested results."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PartialResult(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.partial_result"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("A query returned some but not all of the requested results."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PartialResult(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.partial_result"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("A query returned some but not all of the requested results."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Missing_Role()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_role"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Missing_Role", "Client is missing the whitelisted role"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Role(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_role"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Client is missing the whitelisted role"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Role(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_role"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Client is missing the whitelisted role"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Role(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_role"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Client is missing the whitelisted role"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Role(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_role"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Client is missing the whitelisted role"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Role(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_role"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Client is missing the whitelisted role"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::InvalidUser()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::InvalidUser, TEXT("errors.com.redpoint.eos.invalid_user"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_InvalidUser", "Failed due to invalid or missing user"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidUser(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidUser, TEXT("errors.com.redpoint.eos.invalid_user"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Failed due to invalid or missing user"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidUser(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidUser, TEXT("errors.com.redpoint.eos.invalid_user"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Failed due to invalid or missing user"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidUser(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidUser, TEXT("errors.com.redpoint.eos.invalid_user"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Failed due to invalid or missing user"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidUser(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidUser, TEXT("errors.com.redpoint.eos.invalid_user"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Failed due to invalid or missing user"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidUser(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidUser, TEXT("errors.com.redpoint.eos.invalid_user"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Failed due to invalid or missing user"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Missing_Feature()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_feature"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Missing_Feature", "Client is missing the whitelisted feature"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Feature(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_feature"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Client is missing the whitelisted feature"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Feature(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_feature"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Client is missing the whitelisted feature"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Feature(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_feature"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Client is missing the whitelisted feature"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Feature(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_feature"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Client is missing the whitelisted feature"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Missing_Feature(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_feature"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Client is missing the whitelisted feature"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Invalid_Sandbox()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Invalid_Sandbox", "The sandbox given to the backend is invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Sandbox(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The sandbox given to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Sandbox(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The sandbox given to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Sandbox(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The sandbox given to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Sandbox(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The sandbox given to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Sandbox(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_sandbox"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The sandbox given to the backend is invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Invalid_Deployment()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_deployment"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Invalid_Deployment", "The deployment given to the backend is invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Deployment(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_deployment"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The deployment given to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Deployment(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_deployment"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The deployment given to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Deployment(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_deployment"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The deployment given to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Deployment(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_deployment"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The deployment given to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Deployment(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_deployment"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The deployment given to the backend is invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Invalid_Product()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Invalid_Product", "The product id specified to the backend is invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Product(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The product id specified to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Product(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The product id specified to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Product(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The product id specified to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Product(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The product id specified to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_Product(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The product id specified to the backend is invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Invalid_ProductUserID()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product_user_id"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Invalid_ProductUserID", "The product user id specified to the backend is invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_ProductUserID(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product_user_id"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The product user id specified to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_ProductUserID(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product_user_id"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The product user id specified to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_ProductUserID(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product_user_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The product user id specified to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_ProductUserID(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product_user_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The product user id specified to the backend is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Invalid_ProductUserID(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_product_user_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The product user id specified to the backend is invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::ServiceFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.service_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_ServiceFailure", "There was a failure with the backend service"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::ServiceFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.service_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("There was a failure with the backend service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::ServiceFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.service_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("There was a failure with the backend service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::ServiceFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.service_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("There was a failure with the backend service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::ServiceFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.service_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("There was a failure with the backend service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::ServiceFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.service_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("There was a failure with the backend service"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::CacheDirectoryMissing()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_missing"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_CacheDirectoryMissing", "Cache directory is not set in platform options."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryMissing(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_missing"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Cache directory is not set in platform options."))));
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryMissing(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_missing"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Cache directory is not set in platform options."))));
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryMissing(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_missing"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Cache directory is not set in platform options."))));
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryMissing(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_missing"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Cache directory is not set in platform options."))));
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryMissing(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_missing"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Cache directory is not set in platform options."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::CacheDirectoryInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_CacheDirectoryInvalid", "Cache directory is not accessible."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Cache directory is not accessible."))));
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Cache directory is not accessible."))));
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Cache directory is not accessible."))));
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Cache directory is not accessible."))));
}
const FOnlineError OnlineRedpointEOS::Errors::CacheDirectoryInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.cache_directory_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Cache directory is not accessible."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::InvalidState()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_state"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_InvalidState", "The request failed because resource was in an invalid state"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidState(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_state"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The request failed because resource was in an invalid state"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidState(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_state"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The request failed because resource was in an invalid state"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidState(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_state"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The request failed because resource was in an invalid state"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidState(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_state"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The request failed because resource was in an invalid state"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidState(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.invalid_state"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The request failed because resource was in an invalid state"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RequestInProgress()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.request_in_progress"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RequestInProgress", "Request is in progress"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RequestInProgress(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request is in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RequestInProgress(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request is in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RequestInProgress(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request is in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RequestInProgress(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request is in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RequestInProgress(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request is in progress"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::InvalidAuth()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::InvalidAuth, TEXT("errors.com.redpoint.eos.invalid_auth"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_InvalidAuth", "Failed due to invalid or missing authentication token for user (e.g. not logged in)"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidAuth(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidAuth, TEXT("errors.com.redpoint.eos.invalid_auth"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Failed due to invalid or missing authentication token for user (e.g. not logged in)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidAuth(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidAuth, TEXT("errors.com.redpoint.eos.invalid_auth"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Failed due to invalid or missing authentication token for user (e.g. not logged in)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidAuth(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidAuth, TEXT("errors.com.redpoint.eos.invalid_auth"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Failed due to invalid or missing authentication token for user (e.g. not logged in)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidAuth(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidAuth, TEXT("errors.com.redpoint.eos.invalid_auth"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Failed due to invalid or missing authentication token for user (e.g. not logged in)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::InvalidAuth(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::InvalidAuth, TEXT("errors.com.redpoint.eos.invalid_auth"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Failed due to invalid or missing authentication token for user (e.g. not logged in)"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::ApplicationSuspended()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.application_suspended"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_ApplicationSuspended", "Application is suspended"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::ApplicationSuspended(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.application_suspended"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Application is suspended"))));
}
const FOnlineError OnlineRedpointEOS::Errors::ApplicationSuspended(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.application_suspended"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Application is suspended"))));
}
const FOnlineError OnlineRedpointEOS::Errors::ApplicationSuspended(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.application_suspended"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Application is suspended"))));
}
const FOnlineError OnlineRedpointEOS::Errors::ApplicationSuspended(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.application_suspended"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Application is suspended"))));
}
const FOnlineError OnlineRedpointEOS::Errors::ApplicationSuspended(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.application_suspended"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Application is suspended"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::NetworkDisconnected()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.network_disconnected"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_NetworkDisconnected", "Network is disconnected"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::NetworkDisconnected(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.network_disconnected"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Network is disconnected"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NetworkDisconnected(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.network_disconnected"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Network is disconnected"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NetworkDisconnected(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.network_disconnected"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Network is disconnected"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NetworkDisconnected(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.network_disconnected"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Network is disconnected"))));
}
const FOnlineError OnlineRedpointEOS::Errors::NetworkDisconnected(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.network_disconnected"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Network is disconnected"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AccessDenied()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::AccessDenied, TEXT("errors.com.redpoint.eos.access_denied"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AccessDenied", "Failed due to invalid access"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AccessDenied(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::AccessDenied, TEXT("errors.com.redpoint.eos.access_denied"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Failed due to invalid access"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AccessDenied(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::AccessDenied, TEXT("errors.com.redpoint.eos.access_denied"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Failed due to invalid access"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AccessDenied(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::AccessDenied, TEXT("errors.com.redpoint.eos.access_denied"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Failed due to invalid access"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AccessDenied(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::AccessDenied, TEXT("errors.com.redpoint.eos.access_denied"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Failed due to invalid access"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AccessDenied(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::AccessDenied, TEXT("errors.com.redpoint.eos.access_denied"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Failed due to invalid access"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::MissingPermissions()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_permissions"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_MissingPermissions", "If the client does not possess the permission required"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::MissingPermissions(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_permissions"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("If the client does not possess the permission required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::MissingPermissions(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_permissions"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("If the client does not possess the permission required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::MissingPermissions(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_permissions"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("If the client does not possess the permission required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::MissingPermissions(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_permissions"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("If the client does not possess the permission required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::MissingPermissions(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.missing_permissions"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("If the client does not possess the permission required"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Token_Not_Account()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.token_not_account"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Token_Not_Account", "If the token provided does not represent an account"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Token_Not_Account(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.token_not_account"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("If the token provided does not represent an account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Token_Not_Account(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.token_not_account"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("If the token provided does not represent an account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Token_Not_Account(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.token_not_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("If the token provided does not represent an account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Token_Not_Account(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.token_not_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("If the token provided does not represent an account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Token_Not_Account(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.token_not_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("If the token provided does not represent an account"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::TooManyRequests()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::TooManyRequests, TEXT("errors.com.redpoint.eos.too_many_requests"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_TooManyRequests", "Throttled due to too many requests"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::TooManyRequests(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::TooManyRequests, TEXT("errors.com.redpoint.eos.too_many_requests"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Throttled due to too many requests"))));
}
const FOnlineError OnlineRedpointEOS::Errors::TooManyRequests(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::TooManyRequests, TEXT("errors.com.redpoint.eos.too_many_requests"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Throttled due to too many requests"))));
}
const FOnlineError OnlineRedpointEOS::Errors::TooManyRequests(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::TooManyRequests, TEXT("errors.com.redpoint.eos.too_many_requests"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Throttled due to too many requests"))));
}
const FOnlineError OnlineRedpointEOS::Errors::TooManyRequests(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::TooManyRequests, TEXT("errors.com.redpoint.eos.too_many_requests"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Throttled due to too many requests"))));
}
const FOnlineError OnlineRedpointEOS::Errors::TooManyRequests(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::TooManyRequests, TEXT("errors.com.redpoint.eos.too_many_requests"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Throttled due to too many requests"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AlreadyPending()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::AlreadyPending, TEXT("errors.com.redpoint.eos.already_pending"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AlreadyPending", "Async request was already pending"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyPending(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::AlreadyPending, TEXT("errors.com.redpoint.eos.already_pending"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Async request was already pending"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyPending(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::AlreadyPending, TEXT("errors.com.redpoint.eos.already_pending"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Async request was already pending"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyPending(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::AlreadyPending, TEXT("errors.com.redpoint.eos.already_pending"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Async request was already pending"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyPending(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::AlreadyPending, TEXT("errors.com.redpoint.eos.already_pending"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Async request was already pending"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AlreadyPending(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::AlreadyPending, TEXT("errors.com.redpoint.eos.already_pending"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Async request was already pending"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::AccountLocked()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_AccountLocked", "Account locked due to login failures"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLocked(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Account locked due to login failures"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLocked(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Account locked due to login failures"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLocked(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Account locked due to login failures"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLocked(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Account locked due to login failures"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLocked(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Account locked due to login failures"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked_for_update"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_AccountLockedForUpdate", "Account locked by update operation."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked_for_update"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Account locked by update operation."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked_for_update"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Account locked by update operation."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked_for_update"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Account locked by update operation."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked_for_update"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Account locked by update operation."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountLockedForUpdate(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_locked_for_update"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Account locked by update operation."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_refresh_token"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_InvalidRefreshToken", "Refresh token used was invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_refresh_token"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Refresh token used was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_refresh_token"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Refresh token used was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_refresh_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Refresh token used was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_refresh_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Refresh token used was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidRefreshToken(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_refresh_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Refresh token used was invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::InvalidToken()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_token"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_InvalidToken", "Invalid access token, typically when switching between backend environments"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidToken(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_token"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invalid access token, typically when switching between backend environments"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidToken(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_token"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invalid access token, typically when switching between backend environments"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidToken(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invalid access token, typically when switching between backend environments"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidToken(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invalid access token, typically when switching between backend environments"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidToken(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invalid access token, typically when switching between backend environments"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::AuthenticationFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.authentication_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_AuthenticationFailure", "Invalid bearer token"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.authentication_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invalid bearer token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.authentication_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invalid bearer token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.authentication_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invalid bearer token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.authentication_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invalid bearer token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AuthenticationFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.authentication_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invalid bearer token"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_platform_token"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_InvalidPlatformToken", "Invalid platform token"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_platform_token"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invalid platform token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_platform_token"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invalid platform token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_platform_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invalid platform token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_platform_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invalid platform token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::InvalidPlatformToken(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.invalid_platform_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invalid platform token"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::WrongAccount()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_account"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_WrongAccount", "Auth parameters are not associated with this account"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongAccount(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_account"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Auth parameters are not associated with this account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongAccount(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_account"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Auth parameters are not associated with this account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongAccount(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Auth parameters are not associated with this account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongAccount(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Auth parameters are not associated with this account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongAccount(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Auth parameters are not associated with this account"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::WrongClient()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_client"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_WrongClient", "Auth parameters are not associated with this client"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongClient(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_client"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Auth parameters are not associated with this client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongClient(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_client"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Auth parameters are not associated with this client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongClient(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_client"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Auth parameters are not associated with this client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongClient(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_client"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Auth parameters are not associated with this client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::WrongClient(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.wrong_client"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Auth parameters are not associated with this client"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::FullAccountRequired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.full_account_required"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_FullAccountRequired", "Full account is required"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::FullAccountRequired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.full_account_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Full account is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::FullAccountRequired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.full_account_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Full account is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::FullAccountRequired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.full_account_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Full account is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::FullAccountRequired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.full_account_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Full account is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::FullAccountRequired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.full_account_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Full account is required"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.headless_account_required"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_HeadlessAccountRequired", "Headless account is required"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.headless_account_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Headless account is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.headless_account_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Headless account is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.headless_account_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Headless account is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.headless_account_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Headless account is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::HeadlessAccountRequired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.headless_account_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Headless account is required"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::PasswordResetRequired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_reset_required"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_PasswordResetRequired", "Password reset is required"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_reset_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Password reset is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_reset_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Password reset is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_reset_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Password reset is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_reset_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Password reset is required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordResetRequired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_reset_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Password reset is required"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_cannot_be_reused"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_PasswordCannotBeReused", "Password was previously used and cannot be reused"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_cannot_be_reused"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Password was previously used and cannot be reused"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_cannot_be_reused"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Password was previously used and cannot be reused"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_cannot_be_reused"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Password was previously used and cannot be reused"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_cannot_be_reused"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Password was previously used and cannot be reused"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PasswordCannotBeReused(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.password_cannot_be_reused"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Password was previously used and cannot be reused"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::Expired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.expired"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_Expired", "Authorization code/exchange code has expired"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::Expired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Authorization code/exchange code has expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::Expired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Authorization code/exchange code has expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::Expired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Authorization code/exchange code has expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::Expired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Authorization code/exchange code has expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::Expired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Authorization code/exchange code has expired"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_consent_required"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ScopeConsentRequired", "Consent has not been given by the user"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_consent_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Consent has not been given by the user"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_consent_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Consent has not been given by the user"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_consent_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Consent has not been given by the user"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_consent_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Consent has not been given by the user"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeConsentRequired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_consent_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Consent has not been given by the user"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ApplicationNotFound()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.application_not_found"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ApplicationNotFound", "The application has no profile on the backend"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.application_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The application has no profile on the backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.application_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The application has no profile on the backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.application_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The application has no profile on the backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.application_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The application has no profile on the backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ApplicationNotFound(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.application_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The application has no profile on the backend"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ScopeNotFound()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_not_found"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ScopeNotFound", "The requested consent wasn't found on the backend"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeNotFound(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The requested consent wasn't found on the backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeNotFound(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The requested consent wasn't found on the backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeNotFound(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The requested consent wasn't found on the backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeNotFound(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The requested consent wasn't found on the backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ScopeNotFound(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.scope_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The requested consent wasn't found on the backend"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_feature_restricted"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_AccountFeatureRestricted", "This account has been denied access to login"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_feature_restricted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("This account has been denied access to login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_feature_restricted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("This account has been denied access to login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_feature_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("This account has been denied access to login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_feature_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("This account has been denied access to login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountFeatureRestricted(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_feature_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("This account has been denied access to login"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_portal_load_error"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_AccountPortalLoadError", "The overlay failed to load the Account Portal. This can range from general overlay failure, to overlay failed to connect to the web server, to overlay failed to render the web page."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_portal_load_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The overlay failed to load the Account Portal. This can range from general overlay failure, to overlay failed to connect to the web server, to overlay failed to render the web page."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_portal_load_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The overlay failed to load the Account Portal. This can range from general overlay failure, to overlay failed to connect to the web server, to overlay failed to render the web page."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_portal_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The overlay failed to load the Account Portal. This can range from general overlay failure, to overlay failed to connect to the web server, to overlay failed to render the web page."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_portal_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The overlay failed to load the Account Portal. This can range from general overlay failure, to overlay failed to connect to the web server, to overlay failed to render the web page."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::AccountPortalLoadError(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.account_portal_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The overlay failed to load the Account Portal. This can range from general overlay failure, to overlay failed to connect to the web server, to overlay failed to render the web page."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.corrective_action_required"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_CorrectiveActionRequired", "An attempted login has failed due to the user needing to take corrective action on their account."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.corrective_action_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("An attempted login has failed due to the user needing to take corrective action on their account."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.corrective_action_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("An attempted login has failed due to the user needing to take corrective action on their account."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.corrective_action_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("An attempted login has failed due to the user needing to take corrective action on their account."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.corrective_action_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("An attempted login has failed due to the user needing to take corrective action on their account."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::CorrectiveActionRequired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.corrective_action_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("An attempted login has failed due to the user needing to take corrective action on their account."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::PinGrantCode()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_code"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_PinGrantCode", "Pin grant code initiated"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantCode(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_code"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Pin grant code initiated"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantCode(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_code"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Pin grant code initiated"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantCode(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_code"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Pin grant code initiated"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantCode(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_code"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Pin grant code initiated"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantCode(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_code"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Pin grant code initiated"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::PinGrantExpired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_expired"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_PinGrantExpired", "Pin grant code attempt expired"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantExpired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Pin grant code attempt expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantExpired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Pin grant code attempt expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantExpired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Pin grant code attempt expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantExpired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Pin grant code attempt expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantExpired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Pin grant code attempt expired"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::PinGrantPending()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_pending"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_PinGrantPending", "Pin grant code attempt pending"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantPending(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_pending"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Pin grant code attempt pending"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantPending(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_pending"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Pin grant code attempt pending"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantPending(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_pending"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Pin grant code attempt pending"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantPending(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_pending"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Pin grant code attempt pending"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PinGrantPending(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.pin_grant_pending"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Pin grant code attempt pending"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_not_linked"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ExternalAuthNotLinked", "External auth source did not yield an account"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_not_linked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("External auth source did not yield an account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_not_linked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("External auth source did not yield an account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_not_linked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("External auth source did not yield an account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_not_linked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("External auth source did not yield an account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthNotLinked(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_not_linked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("External auth source did not yield an account"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_revoked"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ExternalAuthRevoked", "External auth access revoked"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_revoked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("External auth access revoked"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_revoked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("External auth access revoked"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_revoked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("External auth access revoked"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_revoked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("External auth access revoked"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRevoked(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_revoked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("External auth access revoked"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ExternalAuthInvalid", "External auth token cannot be interpreted"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("External auth token cannot be interpreted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("External auth token cannot be interpreted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("External auth token cannot be interpreted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("External auth token cannot be interpreted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("External auth token cannot be interpreted"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_restricted"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ExternalAuthRestricted", "External auth cannot be linked to his account due to restrictions"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_restricted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("External auth cannot be linked to his account due to restrictions"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_restricted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("External auth cannot be linked to his account due to restrictions"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("External auth cannot be linked to his account due to restrictions"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("External auth cannot be linked to his account due to restrictions"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthRestricted(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_restricted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("External auth cannot be linked to his account due to restrictions"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_cannot_login"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ExternalAuthCannotLogin", "External auth cannot be used for login"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_cannot_login"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("External auth cannot be used for login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_cannot_login"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("External auth cannot be used for login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_cannot_login"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("External auth cannot be used for login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_cannot_login"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("External auth cannot be used for login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthCannotLogin(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_cannot_login"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("External auth cannot be used for login"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_expired"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ExternalAuthExpired", "External auth is expired"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("External auth is expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("External auth is expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("External auth is expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("External auth is expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthExpired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("External auth is expired"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_is_last_login_type"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ExternalAuthIsLastLoginType", "External auth cannot be removed since it's the last possible way to login"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_is_last_login_type"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("External auth cannot be removed since it's the last possible way to login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_is_last_login_type"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("External auth cannot be removed since it's the last possible way to login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_is_last_login_type"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("External auth cannot be removed since it's the last possible way to login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_is_last_login_type"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("External auth cannot be removed since it's the last possible way to login"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExternalAuthIsLastLoginType(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.external_auth_is_last_login_type"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("External auth cannot be removed since it's the last possible way to login"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.exchange_code_not_found"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ExchangeCodeNotFound", "Exchange code not found"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.exchange_code_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Exchange code not found"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.exchange_code_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Exchange code not found"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.exchange_code_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Exchange code not found"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.exchange_code_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Exchange code not found"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ExchangeCodeNotFound(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.exchange_code_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Exchange code not found"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.originating_exchange_code_session_expired"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_OriginatingExchangeCodeSessionExpired", "Originating exchange code session has expired"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.originating_exchange_code_session_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Originating exchange code session has expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.originating_exchange_code_session_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Originating exchange code session has expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.originating_exchange_code_session_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Originating exchange code session has expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.originating_exchange_code_session_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Originating exchange code session has expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::OriginatingExchangeCodeSessionExpired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.originating_exchange_code_session_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Originating exchange code session has expired"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.persistent_auth_account_not_active"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_PersistentAuth_AccountNotActive", "The account has been disabled and cannot be used for authentication"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.persistent_auth_account_not_active"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The account has been disabled and cannot be used for authentication"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.persistent_auth_account_not_active"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The account has been disabled and cannot be used for authentication"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.persistent_auth_account_not_active"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The account has been disabled and cannot be used for authentication"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.persistent_auth_account_not_active"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The account has been disabled and cannot be used for authentication"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::PersistentAuth_AccountNotActive(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.persistent_auth_account_not_active"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The account has been disabled and cannot be used for authentication"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::MFARequired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.mfa_required"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_MFARequired", "MFA challenge required"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::MFARequired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.mfa_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("MFA challenge required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::MFARequired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.mfa_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("MFA challenge required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::MFARequired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.mfa_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("MFA challenge required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::MFARequired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.mfa_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("MFA challenge required"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::MFARequired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.mfa_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("MFA challenge required"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::ParentalControls()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.parental_controls"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_ParentalControls", "Parental locks are in place"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ParentalControls(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.parental_controls"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Parental locks are in place"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ParentalControls(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.parental_controls"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Parental locks are in place"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ParentalControls(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.parental_controls"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Parental locks are in place"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ParentalControls(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.parental_controls"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Parental locks are in place"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::ParentalControls(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.parental_controls"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Parental locks are in place"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Auth::NoRealId()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.no_real_id"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Auth_NoRealId", "Korea real id association required but missing"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::NoRealId(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.no_real_id"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Korea real id association required but missing"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::NoRealId(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.no_real_id"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Korea real id association required but missing"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::NoRealId(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.no_real_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Korea real id association required but missing"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::NoRealId(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.no_real_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Korea real id association required but missing"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Auth::NoRealId(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.auth.no_real_id"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Korea real id association required but missing"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.invite_awaiting_acceptance"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Friends_InviteAwaitingAcceptance", "An outgoing friend invitation is awaiting acceptance; sending another invite to the same user is erroneous"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.invite_awaiting_acceptance"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("An outgoing friend invitation is awaiting acceptance; sending another invite to the same user is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.invite_awaiting_acceptance"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("An outgoing friend invitation is awaiting acceptance; sending another invite to the same user is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.invite_awaiting_acceptance"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("An outgoing friend invitation is awaiting acceptance; sending another invite to the same user is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.invite_awaiting_acceptance"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("An outgoing friend invitation is awaiting acceptance; sending another invite to the same user is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::InviteAwaitingAcceptance(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.invite_awaiting_acceptance"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("An outgoing friend invitation is awaiting acceptance; sending another invite to the same user is erroneous"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Friends::NoInvitation()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.no_invitation"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Friends_NoInvitation", "There is no friend invitation to accept/reject"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NoInvitation(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.no_invitation"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("There is no friend invitation to accept/reject"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NoInvitation(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.no_invitation"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("There is no friend invitation to accept/reject"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NoInvitation(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.no_invitation"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("There is no friend invitation to accept/reject"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NoInvitation(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.no_invitation"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("There is no friend invitation to accept/reject"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NoInvitation(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.no_invitation"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("There is no friend invitation to accept/reject"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Friends::AlreadyFriends()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.already_friends"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Friends_AlreadyFriends", "Users are already friends, so sending another invite is erroneous"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::AlreadyFriends(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.already_friends"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Users are already friends, so sending another invite is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::AlreadyFriends(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.already_friends"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Users are already friends, so sending another invite is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::AlreadyFriends(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.already_friends"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Users are already friends, so sending another invite is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::AlreadyFriends(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.already_friends"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Users are already friends, so sending another invite is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::AlreadyFriends(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.already_friends"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Users are already friends, so sending another invite is erroneous"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Friends::NotFriends()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.not_friends"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Friends_NotFriends", "Users are not friends, so deleting the friend is erroneous"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NotFriends(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.not_friends"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Users are not friends, so deleting the friend is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NotFriends(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.not_friends"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Users are not friends, so deleting the friend is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NotFriends(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.not_friends"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Users are not friends, so deleting the friend is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NotFriends(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.not_friends"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Users are not friends, so deleting the friend is erroneous"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::NotFriends(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.not_friends"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Users are not friends, so deleting the friend is erroneous"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_too_many_invites"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Friends_TargetUserTooManyInvites", "Remote user has too many invites to receive new invites"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Remote user has too many invites to receive new invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Remote user has too many invites to receive new invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Remote user has too many invites to receive new invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Remote user has too many invites to receive new invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserTooManyInvites(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Remote user has too many invites to receive new invites"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_too_many_invites"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Friends_LocalUserTooManyInvites", "Local user has too many invites to send new invites"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Local user has too many invites to send new invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Local user has too many invites to send new invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Local user has too many invites to send new invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Local user has too many invites to send new invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserTooManyInvites(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Local user has too many invites to send new invites"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_friend_limit_exceeded"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Friends_TargetUserFriendLimitExceeded", "Remote user has too many friends to make a new friendship"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Remote user has too many friends to make a new friendship"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Remote user has too many friends to make a new friendship"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Remote user has too many friends to make a new friendship"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Remote user has too many friends to make a new friendship"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::TargetUserFriendLimitExceeded(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.target_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Remote user has too many friends to make a new friendship"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_friend_limit_exceeded"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Friends_LocalUserFriendLimitExceeded", "Local user has too many friends to make a new friendship"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Local user has too many friends to make a new friendship"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Local user has too many friends to make a new friendship"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Local user has too many friends to make a new friendship"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Local user has too many friends to make a new friendship"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Friends::LocalUserFriendLimitExceeded(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.friends.local_user_friend_limit_exceeded"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Local user has too many friends to make a new friendship"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::DataInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_DataInvalid", "Request data was null or invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request data was null or invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request data was null or invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request data was null or invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request data was null or invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request data was null or invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::DataLengthInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_length_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_DataLengthInvalid", "Request contained too many or too few unique data items, or the request would overflow the maximum amount of data allowed"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request contained too many or too few unique data items, or the request would overflow the maximum amount of data allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request contained too many or too few unique data items, or the request would overflow the maximum amount of data allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request contained too many or too few unique data items, or the request would overflow the maximum amount of data allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request contained too many or too few unique data items, or the request would overflow the maximum amount of data allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataLengthInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request contained too many or too few unique data items, or the request would overflow the maximum amount of data allowed"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::DataKeyInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_DataKeyInvalid", "Request contained data with an invalid key"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request contained data with an invalid key"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request contained data with an invalid key"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request contained data with an invalid key"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request contained data with an invalid key"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request contained data with an invalid key"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_length_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_DataKeyLengthInvalid", "Request contained data with a key too long or too short"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request contained data with a key too long or too short"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request contained data with a key too long or too short"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request contained data with a key too long or too short"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request contained data with a key too long or too short"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataKeyLengthInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_key_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request contained data with a key too long or too short"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::DataValueInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_DataValueInvalid", "Request contained data with an invalid value"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request contained data with an invalid value"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request contained data with an invalid value"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request contained data with an invalid value"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request contained data with an invalid value"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request contained data with an invalid value"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_length_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_DataValueLengthInvalid", "Request contained data with a value too long"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request contained data with a value too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request contained data with a value too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request contained data with a value too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request contained data with a value too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::DataValueLengthInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.data_value_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request contained data with a value too long"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::RichTextInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_RichTextInvalid", "Request contained an invalid rich text string"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request contained an invalid rich text string"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request contained an invalid rich text string"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request contained an invalid rich text string"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request contained an invalid rich text string"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request contained an invalid rich text string"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_length_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_RichTextLengthInvalid", "Request contained a rich text string that was too long"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request contained a rich text string that was too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request contained a rich text string that was too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request contained a rich text string that was too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request contained a rich text string that was too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::RichTextLengthInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.rich_text_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request contained a rich text string that was too long"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Presence::StatusInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.status_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Presence_StatusInvalid", "Request contained an invalid status state"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::StatusInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.status_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request contained an invalid status state"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::StatusInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.status_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request contained an invalid status state"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::StatusInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.status_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request contained an invalid status state"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::StatusInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.status_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request contained an invalid status state"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Presence::StatusInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.presence.status_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request contained an invalid status state"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Ecom::EntitlementStale()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.entitlement_stale"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Ecom_EntitlementStale", "The entitlement retrieved is stale, requery for updated information"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::EntitlementStale(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.entitlement_stale"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The entitlement retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::EntitlementStale(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.entitlement_stale"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The entitlement retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::EntitlementStale(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.entitlement_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The entitlement retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::EntitlementStale(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.entitlement_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The entitlement retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::EntitlementStale(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.entitlement_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The entitlement retrieved is stale, requery for updated information"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_stale"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Ecom_CatalogOfferStale", "The offer retrieved is stale, requery for updated information"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_stale"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The offer retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_stale"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The offer retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The offer retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The offer retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferStale(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The offer retrieved is stale, requery for updated information"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Ecom::CatalogItemStale()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_item_stale"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Ecom_CatalogItemStale", "The item or associated structure retrieved is stale, requery for updated information"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_item_stale"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The item or associated structure retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_item_stale"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The item or associated structure retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_item_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The item or associated structure retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_item_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The item or associated structure retrieved is stale, requery for updated information"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogItemStale(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_item_stale"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The item or associated structure retrieved is stale, requery for updated information"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_price_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Ecom_CatalogOfferPriceInvalid", "The one or more offers has an invalid price. This may be caused by the price setup."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_price_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The one or more offers has an invalid price. This may be caused by the price setup."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_price_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The one or more offers has an invalid price. This may be caused by the price setup."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_price_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The one or more offers has an invalid price. This may be caused by the price setup."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_price_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The one or more offers has an invalid price. This may be caused by the price setup."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CatalogOfferPriceInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.catalog_offer_price_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The one or more offers has an invalid price. This may be caused by the price setup."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.checkout_load_error"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Ecom_CheckoutLoadError", "The checkout page failed to load"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.checkout_load_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The checkout page failed to load"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.checkout_load_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The checkout page failed to load"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.checkout_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The checkout page failed to load"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.checkout_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The checkout page failed to load"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Ecom::CheckoutLoadError(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ecom.checkout_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The checkout page failed to load"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::SessionInProgress()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_in_progress"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_SessionInProgress", "Session is already in progress"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionInProgress(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Session is already in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionInProgress(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Session is already in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionInProgress(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Session is already in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionInProgress(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Session is already in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionInProgress(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Session is already in progress"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::TooManyPlayers()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_players"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_TooManyPlayers", "Too many players to register with this session"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_players"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Too many players to register with this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_players"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Too many players to register with this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_players"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Too many players to register with this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_players"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Too many players to register with this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyPlayers(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_players"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Too many players to register with this session"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::NoPermission()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.no_permission"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_NoPermission", "Client has no permissions to access this session"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NoPermission(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.no_permission"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Client has no permissions to access this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NoPermission(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.no_permission"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Client has no permissions to access this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NoPermission(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.no_permission"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Client has no permissions to access this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NoPermission(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.no_permission"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Client has no permissions to access this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NoPermission(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.no_permission"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Client has no permissions to access this session"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_already_exists"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_SessionAlreadyExists", "Session already exists in the system"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_already_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Session already exists in the system"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_already_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Session already exists in the system"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Session already exists in the system"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Session already exists in the system"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionAlreadyExists(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Session already exists in the system"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::InvalidLock()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_lock"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_InvalidLock", "Session lock required for operation"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidLock(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Session lock required for operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidLock(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Session lock required for operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidLock(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Session lock required for operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidLock(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Session lock required for operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidLock(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Session lock required for operation"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::InvalidSession()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_session"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_InvalidSession", "Invalid session reference"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidSession(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_session"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invalid session reference"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidSession(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_session"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invalid session reference"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidSession(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_session"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invalid session reference"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidSession(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_session"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invalid session reference"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InvalidSession(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invalid_session"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invalid session reference"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_not_allowed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_SandboxNotAllowed", "Sandbox id associated with auth didn't match"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxNotAllowed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::InviteFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_InviteFailed", "Invite failed to send"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invite failed to send"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invite failed to send"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invite failed to send"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invite failed to send"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invite failed to send"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::InviteNotFound()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_not_found"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_InviteNotFound", "Invite was not found with the service"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteNotFound(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invite was not found with the service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteNotFound(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invite was not found with the service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteNotFound(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invite was not found with the service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteNotFound(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invite was not found with the service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::InviteNotFound(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invite was not found with the service"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.upsert_not_allowed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_UpsertNotAllowed", "This client may not modify the session"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("This client may not modify the session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("This client may not modify the session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("This client may not modify the session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("This client may not modify the session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::UpsertNotAllowed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("This client may not modify the session"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::AggregationFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.aggregation_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_AggregationFailed", "Backend nodes unavailable to process request"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::AggregationFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::AggregationFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::AggregationFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::AggregationFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::AggregationFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::HostAtCapacity()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.host_at_capacity"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_HostAtCapacity", "Individual backend node is as capacity"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Individual backend node is as capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Individual backend node is as capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Individual backend node is as capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Individual backend node is as capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::HostAtCapacity(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Individual backend node is as capacity"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_at_capacity"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_SandboxAtCapacity", "Sandbox on node is at capacity"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SandboxAtCapacity(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_not_anonymous"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_SessionNotAnonymous", "An anonymous operation was attempted on a non anonymous session"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_not_anonymous"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("An anonymous operation was attempted on a non anonymous session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_not_anonymous"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("An anonymous operation was attempted on a non anonymous session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_not_anonymous"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("An anonymous operation was attempted on a non anonymous session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_not_anonymous"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("An anonymous operation was attempted on a non anonymous session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::SessionNotAnonymous(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.session_not_anonymous"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("An anonymous operation was attempted on a non anonymous session"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::OutOfSync()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.out_of_sync"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_OutOfSync", "Session is currently out of sync with the backend, data is saved locally but needs to sync with backend"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::OutOfSync(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.out_of_sync"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Session is currently out of sync with the backend, data is saved locally but needs to sync with backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::OutOfSync(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.out_of_sync"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Session is currently out of sync with the backend, data is saved locally but needs to sync with backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::OutOfSync(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.out_of_sync"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Session is currently out of sync with the backend, data is saved locally but needs to sync with backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::OutOfSync(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.out_of_sync"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Session is currently out of sync with the backend, data is saved locally but needs to sync with backend"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::OutOfSync(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.out_of_sync"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Session is currently out of sync with the backend, data is saved locally but needs to sync with backend"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::TooManyInvites()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_invites"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_TooManyInvites", "User has received too many invites"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyInvites(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User has received too many invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyInvites(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User has received too many invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyInvites(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User has received too many invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyInvites(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User has received too many invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::TooManyInvites(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User has received too many invites"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.presence_session_exists"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_PresenceSessionExists", "Presence session already exists for the client"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.presence_session_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Presence session already exists for the client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.presence_session_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Presence session already exists for the client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.presence_session_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Presence session already exists for the client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.presence_session_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Presence session already exists for the client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PresenceSessionExists(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.presence_session_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Presence session already exists for the client"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.deployment_at_capacity"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_DeploymentAtCapacity", "Deployment on node is at capacity"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Deployment on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Deployment on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Deployment on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Deployment on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::DeploymentAtCapacity(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Deployment on node is at capacity"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::NotAllowed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.not_allowed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_NotAllowed", "Session operation not allowed"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NotAllowed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Session operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NotAllowed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Session operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NotAllowed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Session operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NotAllowed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Session operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::NotAllowed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Session operation not allowed"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.player_sanctioned"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Sessions_PlayerSanctioned", "Session operation not allowed"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.player_sanctioned"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Session operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.player_sanctioned"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Session operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.player_sanctioned"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Session operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.player_sanctioned"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Session operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Sessions::PlayerSanctioned(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.sessions.player_sanctioned"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Session operation not allowed"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_FilenameInvalid", "Request filename was invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request filename was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request filename was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request filename was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request filename was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request filename was invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_length_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_FilenameLengthInvalid", "Request filename was too long"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request filename was too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request filename was too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request filename was too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request filename was too long"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameLengthInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request filename was too long"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid_chars"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_FilenameInvalidChars", "Request filename contained invalid characters"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid_chars"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request filename contained invalid characters"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid_chars"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request filename contained invalid characters"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid_chars"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request filename contained invalid characters"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid_chars"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request filename contained invalid characters"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FilenameInvalidChars(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.filename_invalid_chars"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request filename contained invalid characters"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_too_large"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_FileSizeTooLarge", "Request operation would grow file too large"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_too_large"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request operation would grow file too large"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_too_large"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request operation would grow file too large"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_too_large"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request operation would grow file too large"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_too_large"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request operation would grow file too large"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeTooLarge(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_too_large"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request operation would grow file too large"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_FileSizeInvalid", "Request file length is not valid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request file length is not valid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request file length is not valid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request file length is not valid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request file length is not valid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileSizeInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_size_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request file length is not valid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_handle_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_FileHandleInvalid", "Request file handle is not valid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_handle_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request file handle is not valid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_handle_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request file handle is not valid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_handle_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request file handle is not valid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_handle_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request file handle is not valid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHandleInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_handle_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request file handle is not valid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_DataInvalid", "Request data is invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request data is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request data is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request data is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request data is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request data is invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_length_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_DataLengthInvalid", "Request data length was invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request data length was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request data length was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request data length was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request data length was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::DataLengthInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.data_length_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request data length was invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.start_index_invalid"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_StartIndexInvalid", "Request start index was invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.start_index_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request start index was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.start_index_invalid"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request start index was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.start_index_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request start index was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.start_index_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request start index was invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::StartIndexInvalid(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.start_index_invalid"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request start index was invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.request_in_progress"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_RequestInProgress", "Request is in progress"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request is in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request is in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request is in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request is in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::RequestInProgress(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.request_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request is in progress"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_throttled"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_UserThrottled", "User is marked as throttled which means he can't perform some operations because limits are exceeded."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_throttled"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User is marked as throttled which means he can't perform some operations because limits are exceeded."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_throttled"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User is marked as throttled which means he can't perform some operations because limits are exceeded."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_throttled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User is marked as throttled which means he can't perform some operations because limits are exceeded."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_throttled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User is marked as throttled which means he can't perform some operations because limits are exceeded."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserThrottled(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_throttled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User is marked as throttled which means he can't perform some operations because limits are exceeded."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.encryption_key_not_set"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_EncryptionKeyNotSet", "Encryption key is not set during SDK init."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Encryption key is not set during SDK init."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Encryption key is not set during SDK init."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Encryption key is not set during SDK init."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Encryption key is not set during SDK init."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::EncryptionKeyNotSet(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Encryption key is not set during SDK init."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_error_from_data_callback"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_UserErrorFromDataCallback", "User data callback returned error (EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest or EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest)"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User data callback returned error (EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest or EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User data callback returned error (EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest or EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User data callback returned error (EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest or EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User data callback returned error (EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest or EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest)"))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::UserErrorFromDataCallback(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User data callback returned error (EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest or EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest)"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_header_has_newer_version"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_FileHeaderHasNewerVersion", "User is trying to read file that has header from newer version of SDK. Game/SDK needs to be updated."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User is trying to read file that has header from newer version of SDK. Game/SDK needs to be updated."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User is trying to read file that has header from newer version of SDK. Game/SDK needs to be updated."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User is trying to read file that has header from newer version of SDK. Game/SDK needs to be updated."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User is trying to read file that has header from newer version of SDK. Game/SDK needs to be updated."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileHeaderHasNewerVersion(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User is trying to read file that has header from newer version of SDK. Game/SDK needs to be updated."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_corrupted"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_PlayerDataStorage_FileCorrupted", "The file is corrupted. In some cases retry can fix the issue."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The file is corrupted. In some cases retry can fix the issue."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The file is corrupted. In some cases retry can fix the issue."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The file is corrupted. In some cases retry can fix the issue."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The file is corrupted. In some cases retry can fix the issue."))));
}
const FOnlineError OnlineRedpointEOS::Errors::PlayerDataStorage::FileCorrupted(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.playerdatastorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The file is corrupted. In some cases retry can fix the issue."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_token_validation_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_ExternalTokenValidationFailed", "EOS Auth service deemed the external token invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_token_validation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth service deemed the external token invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_token_validation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth service deemed the external token invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_token_validation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth service deemed the external token invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_token_validation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth service deemed the external token invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalTokenValidationFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_token_validation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth service deemed the external token invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::UserAlreadyExists()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.user_already_exists"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_UserAlreadyExists", "EOS Auth user already exists"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.user_already_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth user already exists"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.user_already_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth user already exists"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.user_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth user already exists"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.user_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth user already exists"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UserAlreadyExists(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.user_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth user already exists"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::AuthExpired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.auth_expired"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_AuthExpired", "EOS Auth expired"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::AuthExpired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.auth_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::AuthExpired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.auth_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::AuthExpired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.auth_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::AuthExpired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.auth_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth expired"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::AuthExpired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.auth_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth expired"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::InvalidToken()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.invalid_token"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_InvalidToken", "EOS Auth invalid token"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::InvalidToken(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.invalid_token"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth invalid token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::InvalidToken(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.invalid_token"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth invalid token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::InvalidToken(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.invalid_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth invalid token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::InvalidToken(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.invalid_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth invalid token"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::InvalidToken(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.invalid_token"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth invalid token"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.unsupported_token_type"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_UnsupportedTokenType", "EOS Auth doesn't support this token type"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.unsupported_token_type"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth doesn't support this token type"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.unsupported_token_type"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth doesn't support this token type"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.unsupported_token_type"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth doesn't support this token type"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.unsupported_token_type"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth doesn't support this token type"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::UnsupportedTokenType(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.unsupported_token_type"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth doesn't support this token type"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::LinkAccountFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_LinkAccountFailed", "EOS Auth Account link failure"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth Account link failure"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth Account link failure"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth Account link failure"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth Account link failure"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth Account link failure"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_unavailable"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_ExternalServiceUnavailable", "EOS Auth External service for validation was unavailable"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_unavailable"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth External service for validation was unavailable"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_unavailable"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth External service for validation was unavailable"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_unavailable"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth External service for validation was unavailable"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_unavailable"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth External service for validation was unavailable"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceUnavailable(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_unavailable"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth External service for validation was unavailable"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_configuration_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_ExternalServiceConfigurationFailure", "EOS Auth External Service configuration failure with Dev Portal"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_configuration_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth External Service configuration failure with Dev Portal"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_configuration_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth External Service configuration failure with Dev Portal"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_configuration_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth External Service configuration failure with Dev Portal"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_configuration_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth External Service configuration failure with Dev Portal"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::ExternalServiceConfigurationFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.external_service_configuration_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth External Service configuration failure with Dev Portal"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed_missing_nintendo_id_account"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Connect_LinkAccountFailedMissingNintendoIdAccount", "EOS Auth Account link failure. Tried to link Nintendo Network Service Account without first linking Nintendo Account"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed_missing_nintendo_id_account"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Auth Account link failure. Tried to link Nintendo Network Service Account without first linking Nintendo Account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed_missing_nintendo_id_account"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Auth Account link failure. Tried to link Nintendo Network Service Account without first linking Nintendo Account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed_missing_nintendo_id_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Auth Account link failure. Tried to link Nintendo Network Service Account without first linking Nintendo Account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed_missing_nintendo_id_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Auth Account link failure. Tried to link Nintendo Network Service Account without first linking Nintendo Account"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Connect::LinkAccountFailedMissingNintendoIdAccount(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.connect.link_account_failed_missing_nintendo_id_account"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Auth Account link failure. Tried to link Nintendo Network Service Account without first linking Nintendo Account"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ui.social_overlay_load_error"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_UI_SocialOverlayLoadError", "The social overlay page failed to load"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ui.social_overlay_load_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The social overlay page failed to load"))));
}
const FOnlineError OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ui.social_overlay_load_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The social overlay page failed to load"))));
}
const FOnlineError OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ui.social_overlay_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The social overlay page failed to load"))));
}
const FOnlineError OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ui.social_overlay_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The social overlay page failed to load"))));
}
const FOnlineError OnlineRedpointEOS::Errors::UI::SocialOverlayLoadError(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.ui.social_overlay_load_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The social overlay page failed to load"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::NotOwner()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_owner"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_NotOwner", "Client has no permissions to modify this lobby"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotOwner(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_owner"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Client has no permissions to modify this lobby"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotOwner(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_owner"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Client has no permissions to modify this lobby"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotOwner(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_owner"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Client has no permissions to modify this lobby"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotOwner(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_owner"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Client has no permissions to modify this lobby"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotOwner(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_owner"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Client has no permissions to modify this lobby"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::InvalidLock()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_lock"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_InvalidLock", "Lobby lock required for operation"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidLock(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Lobby lock required for operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidLock(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Lobby lock required for operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidLock(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Lobby lock required for operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidLock(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Lobby lock required for operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidLock(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_lock"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Lobby lock required for operation"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.lobby_already_exists"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_LobbyAlreadyExists", "Lobby already exists in the system"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.lobby_already_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Lobby already exists in the system"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.lobby_already_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Lobby already exists in the system"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.lobby_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Lobby already exists in the system"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.lobby_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Lobby already exists in the system"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::LobbyAlreadyExists(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.lobby_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Lobby already exists in the system"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::SessionInProgress()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.session_in_progress"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_SessionInProgress", "Lobby is already in progress"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SessionInProgress(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Lobby is already in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SessionInProgress(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Lobby is already in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SessionInProgress(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Lobby is already in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SessionInProgress(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Lobby is already in progress"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SessionInProgress(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.session_in_progress"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Lobby is already in progress"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::TooManyPlayers()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_players"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_TooManyPlayers", "Too many players to register with this session"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_players"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Too many players to register with this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_players"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Too many players to register with this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_players"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Too many players to register with this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_players"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Too many players to register with this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyPlayers(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_players"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Too many players to register with this session"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::NoPermission()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.no_permission"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_NoPermission", "Client has no permissions to access this session"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NoPermission(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.no_permission"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Client has no permissions to access this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NoPermission(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.no_permission"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Client has no permissions to access this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NoPermission(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.no_permission"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Client has no permissions to access this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NoPermission(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.no_permission"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Client has no permissions to access this session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NoPermission(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.no_permission"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Client has no permissions to access this session"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::InvalidSession()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_session"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_InvalidSession", "Invalid session reference"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidSession(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_session"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invalid session reference"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidSession(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_session"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invalid session reference"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidSession(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_session"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invalid session reference"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidSession(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_session"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invalid session reference"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InvalidSession(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invalid_session"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invalid session reference"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_not_allowed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_SandboxNotAllowed", "Sandbox id associated with auth didn't match"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxNotAllowed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Sandbox id associated with auth didn't match"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::InviteFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_InviteFailed", "Invite failed to send"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invite failed to send"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invite failed to send"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invite failed to send"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invite failed to send"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invite failed to send"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::InviteNotFound()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_not_found"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_InviteNotFound", "Invite was not found with the service"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteNotFound(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invite was not found with the service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteNotFound(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invite was not found with the service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteNotFound(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invite was not found with the service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteNotFound(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invite was not found with the service"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::InviteNotFound(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.invite_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invite was not found with the service"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.upsert_not_allowed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_UpsertNotAllowed", "This client may not modify the session"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("This client may not modify the session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("This client may not modify the session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("This client may not modify the session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("This client may not modify the session"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::UpsertNotAllowed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.upsert_not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("This client may not modify the session"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::AggregationFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.aggregation_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_AggregationFailed", "Backend nodes unavailable to process request"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::AggregationFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::AggregationFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::AggregationFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::AggregationFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::AggregationFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.aggregation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Backend nodes unavailable to process request"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::HostAtCapacity()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.host_at_capacity"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_HostAtCapacity", "Individual backend node is as capacity"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Individual backend node is as capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Individual backend node is as capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Individual backend node is as capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Individual backend node is as capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::HostAtCapacity(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.host_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Individual backend node is as capacity"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_at_capacity"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_SandboxAtCapacity", "Sandbox on node is at capacity"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::SandboxAtCapacity(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.sandbox_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Sandbox on node is at capacity"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::TooManyInvites()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_invites"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_TooManyInvites", "User has received too many invites"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyInvites(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User has received too many invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyInvites(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User has received too many invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyInvites(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User has received too many invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyInvites(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User has received too many invites"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::TooManyInvites(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.too_many_invites"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User has received too many invites"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.deployment_at_capacity"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_DeploymentAtCapacity", "Deployment on node is at capacity"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Deployment on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Deployment on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Deployment on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Deployment on node is at capacity"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::DeploymentAtCapacity(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.deployment_at_capacity"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Deployment on node is at capacity"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::NotAllowed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_allowed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_NotAllowed", "Lobby operation not allowed"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotAllowed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Lobby operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotAllowed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_allowed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Lobby operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotAllowed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Lobby operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotAllowed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Lobby operation not allowed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::NotAllowed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.not_allowed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Lobby operation not allowed"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.member_update_only"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_MemberUpdateOnly", "While restoring a lost connection lobby ownership changed and only local member data was updated"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.member_update_only"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("While restoring a lost connection lobby ownership changed and only local member data was updated"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.member_update_only"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("While restoring a lost connection lobby ownership changed and only local member data was updated"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.member_update_only"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("While restoring a lost connection lobby ownership changed and only local member data was updated"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.member_update_only"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("While restoring a lost connection lobby ownership changed and only local member data was updated"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::MemberUpdateOnly(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.member_update_only"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("While restoring a lost connection lobby ownership changed and only local member data was updated"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.presence_lobby_exists"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_PresenceLobbyExists", "Presence lobby already exists for the client"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.presence_lobby_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Presence lobby already exists for the client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.presence_lobby_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Presence lobby already exists for the client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.presence_lobby_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Presence lobby already exists for the client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.presence_lobby_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Presence lobby already exists for the client"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::PresenceLobbyExists(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.presence_lobby_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Presence lobby already exists for the client"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.voice_not_enabled"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Lobby_VoiceNotEnabled", "Operation requires lobby with voice enabled"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.voice_not_enabled"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Operation requires lobby with voice enabled"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.voice_not_enabled"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Operation requires lobby with voice enabled"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.voice_not_enabled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Operation requires lobby with voice enabled"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.voice_not_enabled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Operation requires lobby with voice enabled"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Lobby::VoiceNotEnabled(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.lobby.voice_not_enabled"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Operation requires lobby with voice enabled"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.user_error_from_data_callback"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_TitleStorage_UserErrorFromDataCallback", "User callback that receives data from storage returned error."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User callback that receives data from storage returned error."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User callback that receives data from storage returned error."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User callback that receives data from storage returned error."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User callback that receives data from storage returned error."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::UserErrorFromDataCallback(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.user_error_from_data_callback"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User callback that receives data from storage returned error."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.encryption_key_not_set"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_TitleStorage_EncryptionKeyNotSet", "User forgot to set Encryption key during platform init. Title Storage can't work without it."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("User forgot to set Encryption key during platform init. Title Storage can't work without it."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("User forgot to set Encryption key during platform init. Title Storage can't work without it."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("User forgot to set Encryption key during platform init. Title Storage can't work without it."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("User forgot to set Encryption key during platform init. Title Storage can't work without it."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::EncryptionKeyNotSet(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.encryption_key_not_set"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("User forgot to set Encryption key during platform init. Title Storage can't work without it."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_corrupted"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_TitleStorage_FileCorrupted", "Downloaded file is corrupted."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Downloaded file is corrupted."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Downloaded file is corrupted."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Downloaded file is corrupted."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Downloaded file is corrupted."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileCorrupted(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_corrupted"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Downloaded file is corrupted."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_header_has_newer_version"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_TitleStorage_FileHeaderHasNewerVersion", "Downloaded file's format is newer than client SDK version."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Downloaded file's format is newer than client SDK version."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Downloaded file's format is newer than client SDK version."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Downloaded file's format is newer than client SDK version."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Downloaded file's format is newer than client SDK version."))));
}
const FOnlineError OnlineRedpointEOS::Errors::TitleStorage::FileHeaderHasNewerVersion(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.titlestorage.file_header_has_newer_version"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Downloaded file's format is newer than client SDK version."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_is_already_running"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_ModSdkProcessIsAlreadyRunning", "ModSdk process is already running. This error comes from the EOSSDK."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_is_already_running"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("ModSdk process is already running. This error comes from the EOSSDK."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_is_already_running"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("ModSdk process is already running. This error comes from the EOSSDK."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_is_already_running"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("ModSdk process is already running. This error comes from the EOSSDK."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_is_already_running"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("ModSdk process is already running. This error comes from the EOSSDK."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessIsAlreadyRunning(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_is_already_running"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("ModSdk process is already running. This error comes from the EOSSDK."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_command_is_empty"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_ModSdkCommandIsEmpty", "ModSdk command is empty. Either the ModSdk configuration file is missing or the manifest location is empty."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_command_is_empty"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("ModSdk command is empty. Either the ModSdk configuration file is missing or the manifest location is empty."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_command_is_empty"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("ModSdk command is empty. Either the ModSdk configuration file is missing or the manifest location is empty."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_command_is_empty"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("ModSdk command is empty. Either the ModSdk configuration file is missing or the manifest location is empty."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_command_is_empty"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("ModSdk command is empty. Either the ModSdk configuration file is missing or the manifest location is empty."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkCommandIsEmpty(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_command_is_empty"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("ModSdk command is empty. Either the ModSdk configuration file is missing or the manifest location is empty."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_creation_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_ModSdkProcessCreationFailed", "Creation of the ModSdk process failed. This error comes from the SDK."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_creation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Creation of the ModSdk process failed. This error comes from the SDK."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_creation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Creation of the ModSdk process failed. This error comes from the SDK."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_creation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Creation of the ModSdk process failed. This error comes from the SDK."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_creation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Creation of the ModSdk process failed. This error comes from the SDK."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModSdkProcessCreationFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_sdk_process_creation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Creation of the ModSdk process failed. This error comes from the SDK."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::CriticalError()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.critical_error"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_CriticalError", "A critical error occurred in the external ModSdk process that we were unable to resolve."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CriticalError(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.critical_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("A critical error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CriticalError(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.critical_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("A critical error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CriticalError(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.critical_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("A critical error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CriticalError(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.critical_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("A critical error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CriticalError(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.critical_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("A critical error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::ToolInternalError()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.tool_internal_error"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_ToolInternalError", "A internal error occurred in the external ModSdk process that we were unable to resolve."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ToolInternalError(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.tool_internal_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("A internal error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ToolInternalError(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.tool_internal_error"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("A internal error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ToolInternalError(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.tool_internal_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("A internal error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ToolInternalError(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.tool_internal_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("A internal error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ToolInternalError(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.tool_internal_error"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("A internal error occurred in the external ModSdk process that we were unable to resolve."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::IPCFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.ipc_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_IPCFailure", "A IPC failure occurred in the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::IPCFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.ipc_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("A IPC failure occurred in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::IPCFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.ipc_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("A IPC failure occurred in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::IPCFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.ipc_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("A IPC failure occurred in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::IPCFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.ipc_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("A IPC failure occurred in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::IPCFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.ipc_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("A IPC failure occurred in the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_ipc_response"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_InvalidIPCResponse", "A invalid IPC response received in the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_ipc_response"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("A invalid IPC response received in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_ipc_response"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("A invalid IPC response received in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_ipc_response"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("A invalid IPC response received in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_ipc_response"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("A invalid IPC response received in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidIPCResponse(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_ipc_response"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("A invalid IPC response received in the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::URILaunchFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.uri_launch_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_URILaunchFailure", "A URI Launch failure occurred in the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::URILaunchFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.uri_launch_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("A URI Launch failure occurred in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::URILaunchFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.uri_launch_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("A URI Launch failure occurred in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::URILaunchFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.uri_launch_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("A URI Launch failure occurred in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::URILaunchFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.uri_launch_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("A URI Launch failure occurred in the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::URILaunchFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.uri_launch_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("A URI Launch failure occurred in the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_is_not_installed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_ModIsNotInstalled", "Attempting to perform an action with a mod that is not installed. This error comes from the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_is_not_installed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Attempting to perform an action with a mod that is not installed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_is_not_installed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Attempting to perform an action with a mod that is not installed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_is_not_installed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Attempting to perform an action with a mod that is not installed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_is_not_installed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Attempting to perform an action with a mod that is not installed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::ModIsNotInstalled(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.mod_is_not_installed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Attempting to perform an action with a mod that is not installed. This error comes from the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.user_does_not_own_the_game"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_UserDoesNotOwnTheGame", "Attempting to perform an action on a game that the user doesn't own. This error comes from the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.user_does_not_own_the_game"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Attempting to perform an action on a game that the user doesn't own. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.user_does_not_own_the_game"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Attempting to perform an action on a game that the user doesn't own. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.user_does_not_own_the_game"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Attempting to perform an action on a game that the user doesn't own. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.user_does_not_own_the_game"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Attempting to perform an action on a game that the user doesn't own. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UserDoesNotOwnTheGame(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.user_does_not_own_the_game"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Attempting to perform an action on a game that the user doesn't own. This error comes from the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_invalid_result"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_OfferRequestByIdInvalidResult", "Invalid result of the request to get the offer for the mod. This error comes from the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_invalid_result"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Invalid result of the request to get the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_invalid_result"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Invalid result of the request to get the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_invalid_result"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Invalid result of the request to get the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_invalid_result"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Invalid result of the request to get the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdInvalidResult(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_invalid_result"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Invalid result of the request to get the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.could_not_find_offer"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_CouldNotFindOffer", "Could not find the offer for the mod. This error comes from the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.could_not_find_offer"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Could not find the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.could_not_find_offer"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Could not find the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.could_not_find_offer"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Could not find the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.could_not_find_offer"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Could not find the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CouldNotFindOffer(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.could_not_find_offer"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Could not find the offer for the mod. This error comes from the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_OfferRequestByIdFailure", "Request to get the offer for the mod failed. This error comes from the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request to get the offer for the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request to get the offer for the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request to get the offer for the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request to get the offer for the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::OfferRequestByIdFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.offer_request_by_id_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request to get the offer for the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::PurchaseFailure()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.purchase_failure"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_PurchaseFailure", "Request to purchase the mod failed. This error comes from the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::PurchaseFailure(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.purchase_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Request to purchase the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::PurchaseFailure(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.purchase_failure"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Request to purchase the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::PurchaseFailure(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.purchase_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Request to purchase the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::PurchaseFailure(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.purchase_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Request to purchase the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::PurchaseFailure(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.purchase_failure"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Request to purchase the mod failed. This error comes from the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_game_install_info"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_InvalidGameInstallInfo", "Attempting to perform an action on a game that is not installed or is partially installed. This error comes from the external ModSdk process."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_game_install_info"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Attempting to perform an action on a game that is not installed or is partially installed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_game_install_info"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Attempting to perform an action on a game that is not installed or is partially installed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_game_install_info"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Attempting to perform an action on a game that is not installed or is partially installed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_game_install_info"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Attempting to perform an action on a game that is not installed or is partially installed. This error comes from the external ModSdk process."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::InvalidGameInstallInfo(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.invalid_game_install_info"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Attempting to perform an action on a game that is not installed or is partially installed. This error comes from the external ModSdk process."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.cannot_get_manifest_location"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_CannotGetManifestLocation", "Failed to get manifest location. Either the ModSdk configuration file is missing or the manifest location is empty"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.cannot_get_manifest_location"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Failed to get manifest location. Either the ModSdk configuration file is missing or the manifest location is empty"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.cannot_get_manifest_location"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Failed to get manifest location. Either the ModSdk configuration file is missing or the manifest location is empty"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.cannot_get_manifest_location"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Failed to get manifest location. Either the ModSdk configuration file is missing or the manifest location is empty"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.cannot_get_manifest_location"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Failed to get manifest location. Either the ModSdk configuration file is missing or the manifest location is empty"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::CannotGetManifestLocation(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.cannot_get_manifest_location"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Failed to get manifest location. Either the ModSdk configuration file is missing or the manifest location is empty"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Mods::UnsupportedOS()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.unsupported_os"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Mods_UnsupportedOS", "Attempting to perform an action with a mod that does not support the current operating system."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UnsupportedOS(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.unsupported_os"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Attempting to perform an action with a mod that does not support the current operating system."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UnsupportedOS(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.unsupported_os"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Attempting to perform an action with a mod that does not support the current operating system."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UnsupportedOS(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.unsupported_os"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Attempting to perform an action with a mod that does not support the current operating system."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UnsupportedOS(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.unsupported_os"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Attempting to perform an action with a mod that does not support the current operating system."))));
}
const FOnlineError OnlineRedpointEOS::Errors::Mods::UnsupportedOS(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.mods.unsupported_os"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Attempting to perform an action with a mod that does not support the current operating system."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_protection_not_available"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_ClientProtectionNotAvailable", "The anti-cheat client protection is not available. Check that the game was started using the correct launcher."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_protection_not_available"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The anti-cheat client protection is not available. Check that the game was started using the correct launcher."))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_protection_not_available"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The anti-cheat client protection is not available. Check that the game was started using the correct launcher."))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_protection_not_available"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The anti-cheat client protection is not available. Check that the game was started using the correct launcher."))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_protection_not_available"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The anti-cheat client protection is not available. Check that the game was started using the correct launcher."))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProtectionNotAvailable(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_protection_not_available"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The anti-cheat client protection is not available. Check that the game was started using the correct launcher."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::InvalidMode()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.invalid_mode"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_InvalidMode", "The current anti-cheat mode is incorrect for using this API"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.invalid_mode"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The current anti-cheat mode is incorrect for using this API"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.invalid_mode"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The current anti-cheat mode is incorrect for using this API"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.invalid_mode"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The current anti-cheat mode is incorrect for using this API"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.invalid_mode"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The current anti-cheat mode is incorrect for using this API"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::InvalidMode(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.invalid_mode"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The current anti-cheat mode is incorrect for using this API"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_product_id_mismatch"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_ClientProductIdMismatch", "The ProductId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_product_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The ProductId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_product_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The ProductId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_product_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The ProductId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_product_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The ProductId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientProductIdMismatch(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_product_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The ProductId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_sandbox_id_mismatch"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_ClientSandboxIdMismatch", "The SandboxId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_sandbox_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The SandboxId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_sandbox_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The SandboxId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_sandbox_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The SandboxId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_sandbox_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The SandboxId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientSandboxIdMismatch(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_sandbox_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The SandboxId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_session_key_required"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_ProtectMessageSessionKeyRequired", "(ProtectMessage/UnprotectMessage) No session key is available, but it is required to complete this operation"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_session_key_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) No session key is available, but it is required to complete this operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_session_key_required"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) No session key is available, but it is required to complete this operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_session_key_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) No session key is available, but it is required to complete this operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_session_key_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("(ProtectMessage/UnprotectMessage) No session key is available, but it is required to complete this operation"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageSessionKeyRequired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_session_key_required"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) No session key is available, but it is required to complete this operation"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_validation_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_ProtectMessageValidationFailed", "(ProtectMessage/UnprotectMessage) Message integrity is invalid"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_validation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) Message integrity is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_validation_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) Message integrity is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_validation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) Message integrity is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_validation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("(ProtectMessage/UnprotectMessage) Message integrity is invalid"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageValidationFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_validation_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) Message integrity is invalid"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_initialization_failed"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_ProtectMessageInitializationFailed", "(ProtectMessage/UnprotectMessage) Initialization failed"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_initialization_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) Initialization failed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_initialization_failed"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) Initialization failed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_initialization_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) Initialization failed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_initialization_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("(ProtectMessage/UnprotectMessage) Initialization failed"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ProtectMessageInitializationFailed(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.protect_message_initialization_failed"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("(ProtectMessage/UnprotectMessage) Initialization failed"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_already_registered"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_PeerAlreadyRegistered", "(RegisterPeer) Peer is already registered"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_already_registered"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("(RegisterPeer) Peer is already registered"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_already_registered"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("(RegisterPeer) Peer is already registered"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_already_registered"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("(RegisterPeer) Peer is already registered"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_already_registered"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("(RegisterPeer) Peer is already registered"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerAlreadyRegistered(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_already_registered"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("(RegisterPeer) Peer is already registered"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_found"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_PeerNotFound", "(UnregisterPeer) Peer does not exist"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("(UnregisterPeer) Peer does not exist"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_found"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("(UnregisterPeer) Peer does not exist"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("(UnregisterPeer) Peer does not exist"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("(UnregisterPeer) Peer does not exist"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotFound(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_found"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("(UnregisterPeer) Peer does not exist"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_protected"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_PeerNotProtected", "(ReceiveMessageFromPeer) Invalid call: Peer is not protected"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_protected"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("(ReceiveMessageFromPeer) Invalid call: Peer is not protected"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_protected"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("(ReceiveMessageFromPeer) Invalid call: Peer is not protected"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_protected"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("(ReceiveMessageFromPeer) Invalid call: Peer is not protected"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_protected"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("(ReceiveMessageFromPeer) Invalid call: Peer is not protected"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::PeerNotProtected(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.peer_not_protected"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("(ReceiveMessageFromPeer) Invalid call: Peer is not protected"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_deployment_id_mismatch"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_ClientDeploymentIdMismatch", "The DeploymentId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_deployment_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The DeploymentId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_deployment_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The DeploymentId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_deployment_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The DeploymentId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_deployment_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The DeploymentId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::ClientDeploymentIdMismatch(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.client_deployment_id_mismatch"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The DeploymentId provided to the anti-cheat client helper executable does not match what was used to initialize the EOS SDK"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.device_id_auth_is_not_supported"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_AntiCheat_DeviceIdAuthIsNotSupported", "EOS Connect DeviceID auth method is not supported for anti-cheat"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.device_id_auth_is_not_supported"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Connect DeviceID auth method is not supported for anti-cheat"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.device_id_auth_is_not_supported"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Connect DeviceID auth method is not supported for anti-cheat"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.device_id_auth_is_not_supported"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Connect DeviceID auth method is not supported for anti-cheat"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.device_id_auth_is_not_supported"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Connect DeviceID auth method is not supported for anti-cheat"))));
}
const FOnlineError OnlineRedpointEOS::Errors::AntiCheat::DeviceIdAuthIsNotSupported(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.anticheat.device_id_auth_is_not_supported"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Connect DeviceID auth method is not supported for anti-cheat"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RTC::TooManyParticipants()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.too_many_participants"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RTC_TooManyParticipants", "EOS RTC room cannot accept more participants"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::TooManyParticipants(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.too_many_participants"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS RTC room cannot accept more participants"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::TooManyParticipants(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.too_many_participants"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS RTC room cannot accept more participants"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::TooManyParticipants(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.too_many_participants"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS RTC room cannot accept more participants"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::TooManyParticipants(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.too_many_participants"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS RTC room cannot accept more participants"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::TooManyParticipants(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.too_many_participants"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS RTC room cannot accept more participants"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_already_exists"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RTC_RoomAlreadyExists", "EOS RTC room already exists"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_already_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS RTC room already exists"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_already_exists"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS RTC room already exists"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS RTC room already exists"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS RTC room already exists"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomAlreadyExists(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_already_exists"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS RTC room already exists"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RTC::UserKicked()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_kicked"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RTC_UserKicked", "The user kicked out from the room"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserKicked(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_kicked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The user kicked out from the room"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserKicked(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_kicked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The user kicked out from the room"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserKicked(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_kicked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The user kicked out from the room"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserKicked(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_kicked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The user kicked out from the room"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserKicked(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_kicked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The user kicked out from the room"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RTC::UserBanned()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_banned"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RTC_UserBanned", "The user is banned"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserBanned(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_banned"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The user is banned"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserBanned(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_banned"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The user is banned"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserBanned(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_banned"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The user is banned"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserBanned(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_banned"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The user is banned"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserBanned(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_banned"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The user is banned"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RTC::RoomWasLeft()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_was_left"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RTC_RoomWasLeft", "EOS RTC room was left successfully"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomWasLeft(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_was_left"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS RTC room was left successfully"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomWasLeft(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_was_left"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS RTC room was left successfully"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomWasLeft(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_was_left"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS RTC room was left successfully"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomWasLeft(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_was_left"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS RTC room was left successfully"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::RoomWasLeft(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.room_was_left"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS RTC room was left successfully"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.reconnection_timegate_expired"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RTC_ReconnectionTimegateExpired", "Connection dropped due to long timeout"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.reconnection_timegate_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("Connection dropped due to long timeout"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.reconnection_timegate_expired"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("Connection dropped due to long timeout"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.reconnection_timegate_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("Connection dropped due to long timeout"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.reconnection_timegate_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("Connection dropped due to long timeout"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ReconnectionTimegateExpired(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.reconnection_timegate_expired"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("Connection dropped due to long timeout"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RTC::ShutdownInvoked()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.shutdown_invoked"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RTC_ShutdownInvoked", "EOS RTC room was left due to platform release"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.shutdown_invoked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS RTC room was left due to platform release"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.shutdown_invoked"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS RTC room was left due to platform release"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.shutdown_invoked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS RTC room was left due to platform release"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.shutdown_invoked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS RTC room was left due to platform release"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::ShutdownInvoked(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.shutdown_invoked"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS RTC room was left due to platform release"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_is_in_blocklist"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_RTC_UserIsInBlocklist", "EOS RTC operation failed because the user is in the local user's block list"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_is_in_blocklist"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS RTC operation failed because the user is in the local user's block list"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_is_in_blocklist"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS RTC operation failed because the user is in the local user's block list"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_is_in_blocklist"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS RTC operation failed because the user is in the local user's block list"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_is_in_blocklist"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS RTC operation failed because the user is in the local user's block list"))));
}
const FOnlineError OnlineRedpointEOS::Errors::RTC::UserIsInBlocklist(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.rtc.user_is_in_blocklist"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS RTC operation failed because the user is in the local user's block list"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.progressionsnapshot.snapshot_id_unavailable"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_ProgressionSnapshot_SnapshotIdUnavailable", "The number of available Snapshot IDs have all been exhausted."));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.progressionsnapshot.snapshot_id_unavailable"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The number of available Snapshot IDs have all been exhausted."))));
}
const FOnlineError OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.progressionsnapshot.snapshot_id_unavailable"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The number of available Snapshot IDs have all been exhausted."))));
}
const FOnlineError OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.progressionsnapshot.snapshot_id_unavailable"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The number of available Snapshot IDs have all been exhausted."))));
}
const FOnlineError OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.progressionsnapshot.snapshot_id_unavailable"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The number of available Snapshot IDs have all been exhausted."))));
}
const FOnlineError OnlineRedpointEOS::Errors::ProgressionSnapshot::SnapshotIdUnavailable(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.progressionsnapshot.snapshot_id_unavailable"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The number of available Snapshot IDs have all been exhausted."))));
}
const FOnlineError& OnlineRedpointEOS::Errors::KWS::ParentEmailMissing()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.parent_email_missing"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_KWS_ParentEmailMissing", "The KWS user does not have a parental email associated with the account.  The parent account was unlinked or deleted"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.parent_email_missing"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The KWS user does not have a parental email associated with the account.  The parent account was unlinked or deleted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.parent_email_missing"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The KWS user does not have a parental email associated with the account.  The parent account was unlinked or deleted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.parent_email_missing"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The KWS user does not have a parental email associated with the account.  The parent account was unlinked or deleted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.parent_email_missing"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The KWS user does not have a parental email associated with the account.  The parent account was unlinked or deleted"))));
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::ParentEmailMissing(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.parent_email_missing"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The KWS user does not have a parental email associated with the account.  The parent account was unlinked or deleted"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::KWS::UserGraduated()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.user_graduated"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_KWS_UserGraduated", "The KWS user is no longer a minor and trying to update the parent email"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::UserGraduated(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.user_graduated"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("The KWS user is no longer a minor and trying to update the parent email"))));
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::UserGraduated(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.user_graduated"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("The KWS user is no longer a minor and trying to update the parent email"))));
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::UserGraduated(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.user_graduated"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("The KWS user is no longer a minor and trying to update the parent email"))));
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::UserGraduated(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.user_graduated"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("The KWS user is no longer a minor and trying to update the parent email"))));
}
const FOnlineError OnlineRedpointEOS::Errors::KWS::UserGraduated(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.kws.user_graduated"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("The KWS user is no longer a minor and trying to update the parent email"))));
}
const FOnlineError& OnlineRedpointEOS::Errors::Android::JavaVMNotStored()
{
    static FOnlineError Err = ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.android.java_vm_not_stored"), NSLOCTEXT("OnlineSubsystemRedpointEOS", "Error_Android_JavaVMNotStored", "EOS Android VM not stored"));
    return Err;
}
const FOnlineError OnlineRedpointEOS::Errors::Android::JavaVMNotStored(const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.android.java_vm_not_stored"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, *InContextMsg, TEXT("EOS Android VM not stored"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Android::JavaVMNotStored(const TCHAR* InCall, const TCHAR* InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.android.java_vm_not_stored"), FText::FromString(FString::Printf(TEXT("(Call) %s (Context) %s (Message) %s"), InCall, InContextMsg, TEXT("EOS Android VM not stored"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Android::JavaVMNotStored(const FUniqueNetId& InActorId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.android.java_vm_not_stored"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), InCall, *InContextMsg, TEXT("EOS Android VM not stored"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Android::JavaVMNotStored(const FUniqueNetId& InActorId)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.android.java_vm_not_stored"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Message) %s"), *InActorId.ToString(), TEXT("EOS Android VM not stored"))));
}
const FOnlineError OnlineRedpointEOS::Errors::Android::JavaVMNotStored(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR* InCall, const FString& InContextMsg)
{
    return ONLINE_ERROR(EOnlineErrorResult::FailExtended, TEXT("errors.com.redpoint.eos.android.java_vm_not_stored"), FText::FromString(FString::Printf(TEXT("(Actor) %s (Object/Target) %s (Call) %s (Context) %s (Message) %s"), *InActorId.ToString(), *InObjectId, InCall, *InContextMsg, TEXT("EOS Android VM not stored"))));
}

#undef ONLINE_ERROR_NAMESPACE

