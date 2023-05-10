
#include "MyTimelineCurve.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include <Net/UnrealNetwork.h>

// Sets default values

AMyTimelineCurve::AMyTimelineCurve()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root component"));
	RootComponent = _RootComponent;
	ElvatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator mesh"));
	ElvatorMesh->SetupAttachment(RootComponent);
	ElvatorDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator door mesh"));
	ElvatorDoorMesh->SetupAttachment(ElvatorMesh);
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction widget"));
	InteractionWidget->SetupAttachment(RootComponent);
	ElvetorPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ElvetorPosition->SetupAttachment(ElvatorMesh);
	bReplicates = true;

}

void AMyTimelineCurve::TimelineProgressDoor(float Value)
{
	FRotator NewLocation = FMath::Lerp(StartLocDoorOpening, EndLocDoorOpening, Value);
	ElvatorDoorMesh->SetRelativeRotation(NewLocation);
}
void AMyTimelineCurve::TimelineProgress_Up_Down(float Value)
{
	FVector NewLocation = FMath::Lerp(StartLocElvator, EndLocElvator, Value);
	FString a = FString::SanitizeFloat(NewLocation.Z);
	ElvatorMesh->SetWorldLocation(NewLocation);
}

void AMyTimelineCurve::TimelineProgressDoorFinishedCallback()
{
	if (AnimationDoneCheck) {
		GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Red, "qwertyui");
		AnimationDoneCheck = false;
		CurveTimelineElvator.PlayFromStart();
	}
	else {
		AnimationDoneCheck = true;
	}
}

void AMyTimelineCurve::TimelineProgress_Up_DownFinishedCallback()
{
	Swap(StartLocElvator, EndLocElvator);
	AnimationDoneCheck = false;
	InteractWithMe();
}



void AMyTimelineCurve::BeginPlay()
{
	Super::BeginPlay();

	BoolOpenDoor = false;
	AnimationDoneCheck = true;
	if (CurveFloatOpen && CurveFloatClose) {
		FOnTimelineFloat TimelineProgressDoor, TimelineProgress_Up_Down;
		FOnTimelineEventStatic TimelineProgressDoorFinishedCallback, TimelineProgress_Up_DownFinishedCallback;

		TimelineProgressDoor.BindUFunction(this, FName("TimelineProgressDoor"));
		TimelineProgressDoorFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineProgressDoorFinishedCallback") });
		CurveTimelineOpen.AddInterpFloat(CurveFloatOpen, TimelineProgressDoor);
		CurveTimelineOpen.SetTimelineFinishedFunc(TimelineProgressDoorFinishedCallback);

		TimelineProgress_Up_Down.BindUFunction(this, FName("TimelineProgress_Up_Down"));
		TimelineProgress_Up_DownFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineProgress_Up_DownFinishedCallback") });
		CurveTimelineElvator.AddInterpFloat(CurveFloatOpen, TimelineProgress_Up_Down);
		CurveTimelineElvator.SetTimelineFinishedFunc(TimelineProgress_Up_DownFinishedCallback);

		StartLocDoorOpening = EndLocDoorOpening = GetActorRotation();
		EndLocDoorOpening.Yaw += ZOffset;
		StartLocElvator = EndLocElvator = ElvetorPosition->GetComponentLocation();
	}
	InteractionWidget->SetVisibility(false);
}
void AMyTimelineCurve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurveTimelineOpen.TickTimeline(DeltaTime);
	CurveTimelineElvator.TickTimeline(DeltaTime);
}
void AMyTimelineCurve::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyTimelineCurve, BoolOpenDoor);
	DOREPLIFETIME(AMyTimelineCurve, AnimationDoneCheck);

}


void AMyTimelineCurve::OnRep_ServerOpenDoor()
{
	if (BoolOpenDoor) {
		CurveTimelineOpen.PlayFromStart();
	}
	else {
		CurveTimelineOpen.Reverse();
	}
}
void AMyTimelineCurve::InteractWithMe()
{
	BoolOpenDoor = !BoolOpenDoor;
	OnRep_ServerOpenDoor();
}

void AMyTimelineCurve::OnRep_ServerElvetor()
{
	if (!AnimationDoneCheck) {
		InteractWithMe();
	}
	else {
		if (BoolOpenDoor) {
			InteractWithMe();
		}
		else {
			CurveTimelineElvator.PlayFromStart();
		}
	}
}



void AMyTimelineCurve::InteractSetSwichObjectPossiton(float z)
{
	EndLocElvator.Z = z-200;
	if (FMath::Abs(StartLocElvator.Z - EndLocElvator.Z) < 200) {
		AnimationDoneCheck = false;
	}
	else {
		if (BoolOpenDoor) {
			AnimationDoneCheck = true;
		}
		else {
			AnimationDoneCheck = true;
		}
	}
	OnRep_ServerElvetor();
}













void AMyTimelineCurve::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

void AMyTimelineCurve::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}



