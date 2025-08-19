// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/WorldResolution.h"
#include "OnlineSubsystemUtils.h"
#include "UObject/NameTypes.h"

EOS_ENABLE_STRICT_WARNINGS

template <typename T> struct FDelegatedInterface
{
    FName SubsystemName;
    TWeakPtr<T, ESPMode::ThreadSafe> Implementation;
};

class FDelegatedSubsystems
{
public:
    class ISubsystemContext
    {
        friend class FDelegatedSubsystems;

    private:
        FName SubsystemName;
        UWorld *World;
        IOnlineSubsystem *MainInstance;
        IOnlineSubsystem *RedpointInstance;

    public:
        template <typename T>
        TWeakPtr<T, ESPMode::ThreadSafe> GetDelegatedInterface(
            std::function<TSharedPtr<T, ESPMode::ThreadSafe>(class IOnlineSubsystem *InOnlineSubsystem)> Resolver)
        {
            if (this->MainInstance == nullptr)
            {
                // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
                this->MainInstance = this->World == nullptr ? IOnlineSubsystem::Get(this->SubsystemName)
                                                            : Online::GetSubsystem(this->World, this->SubsystemName);
            }
            if (this->MainInstance != nullptr)
            {
                TSharedPtr<T, ESPMode::ThreadSafe> Implementation = Resolver(MainInstance);
                if (Implementation.IsValid())
                {
                    return Implementation;
                }
            }
            if (this->RedpointInstance == nullptr)
            {
                this->RedpointInstance =
                    this->World == nullptr
                        // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
                        ? IOnlineSubsystem::Get(
                              FName(*FString::Printf(TEXT("Redpoint%s"), *this->SubsystemName.ToString())))
                        : Online::GetSubsystem(
                              this->World,
                              FName(*FString::Printf(TEXT("Redpoint%s"), *this->SubsystemName.ToString())));
            }
            if (this->RedpointInstance != nullptr)
            {
                TSharedPtr<T, ESPMode::ThreadSafe> Implementation = Resolver(RedpointInstance);
                if (Implementation.IsValid())
                {
                    return Implementation;
                }
            }
            return nullptr;
        }
    };

    template <typename T>
    static TMap<FName, TSharedPtr<T>> GetDelegatedSubsystems(
        const TSharedRef<FEOSConfig> &InConfig,
        FName InstanceName,
        std::function<TSharedPtr<T>(ISubsystemContext &InContext)> Construct)
    {
        TSoftObjectPtr<UWorld> World = FWorldResolution::GetWorld(InstanceName, true);
        if (!World.IsValid())
        {
            if (InstanceName.ToString() == TEXT("DefaultInstance"))
            {
                return GetDelegatedSubsystems(InConfig, nullptr, Construct);
            }
            return TMap<FName, TSharedPtr<T>>();
        }
        return GetDelegatedSubsystems(InConfig, World.Get(), Construct);
    }

    template <typename T>
    static TMap<FName, TSharedPtr<T>> GetDelegatedSubsystems(
        const TSharedRef<FEOSConfig> &InConfig,
        UWorld *InWorld,
        std::function<TSharedPtr<T>(ISubsystemContext &InContext)> Construct)
    {
        TMap<FName, TSharedPtr<T>> Results;
        TArray<FString> SubsystemList;
        InConfig->GetDelegatedSubsystemsString().ParseIntoArray(SubsystemList, TEXT(","), true);
        for (const auto &SubsystemName : SubsystemList)
        {
            ISubsystemContext Context = {};
            Context.SubsystemName = FName(*SubsystemName);
            Context.World = InWorld;

            TSharedPtr<T> Value = Construct(Context);
            if (Value.IsValid())
            {
                Results.Add(FName(*SubsystemName), Value);
            }
        }
        return Results;
    }

    template <typename T>
    static TMap<FName, FDelegatedInterface<T>> GetDelegatedInterfaces(
        const TSharedRef<FEOSConfig> &InConfig,
        FName InstanceName,
        std::function<TSharedPtr<T, ESPMode::ThreadSafe>(class IOnlineSubsystem *InOnlineSubsystem)> Resolver)
    {
        TSoftObjectPtr<UWorld> World = FWorldResolution::GetWorld(InstanceName, true);
        if (!World.IsValid())
        {
            if (InstanceName.ToString() == TEXT("DefaultInstance"))
            {
                return GetDelegatedInterfaces(InConfig, nullptr, Resolver);
            }
            return TMap<FName, FDelegatedInterface<T>>();
        }
        return GetDelegatedInterfaces(InConfig, World.Get(), Resolver);
    }

    template <typename T>
    static TMap<FName, FDelegatedInterface<T>> GetDelegatedInterfaces(
        const TSharedRef<FEOSConfig> &InConfig,
        UWorld *InWorld,
        std::function<TSharedPtr<T, ESPMode::ThreadSafe>(class IOnlineSubsystem *InOnlineSubsystem)> Resolver)
    {
        TMap<FName, FDelegatedInterface<T>> Results;
        TArray<FString> SubsystemList;
        InConfig->GetDelegatedSubsystemsString().ParseIntoArray(SubsystemList, TEXT(","), true);
        for (const auto &SubsystemName : SubsystemList)
        {
            // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
            IOnlineSubsystem *MainInstance = InWorld == nullptr ? IOnlineSubsystem::Get(FName(*SubsystemName))
                                                                : Online::GetSubsystem(InWorld, FName(*SubsystemName));
            if (MainInstance != nullptr)
            {
                TSharedPtr<T, ESPMode::ThreadSafe> Implementation = Resolver(MainInstance);
                if (Implementation.IsValid())
                {
                    Results.Add(FName(*SubsystemName), {FName(*SubsystemName), Implementation});
                    continue;
                }
            }
            IOnlineSubsystem *RedpointInstance =
                InWorld == nullptr
                    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
                    ? IOnlineSubsystem::Get(FName(*FString::Printf(TEXT("Redpoint%s"), *SubsystemName)))
                    : Online::GetSubsystem(InWorld, FName(*FString::Printf(TEXT("Redpoint%s"), *SubsystemName)));
            if (RedpointInstance != nullptr)
            {
                TSharedPtr<T, ESPMode::ThreadSafe> Implementation = Resolver(RedpointInstance);
                if (Implementation.IsValid())
                {
                    Results.Add(FName(*SubsystemName), {FName(*SubsystemName), Implementation});
                    continue;
                }
            }
        }
        return Results;
    }
};

EOS_DISABLE_STRICT_WARNINGS
