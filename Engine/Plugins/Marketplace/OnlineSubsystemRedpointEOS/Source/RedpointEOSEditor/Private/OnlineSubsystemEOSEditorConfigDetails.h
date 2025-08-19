// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISettingsCategory.h"

#include "Containers/Map.h"
#include "DetailCategoryBuilder.h"
#include "IDetailCustomization.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include <functional>

// Placing an .eac file next to the executable for testing builds doesn't work yet (it breaks the EAC launcher due to
// some bug on Epic's side). Hide this functionality from Project Settings until it's working again.
#define EAC_CLIENT_MODULE_TESTING_WORKS 0

struct FDynamicDropDownOption
{
    FText DisplayText;
    bool bHiddenByDefault;

    FDynamicDropDownOption(FText InDisplayText, bool bInHiddenByDefault = false)
        : DisplayText(MoveTemp(InDisplayText))
        , bHiddenByDefault(bInHiddenByDefault){};
};

typedef TMap<FName, FDynamicDropDownOption> FDynamicOptionMap;

DECLARE_DELEGATE_RetVal(FDynamicOptionMap, FOnGenerateDropDownMenu);
DECLARE_DELEGATE_OneParam(FDynamicOptionChanged, FName /* NewValue */);
DECLARE_DELEGATE_RetVal(bool, FComputePropertyVisibility);

class FOnlineSubsystemEOSEditorConfigDetails : public IDetailCustomization
{
private:
    TMap<FName, IDetailCategoryBuilder *> Categories;
    TMap<FName, IDetailPropertyRow *> Rows;

    void SetPropertyAsDynamicDropDown(
        class IDetailLayoutBuilder &DetailBuilder,
        FName CategoryName,
        FName PropertyName,
        const FText &Description,
        const FOnGenerateDropDownMenu &GenerateDropDownMenu,
        const FDynamicOptionChanged &OnOptionChanged = FDynamicOptionChanged());

    void SetPropertyAsTrustedPlatforms(
        class IDetailLayoutBuilder &DetailBuilder,
        FName CategoryName,
        FName PropertyName,
        const std::function<ENetworkAuthenticationMode()> &GetEffectiveNetworkAuthenticationMode);

    void AddAntiCheatSigningRow(
        class IDetailLayoutBuilder &DetailBuilder,
        FName CategoryName,
        const std::function<ENetworkAuthenticationMode()> &GetEffectiveNetworkAuthenticationMode);
#if EAC_CLIENT_MODULE_TESTING_WORKS
    void AddAntiCheatClientModuleRow(class IDetailLayoutBuilder &DetailBuilder, FName CategoryName);
#endif

    IDetailPropertyRow &GetOrAddPropertyRow(
        class IDetailLayoutBuilder &DetailBuilder,
        FName CategoryName,
        FName PropertyName);

    void DeclarePropertyOrder(
        class IDetailLayoutBuilder &DetailBuilder,
        FName CategoryName,
        const TArray<FName> &PropertyNames);

    void SetPropertyVisibilityCondition(
        class IDetailLayoutBuilder &DetailBuilder,
        FName CategoryName,
        FName PropertyName,
        const FComputePropertyVisibility &VisibilityCallback);

public:
    FOnlineSubsystemEOSEditorConfigDetails() = default;
    UE_NONCOPYABLE(FOnlineSubsystemEOSEditorConfigDetails);

    static TSharedRef<IDetailCustomization> MakeInstance();

    virtual void CustomizeDetails(class IDetailLayoutBuilder &DetailBuilder) override;
};