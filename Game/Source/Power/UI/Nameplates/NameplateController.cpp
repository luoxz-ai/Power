// Fill out your copyright notice in the Description page of Project Settings.

#include "NameplateController.h"
#include "Engine.h"
#include "Engine/Blueprint.h"
#include "Blueprint/UserWidget.h"

UNameplateController::UNameplateController() {
	static ConstructorHelpers::FClassFinder<UNameplateWidget> Nameplate(TEXT("/Game/Power/UI/NamePlates/BP_Nameplate.BP_Nameplate_C"));
	if (Nameplate.Succeeded())
	{
		this->SetWidgetClass(Nameplate.Class);
	}
	this->bEditableWhenInherited = true;
}