// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "UObject/Interface.h"

EOS_ENABLE_STRICT_WARNINGS

template <typename TWidgetInterfaceType, typename TWidgetInterfaceUClassType, typename TWidgetContextType>
class TUserInterfaceRef
    : public TSharedFromThis<TUserInterfaceRef<TWidgetInterfaceType, TWidgetInterfaceUClassType, TWidgetContextType>>
{
private:
    TSoftObjectPtr<UUserWidget> Widget;
    TSoftObjectPtr<TWidgetContextType> Context;
    bool bIsValid;
    FString InvalidErrorMessage;

public:
    TUserInterfaceRef(
        const TSharedRef<FAuthenticationGraphState> &State,
        const FString &InWidgetAssetPath,
        const FString &InRequiredInterfaceName)
        : Widget(nullptr)
        , Context(nullptr)
        , bIsValid(false)
        , InvalidErrorMessage()
    {
        FSoftClassPath WidgetClassPath = FSoftClassPath(InWidgetAssetPath);
        TSubclassOf<UUserWidget> WidgetClass = WidgetClassPath.TryLoadClass<UUserWidget>();
        if (WidgetClass == nullptr)
        {
            this->InvalidErrorMessage = FString::Printf(
                TEXT("TUserInterfaceRef: Unable to locate the UMG widget at %s. Check that it exists and if "
                     "necessary, add it to your Asset Manager configuration so it gets included in packaged builds."),
                *WidgetClassPath.GetAssetPathString());
            return;
        }
        if (!WidgetClass->ImplementsInterface(TWidgetInterfaceUClassType::StaticClass()))
        {
            this->InvalidErrorMessage = FString::Printf(
                TEXT("TUserInterfaceRef: The configured UMG widget does not implement the "
                     "%s interface."),
                *InRequiredInterfaceName);
            return;
        }

        UWorld *WorldRef = State->GetWorld();
        if (WorldRef == nullptr)
        {
            this->InvalidErrorMessage =
                FString::Printf(TEXT("TUserInterfaceRef: There is no active world. A world is required in order to be "
                                     "able to create and display UMG widgets."));
            return;
        }

        this->Widget = CreateWidget<UUserWidget>(WorldRef, WidgetClass);
        if (this->Widget == nullptr)
        {
            this->InvalidErrorMessage = FString::Printf(
                TEXT("PromptToSelectEOSAccountNode: Unable to create UMG widget."),
                *InRequiredInterfaceName);
            return;
        }

        this->Context = NewObject<TWidgetContextType>(this->Widget.Get());

        this->bIsValid = true;
    }

    UE_NONCOPYABLE(TUserInterfaceRef);

    ~TUserInterfaceRef()
    {
        if (this->Widget.IsValid())
        {
            this->Widget->RemoveFromParent();
            this->Widget.Reset();
        }
        if (this->Context.IsValid())
        {
            this->Context.Reset();
        }

        this->bIsValid = false;
    }

    bool IsValid()
    {
        return this->bIsValid && this->Context.IsValid() && this->Widget.IsValid();
    }

    FString GetInvalidErrorMessage()
    {
        return this->InvalidErrorMessage;
    }

    void AddToState(const TSharedRef<FAuthenticationGraphState> &State)
    {
        check(this->IsValid());
        State->SetCurrentUserInterfaceWidget(this->Widget.Get());
    }

    void RemoveFromState(const TSharedRef<FAuthenticationGraphState> &State)
    {
        check(this->IsValid());

        // @todo: Validate that it is our widget that is set.
        State->ClearCurrentUserInterfaceWidget();
    }

    TWidgetContextType *GetContext()
    {
        check(this->IsValid());
        return this->Context.Get();
    }

    UUserWidget *GetWidget()
    {
        check(this->IsValid());
        return this->Widget.Get();
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
