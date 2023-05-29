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
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));


	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction box"));
	InteractionBox->SetBoxExtent(FVector(120, 32, 32));
	InteractionBox->SetupAttachment(FirstPersonCameraComponent);

	FVector CurrentLocation = InteractionBox->GetRelativeTransform().GetLocation();
	CurrentLocation.X += 100;
	InteractionBox->GetRelativeTransform().SetLocation(CurrentLocation);
	MaterialOne = CreateDefaultSubobject<UMaterialInterface>("MaterialOne");
	MaterialTwo = CreateDefaultSubobject<UMaterialInterface>("MaterialTwo");
	MaterialThree = CreateDefaultSubobject<UMaterialInterface>("MaterialThree");
	MaterialThree = CreateDefaultSubobject<UMaterialInterface>("MaterialFour");
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




void ADoorCharacter::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TArray<UPrimitiveComponent*> AllComponent;
	//OverlappedComponent->GetOverlappingComponents(AllComponent);
	//UPrimitiveComponent* min = AllComponent[0];
	//for (UPrimitiveComponent* i : AllComponent) {
	//	if ((min->GetRelativeLocation() - OverlappedComponent->GetRelativeLocation()).Size() < (i->GetRelativeLocation() - OverlappedComponent->GetRelativeLocation()).Size()) {
	//		min = i;
	//	}
	//}
	//OtherComp = min;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, OtherComp->GetName());
	ElevatorButtonComponent = OtherComp;
	if (OtherComp->GetMaterial(0) == MaterialThree) {
		OtherComp->SetMaterial(0, MaterialFour);
	}
	else {
		OtherComp->SetMaterial(0, MaterialOne);
		bChooseOne = !bChooseOne;
	}
	if (Interface) {
		Interface->HideInteractionWidget();
	}
	
	Interface = Cast<IInteractionInterface>(OtherActor);
	if (Interface) {
		ZPositionElvatorDoorMesh = OtherComp->GetComponentLocation().Z;
		Interface->ShowInteractionWidget();
	}

}

void ADoorCharacter::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->GetMaterial(0) == MaterialOne) {
		OtherComp->SetMaterial(0, MaterialTwo);
		bChooseOne = !bChooseOne;
	}
	if (OtherComp->GetMaterial(0) == MaterialFour) {
		OtherComp->SetMaterial(0, MaterialThree);
	}
	if (Interface) {
		Interface->HideInteractionWidget();
		Interface = nullptr;
	}
}

bool ADoorCharacter::CheckIfElevatorButtonComponent(FString v)
{
	bool b = 0;
	for (int i = 0; i < v.Len(); i++) {
		if (v[i] - '0' < 0 || v[i] - '0' > 9) {
			return false;
		}
	}
	return true;
}

void ADoorCharacter::Interact()
{
	if (HasAuthority()) {
		if (CheckIfElevatorButtonComponent(ElevatorButtonComponent->GetName())) {
			ZPositionElvatorDoorMesh = FCString::Atoi(*ElevatorButtonComponent->GetName()) * 100;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, ElevatorButtonComponent->GetName());
		Interface->InteractSetSwichObjectPossiton(ZPositionElvatorDoorMesh);
		if (ElevatorButtonComponent->GetMaterial(0) == MaterialFour) {
			ElevatorButtonComponent->SetMaterial(0, MaterialTwo);
		}	
		else {
			ElevatorButtonComponent->SetMaterial(0, MaterialThree);
		}
	}
	else {
		// call to server
		Server_Interact();
	}
}


void ADoorCharacter::OnInteract() {
	if (Interface) {
		Interact();
	}
	else {
		float LengthOfTrace = 100.f;

		FVector StartLocation;
		FVector EndLocation;
		
		StartLocation = FirstPersonCameraComponent->GetComponentLocation();

		EndLocation = StartLocation +
			(FirstPersonCameraComponent->GetForwardVector() * LengthOfTrace);

		FHitResult OutHitResult;
		FCollisionQueryParams LineTraceParams;
		LineTraceParams.AddIgnoredActor(this);
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1, 0, 1);

		bool bHitSomething = GetWorld()->LineTraceSingleByChannel(OutHitResult,StartLocation, EndLocation, ECC_Visibility, LineTraceParams);
		if (bHitSomething)
		{
			Interface = Cast<IInteractionInterface>(OutHitResult.GetActor());
			if (Interface)
			{
				ElevatorButtonComponent = OutHitResult.GetComponent();
				Interact();
			}
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
