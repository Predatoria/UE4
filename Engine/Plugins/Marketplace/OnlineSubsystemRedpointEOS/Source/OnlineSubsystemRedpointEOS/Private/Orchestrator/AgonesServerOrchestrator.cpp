// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_ORCHESTRATORS

#include "./AgonesServerOrchestrator.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IHttpResponse.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

FAgonesServerOrchestrator::FAgonesServerOrchestrator()
    : AgonesHttpServerPort(0)
    , HealthCheckTimerHandle()
    , bHasSentReady(false)
    , bHasSentAllocate(false)
    , bHasSentShutdown(false)
{
}

FAgonesServerOrchestrator::~FAgonesServerOrchestrator()
{
    if (this->HealthCheckTimerHandle.IsValid())
    {
        FUTicker::GetCoreTicker().RemoveTicker(this->HealthCheckTimerHandle);
        this->HealthCheckTimerHandle.Reset();
    }
}

bool FAgonesServerOrchestrator::IsEnabled() const
{
    return !FPlatformMisc::GetEnvironmentVariable(TEXT("AGONES_SDK_HTTP_PORT")).IsEmpty();
}

bool FAgonesServerOrchestrator::Init()
{
    FString Port = FPlatformMisc::GetEnvironmentVariable(TEXT("AGONES_SDK_HTTP_PORT"));
    checkf(!Port.IsEmpty(), TEXT("AGONES_SDK_HTTP_PORT should not be empty; already checked that it wasn't!"));
    this->AgonesHttpServerPort = (int16)FCString::Atoi(*Port);
    checkf(this->AgonesHttpServerPort != 0, TEXT("AGONES_SDK_HTTP_PORT must not be 0!"));

    checkf(!this->HealthCheckTimerHandle.IsValid(), TEXT("FAgonesServerOrchestrator::Init must not be called twice!"));
    this->HealthCheckTimerHandle = FUTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateSP(this, &FAgonesServerOrchestrator::SendHealthCheck),
        1.0f);

    return false;
}

void FAgonesServerOrchestrator::HandleHealthCheck(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr HttpRequest,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded)
{
    if (!bSucceeded)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Agones: Health check failed (generic connection error when trying to contact Agones HTTP sidecar)"));
        return;
    }

    if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Agones: Health check failed (HTTP error code %d from Agones HTTP sidecar)"),
            HttpResponse->GetResponseCode());
        return;
    }
}

bool FAgonesServerOrchestrator::SendHealthCheck(float DeltaSeconds)
{
    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Printf(TEXT("http://localhost:%d/health"), this->AgonesHttpServerPort));
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString("{}");

    Request->OnProcessRequestComplete().BindSP(this, &FAgonesServerOrchestrator::HandleHealthCheck);
    Request->ProcessRequest();

    this->HealthCheckTimerHandle = FUTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateSP(this, &FAgonesServerOrchestrator::SendHealthCheck),
        1.0f);
    return false;
}

bool FAgonesServerOrchestrator::RetryGetPortMappings(
    float DeltaSeconds,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FServerOrchestratorGetPortMappingsComplete OnComplete)
{
    this->GetPortMappings(OnComplete);
    return false;
}

void FAgonesServerOrchestrator::HandleGameServer(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr HttpRequest,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FServerOrchestratorGetPortMappingsComplete OnComplete)
{
    if (!bSucceeded)
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Agones: Unable to get game server ports from Agones HTTP sidecar (generic connection failure), "
                 "retrying in 5 seconds..."));
        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(this, &FAgonesServerOrchestrator::RetryGetPortMappings, OnComplete),
            5.0f);
        return;
    }

    if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Agones: Unable to get game server ports from Agones HTTP sidecar (HTTP result code %d), retrying in "
                 "5 seconds..."),
            HttpResponse->GetResponseCode());
        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(this, &FAgonesServerOrchestrator::RetryGetPortMappings, OnComplete),
            5.0f);
        return;
    }

    const FString Json = HttpResponse->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Agones: Unable to get game server ports from Agones HTTP sidecar (invalid JSON), retrying in 5 "
                 "seconds..."));
        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(this, &FAgonesServerOrchestrator::RetryGetPortMappings, OnComplete),
            5.0f);
        return;
    }

    auto StatusObj = JsonObject->GetObjectField("status");
    if (!StatusObj.IsValid() || !StatusObj->HasField("ports"))
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Agones: Unable to get game server ports from Agones HTTP sidecar (missing 'status.ports' field in "
                 "JSON), retrying in 5 "
                 "seconds..."));
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Agones: The received game server JSON from the Agones HTTP sidecar was: %s"),
            *Json);
        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(this, &FAgonesServerOrchestrator::RetryGetPortMappings, OnComplete),
            5.0f);
        return;
    }

    UE_LOG(LogEOS, Verbose, TEXT("Agones: Received game server JSON from Agones HTTP sidecar: %s"), *Json);

    const TArray<TSharedPtr<FJsonValue>> &PortsArray = JsonObject->GetObjectField("status")->GetArrayField("ports");
    TArray<FServerOrchestratorPortMapping> Results;
    for (const auto &PortValue : PortsArray)
    {
        const auto &PortObject = PortValue->AsObject();

        FName PortName = FName(*PortObject->GetStringField("name"));
        int32 PortPort = PortObject->GetIntegerField("port");

        Results.Add(FServerOrchestratorPortMapping(PortName, (int16)PortPort));
    }
    OnComplete.ExecuteIfBound(Results);
}

void FAgonesServerOrchestrator::HandleLabel(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr HttpRequest,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FServerOrchestratorServingTrafficComplete OnComplete)
{
    if (!bSucceeded)
    {
        // Only warn about this, since Agones might not be new enough to support it.
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Agones: Unable to label game server with session ID; make sure Agones is on a new enough version"));
    }

    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Printf(TEXT("http://localhost:%d/ready"), this->AgonesHttpServerPort));
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString("{}");

    Request->OnProcessRequestComplete().BindSP(this, &FAgonesServerOrchestrator::HandleReady, MoveTemp(OnComplete));
    Request->ProcessRequest();
}

void FAgonesServerOrchestrator::HandleReady(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr HttpRequest,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FServerOrchestratorServingTrafficComplete OnComplete)
{
    if (!bSucceeded)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Agones: Marking game server as ready failed (generic connection error when trying to contact Agones "
                 "HTTP sidecar)"));
        this->bHasSentReady = false;
        OnComplete.ExecuteIfBound();
        return;
    }

    if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Agones: Marking game server as ready failed (HTTP error code %d from Agones HTTP sidecar)"),
            HttpResponse->GetResponseCode());
        this->bHasSentReady = false;
        OnComplete.ExecuteIfBound();
        return;
    }

    UE_LOG(LogEOS, Verbose, TEXT("Agones: Marked game server as ready for connections"));
    OnComplete.ExecuteIfBound();
}

void FAgonesServerOrchestrator::HandleAllocate(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr HttpRequest,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FServerOrchestratorSessionStartedComplete OnComplete)
{
    if (!bSucceeded)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Agones: Marking game server as allocated failed (generic connection error when trying to contact "
                 "Agones HTTP sidecar)"));
        this->bHasSentAllocate = false;
        OnComplete.ExecuteIfBound();
        return;
    }

    if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Agones: Marking game server as ready allocated (HTTP error code %d from Agones HTTP sidecar)"),
            HttpResponse->GetResponseCode());
        this->bHasSentAllocate = false;
        OnComplete.ExecuteIfBound();
        return;
    }

    UE_LOG(LogEOS, Verbose, TEXT("Agones: Marked game server as allocated"));
    OnComplete.ExecuteIfBound();
}

void FAgonesServerOrchestrator::HandleShutdown(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr HttpRequest,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FServerOrchestratorShutdownComplete OnComplete)
{
    if (!bSucceeded)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Agones: Shutting down game server failed (generic connection error when trying to contact "
                 "Agones HTTP sidecar)"));
        this->bHasSentAllocate = false;
        OnComplete.ExecuteIfBound();
        return;
    }

    if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Agones: Shutting down game server failed (HTTP error code %d from Agones HTTP sidecar)"),
            HttpResponse->GetResponseCode());
        this->bHasSentAllocate = false;
        OnComplete.ExecuteIfBound();
        return;
    }

    UE_LOG(LogEOS, Verbose, TEXT("Agones: Notified that the game server should shutdown"));
    OnComplete.ExecuteIfBound();
}

bool FAgonesServerOrchestrator::GetPortMappings(const FServerOrchestratorGetPortMappingsComplete &OnComplete)
{
    checkf(this->AgonesHttpServerPort != 0, TEXT("FAgonesServerOrchestrator::Init must have been called!"));

    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Printf(TEXT("http://localhost:%d/gameserver"), this->AgonesHttpServerPort));
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString("");

    Request->OnProcessRequestComplete().BindSP(this, &FAgonesServerOrchestrator::HandleGameServer, OnComplete);
    Request->ProcessRequest();

    return true;
}

bool FAgonesServerOrchestrator::ServingTraffic(
    const FName &SessionName,
    const FString &SessionId,
    const FServerOrchestratorServingTrafficComplete &OnComplete)
{
    checkf(this->AgonesHttpServerPort != 0, TEXT("FAgonesServerOrchestrator::Init must have been called!"));

    if (this->bHasSentReady)
    {
        // We've already told Agones we're ready.
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Agones: Already informed the allocator that this game server is ready. Once the match ends, you "
                 "should terminate the game server instead of re-using the same process."));
        OnComplete.ExecuteIfBound();
        return true;
    }

    this->bHasSentReady = true;

    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Printf(TEXT("http://localhost:%d/metadata/label"), this->AgonesHttpServerPort));
    Request->SetVerb("PUT");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString(FString::Printf(
        TEXT("{\"key\": \"sessionid-%s\", \"value\": \"%s\"}"),
        *SessionName.ToString().ToLower(),
        *SessionId));

    Request->OnProcessRequestComplete().BindSP(this, &FAgonesServerOrchestrator::HandleLabel, OnComplete);
    Request->ProcessRequest();

    return true;
}

bool FAgonesServerOrchestrator::SessionStarted(const FServerOrchestratorSessionStartedComplete &OnComplete)
{
    checkf(this->AgonesHttpServerPort != 0, TEXT("FAgonesServerOrchestrator::Init must have been called!"));

    if (this->bHasSentAllocate)
    {
        // We've already told Agones we're ready.
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Agones: Already informed the allocator that this game server is allocated. Once the match ends, you "
                 "should call EndSession or DestroySession to get Agones to terminate the game server instead of "
                 "re-using it."));
        OnComplete.ExecuteIfBound();
        return true;
    }

    this->bHasSentAllocate = true;

    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Printf(TEXT("http://localhost:%d/allocate"), this->AgonesHttpServerPort));
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString("{}");

    Request->OnProcessRequestComplete().BindSP(this, &FAgonesServerOrchestrator::HandleAllocate, OnComplete);
    Request->ProcessRequest();

    return true;
}

bool FAgonesServerOrchestrator::ShouldDestroySessionOnEndSession()
{
    return true;
}

bool FAgonesServerOrchestrator::Shutdown(const FServerOrchestratorShutdownComplete &OnComplete)
{
    checkf(this->AgonesHttpServerPort != 0, TEXT("FAgonesServerOrchestrator::Init must have been called!"));

    if (this->bHasSentShutdown)
    {
        // We've already told Agones we're shutting down.
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Agones: Already informed the allocator that this game server is to be shutdown."));
        OnComplete.ExecuteIfBound();
        return true;
    }

    this->bHasSentShutdown = true;

    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Printf(TEXT("http://localhost:%d/shutdown"), this->AgonesHttpServerPort));
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString("{}");

    Request->OnProcessRequestComplete().BindSP(this, &FAgonesServerOrchestrator::HandleAllocate, OnComplete);
    Request->ProcessRequest();

    return true;
}

#endif // #if EOS_HAS_ORCHESTRATORS