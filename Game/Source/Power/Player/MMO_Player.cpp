// Fill out your copyright notice in the Description page of Project Settings.

#include "MMO_Player.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "UnrealNetwork.h"
#include "SpatialNetDriver.h"

// Sets default values
AMMO_Player::AMMO_Player()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Configure character movement	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

}

void AMMO_Player::BeginPlay()
{
	Super::BeginPlay();
}

// Called to bind functionality to input
void AMMO_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMMO_Player::MoveForward);
	PlayerInputComponent->BindAxis("MoveBackward", this, &AMMO_Player::MoveBackward);
	
	PlayerInputComponent->BindAxis("StrafeLeft", this, &AMMO_Player::StrafeLeft);
	PlayerInputComponent->BindAxis("StrafeRight", this, &AMMO_Player::StrafeRight);
	
	PlayerInputComponent->BindAxis("RotateRight", this, &AMMO_Player::RotateRight);
	PlayerInputComponent->BindAxis("RotateLeft", this, &AMMO_Player::RotateLeft);

    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMMO_Player::Interact);

    //AbilitySystem->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbiliyInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
}

/*
void AMMO_Player::RotateRight(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMMO_Player::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
*/

void AMMO_Player::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMMO_Player::MoveBackward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(-Direction, Value);

		//Set the Character to face forward while moving backwards
		Controller->SetControlRotation(YawRotation);
	}
}

void AMMO_Player::StrafeRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMMO_Player::StrafeLeft(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(-Direction, Value);
	}
}

void AMMO_Player::RotateRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		FRotator Yaw = Controller->GetControlRotation();
		AMMO_Player::UpdatePlayerRot(Yaw);
	}
}

void AMMO_Player::RotateLeft(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AddControllerYawInput(-1 * Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		FRotator Yaw = Controller->GetControlRotation();
		AMMO_Player::UpdatePlayerRot(Yaw);
	}
} 

void AMMO_Player::UpdatePlayerRot(FRotator NewRot) {
	const FRotator rot(0, NewRot.Yaw, 0);
	ACharacter* myCharacter = Cast<ACharacter>(GetOwner());
	SetActorRotation(rot);

	if (Role < ROLE_Authority)
	{
		ServerTurnPlayer(NewRot);
	}
};

bool AMMO_Player::ServerTurnPlayer_Validate(FRotator NewRot)
{
	return true;
}

void AMMO_Player::ServerTurnPlayer_Implementation(FRotator NewRot)
{
	UpdatePlayerRot(NewRot);
}

void AMMO_Player::Interact()
{
    // Note:
    // Interact is, for now, used as a test method for pretty much anything we wanna try out. 
    // In this case, pressing F will deal damage to the character pressing it. 
    // Allows checking that health is replicated to everyone
    UE_LOG(LogTemp, Warning, TEXT("Health should decrease"))
    
    this->DealDamage(10);
}