// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerEntity.h"
#include "UnrealNetwork.h"
#include "PowerEntityAttributeSet.h"
#include "Classes/Components/DecalComponent.h"


// Sets default values
APowerEntity::APowerEntity()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    this->TargetCircle = CreateDefaultSubobject<UDecalComponent>(TEXT("TargetCircle"));
    this->TargetCircle->DecalSize = FVector(64.f, 64.f, 64.f);
    this->TargetCircle->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterial(TEXT("Material'/Game/Power/Materials/TargetDecal/M_DecalTarget.M_DecalTarget'"));
    if (DecalMaterial.Succeeded()) {
        this->TargetCircle->SetMaterial(0, DecalMaterial.Object);
    }

    //Ability system
    this->AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    this->AttributeSet = CreateDefaultSubobject<UPowerEntityAttributeSet>(TEXT("PowerEntityAttributeSet"));
}

// Called when the game starts or when spawned
void APowerEntity::BeginPlay()
{
	Super::BeginPlay();
    this->Health = 100;
    this->Mana = 1000;
    this->Level = 0;
    this->TargetCircle->SetRelativeLocation(FVector(0.f, 0.f, -110.f));
    this->TargetCircle->SetRelativeRotation(FQuat(180.f, 90.f, 180.f, 0.0f));
}

// Called every frame
void APowerEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APowerEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APowerEntity::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {

    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // Replicate to everyone
    DOREPLIFETIME(APowerEntity, Health);
    DOREPLIFETIME(APowerEntity, Mana);
    DOREPLIFETIME(APowerEntity, Level);
}

void APowerEntity::DealDamage(int Amount)
{
    if (Role < ROLE_Authority) {
        ServerDealDamage(Amount);
    }
}

void APowerEntity::ServerDealDamage_Implementation(int Amount)
{
    this->Health -= Amount;
}

bool APowerEntity::ServerDealDamage_Validate(int Amount)
{
    return true;
}



void APowerEntity::GiveAbility(TSubclassOf<UGameplayAbility> Ability)
{
    if (AbilitySystem) {
        if (HasAuthority() && Ability) {
            AbilitySystem->GiveAbility(FGameplayAbilitySpec(Ability, 1, 0));
        }

        AbilitySystem->InitAbilityActorInfo(this, this);
    }
}