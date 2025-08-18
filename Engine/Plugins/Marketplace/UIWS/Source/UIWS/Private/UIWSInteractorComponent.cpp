// Copyright 2018 Elliot Gray. All Rights Reserved.

#include "UIWSInteractorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"


// Sets default values for this component's properties
UUIWSInteractorComponent::UUIWSInteractorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//Only run this check sometimes
	SetComponentTickInterval(1 / 10);
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUIWSInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	UpdateComponentList();
	if(bShouldInteract)
	{
		EnableInteraction();
	}

	if(bEnableInteractiveStateSwitching && bShouldInteract)
	{
		SetComponentTickEnabled(true);
	}
	
}


void UUIWSInteractorComponent::EnableInteraction()
{
	for (UStaticMeshComponent* sm : Statics)
	{
		//UStaticMeshComponent* SMCast = Cast<UStaticMeshComponent>(sm);
		if (sm)
		{
			sm->SetRenderCustomDepth(true);
			sm->SetCustomDepthStencilValue(1);
		}

	}
	for (USkeletalMeshComponent* sk : Skels)
	{
		if (sk)
		{
			sk->SetRenderCustomDepth(true);
			sk->SetCustomDepthStencilValue(1);
		}
	}
}

void UUIWSInteractorComponent::DisableInteraction()
{
	for (UStaticMeshComponent* sm : Statics)
	{
		//UStaticMeshComponent* SMCast = Cast<UStaticMeshComponent>(sm);
		if (sm)
		{
			sm->SetRenderCustomDepth(false);
			sm->SetCustomDepthStencilValue(0);
		}

	}
	for (USkeletalMeshComponent* sk : Skels)
	{
		if (sk)
		{
			sk->SetRenderCustomDepth(false);
			sk->SetCustomDepthStencilValue(0);
		}
	}
}

// Called every frame
void UUIWSInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(bEnableInteractiveStateSwitching && bShouldInteract)
	{
		if (FMath::Abs(GetOwner()->GetVelocity().Size())>MinInteractionVelocity)
		{
			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Enabling Interaction");
			EnableInteraction();
		}
		else
		{
			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Disabling Interaction");
			DisableInteraction();
		}
	}

}

void UUIWSInteractorComponent::UpdateComponentList()
{
	GetOwner()->GetComponents<UStaticMeshComponent, FDefaultAllocator>(Statics); //->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	GetOwner()->GetComponents<USkeletalMeshComponent, FDefaultAllocator>(Skels); //->GetComponentsByClass(USkeletalMeshComponent::StaticClass());
}

