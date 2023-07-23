// Copyright Epic Games, Inc. All Rights Reserved.

#include "DoorCharacter.h"
#include "DoorProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/Engine.h"
#include <Net/UnrealNetwork.h>

//////////////////////////////////////////////////////////////////////////
// ADoorCharacter

ADoorCharacter::ADoorCharacter()
{
	// Character doesnt have a rifle at start	
	bHasRifle = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void ADoorCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}
void ADoorCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADoorCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADoorCharacter::Look);
	}

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADoorCharacter::OnInteract);
}
void ADoorCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}
void ADoorCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void ADoorCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}
bool ADoorCharacter::GetHasRifle()
{
	return bHasRifle;
}


void ADoorCharacter::Interact()
{
	if (HasAuthority()) {
		Interface->InteractSetSwichObjectPossiton(ZPositionElvatorDoorMesh);	
	}
	else {
		Server_Interact();
	}
}

void ADoorCharacter::LineTrace()
{
	float LengthOfTrace = 400.f;
	FVector StartLocation;
	FVector EndLocation;
	StartLocation = FirstPersonCameraComponent->GetComponentLocation();
	EndLocation = StartLocation + (FirstPersonCameraComponent->GetForwardVector() * LengthOfTrace);
	FHitResult OutHitResult;
	FCollisionQueryParams LineTraceParams;
	LineTraceParams.AddIgnoredActor(this);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.01f, 0, 1);

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(OutHitResult, StartLocation, EndLocation, ECC_Visibility, LineTraceParams);
	if (bHitSomething)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));
		Interface = Cast<IInteractionInterface>(OutHitResult.GetActor());
		if (Interface)
		{
			ElevatorButtonComponent = OutHitResult.GetComponent();
			isInteractTextShow = 1;
		}
	}
	else {
		Interface = nullptr;
		isInteractTextShow = 0;
	}
}


void ADoorCharacter::OnInteract() {
	if (Interface) {
		ZPositionElvatorDoorMesh = ElevatorButtonComponent->GetName();
		Interact();
	}
	
}

void ADoorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LineTrace();
}



bool ADoorCharacter::Server_Interact_Validate()
{
	return true;
}
void ADoorCharacter::Server_Interact_Implementation()
{
	ADoorCharacter::OnInteract();
}
