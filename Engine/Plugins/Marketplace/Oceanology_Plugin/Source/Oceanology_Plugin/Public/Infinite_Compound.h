// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*=================================================
* FileName: Infinite_Compound.h
*
* Created by: Galidar
* Project name: Oceanology
* Created on: 2020/18/10
*
* =================================================*/
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/SceneComponent.h>
#include "Infinite_Compound.generated.h"

UENUM(BlueprintType)
enum class EInfinityCategory : uint8
{
	InfiniteOcean
};

UCLASS(hidecategories = (Object, Mobility, LOD), ClassGroup = Physics, showcategories = Trigger, meta = (BlueprintSpawnableComponent))
class OCEANOLOGY_PLUGIN_API UInfinite_Compound : public USceneComponent
{
	GENERATED_BODY()

public:


	UInfinite_Compound(const class FObjectInitializer& ObjectInitializer);

	//--------------------------------------------------------------------------------------------------

	UPROPERTY(Category = "Location", BlueprintReadWrite, EditAnywhere)
		bool RealTime = true;

	UPROPERTY(Category = "Location", BlueprintReadWrite, EditAnywhere)
		float TimeJump = 0.0f;

	//--------------------------------------------------------------------------------------------------

	UPROPERTY()
		EInfinityCategory InfinityCategory = EInfinityCategory::InfiniteOcean;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UWorld* Origin;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};