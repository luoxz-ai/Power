// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Nameplates/RotateNameplateToCamera.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "UI/Nameplates/NameplateWidget.h"
#include "NameplateController.generated.h"

/**
 * 
 */
UCLASS()
class POWER_API UNameplateController : public URotateNameplateToCamera
{
	GENERATED_BODY()

public:
	UNameplateController();
	

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateNameplate();
	
	//UFUNCTION(BlueprintImplementableEvent, Category = "From Cpp")
	//void UpdateNameText();

	/*
	UFUNCTION(BlueprintCallable, Category = "From Cpp")
	void UpdateLevelText(int NewLevel);

	UFUNCTION(BlueprintCallable, Category = "From Cpp")
	void UpdateHealthBar(int CurrentHealth, int MaxHealth);

	UPROPERTY()
	UNameplateWidget* NameplateBlueprint;
	*/
};