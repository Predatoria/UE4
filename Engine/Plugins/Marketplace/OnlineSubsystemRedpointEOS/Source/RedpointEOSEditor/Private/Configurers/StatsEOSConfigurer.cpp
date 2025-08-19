// Copyright June Rhodes. All Rights Reserved.

#include "StatsEOSConfigurer.h"

void FStatsEOSConfigurer::InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    Instance->TypingRules.Empty();
}

void FStatsEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    TArray<FString> StatTypingRules;
    if (Reader.GetArray(TEXT("StatTypingRules"), StatTypingRules))
    {
        Instance->TypingRules.Empty();
        for (const FString &V : StatTypingRules)
        {
            TArray<FString> Components;
            V.ParseIntoArray(Components, TEXT(":"));
            if (Components.Num() == 2)
            {
                EStatTypingRule Rule = EStatTypingRule::Int32;
                if (Components[1] == TEXT("Int32"))
                {
                    Rule = EStatTypingRule::Int32;
                }
                else if (Components[1] == TEXT("Bool"))
                {
                    Rule = EStatTypingRule::Bool;
                }
                else if (Components[1] == TEXT("FloatTruncated"))
                {
                    Rule = EStatTypingRule::FloatTruncated;
                }
                else if (Components[1] == TEXT("FloatEncoded"))
                {
                    Rule = EStatTypingRule::FloatEncoded;
                }
                else if (Components[1] == TEXT("DoubleEncoded"))
                {
                    Rule = EStatTypingRule::DoubleEncoded;
                }
                FStatTypingRule Entry = {};
                Entry.StatName = Components[0];
                Entry.Type = Rule;
                Instance->TypingRules.Add(Entry);
            }
        }
    }
    Reader.GetBool(TEXT("AcceptStatWriteRequestsFromServers"), Instance->AcceptStatWriteRequestsFromServers);
}

void FStatsEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    TArray<FString> StatTypingRules;
    for (const auto &KV : Instance->TypingRules)
    {
        FString Rule;
        switch (KV.Type)
        {
        case EStatTypingRule::Int32:
            Rule = "Int32";
            break;
        case EStatTypingRule::Bool:
            Rule = "Bool";
            break;
        case EStatTypingRule::FloatTruncated:
            Rule = "FloatTruncated";
            break;
        case EStatTypingRule::FloatEncoded:
            Rule = "FloatEncoded";
            break;
        case EStatTypingRule::DoubleEncoded:
            Rule = "DoubleEncoded";
            break;
        }
        if (Rule.IsEmpty())
        {
            continue;
        }
        StatTypingRules.Add(FString::Printf(TEXT("%s:%s"), *KV.StatName, *Rule));
    }
    Writer.ReplaceArray(TEXT("StatTypingRules"), StatTypingRules);
    Writer.SetBool(TEXT("AcceptStatWriteRequestsFromServers"), Instance->AcceptStatWriteRequestsFromServers);
}