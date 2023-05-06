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
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));


	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction box"));
	InteractionBox->SetBoxExtent(FVector(120, 32, 32));
	InteractionBox->SetupAttachment(FirstPersonCameraComponent);

	FVector CurrentLocation = InteractionBox->GetRelativeTransform().GetLocation();
	CurrentLocation.X += 100;
	InteractionBox->GetRelativeTransform().SetLocation(CurrentLocation);

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


	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorCharacter::OnBoxBeginOverlap);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &ADoorCharacter::OnBoxOverlapEnd);
}

//////////////////////////////////////////////////////////////////////////// Input

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

void ADoorCharacter::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<AActor*>OvelrappingActors;
	InteractionBox->GetOverlappingActors(OvelrappingActors);
	AActor* ClosestActor = OvelrappingActors[0];

	for (auto CurrentActor : OvelrappingActors)
	{
		if (GetDistanceTo(CurrentActor) < GetDistanceTo(ClosestActor))
		{
			ClosestActor = CurrentActor;
		}
	}
	if (Interface) {
		Interface->HideInteractionWidget();
	}

	Interface = Cast<IInteractionInterface>(ClosestActor);
	if (Interface) {
		Interface->ShowInteractionWidget();
	}

}

void ADoorCharacter::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Interface) {
		Interface->HideInteractionWidget();
		Interface = nullptr;
	}
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


void ADoorCharacter::OnInteract() {
	if (Interface) {
		if (HasAuthority()) {
			Interface->InteractWithMe();
		}
		else {
			// call to server
			Server_Interact();
		}
	}
}

bool ADoorCharacter::Server_Interact_Validate()
{
	return true;
}


void ADoorCharacter::Server_Interact_Implementation()
{
	ADoorCharacter::OnInteract();
}
