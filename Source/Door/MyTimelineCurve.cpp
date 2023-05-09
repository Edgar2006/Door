
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
	ElvatorDoorMesh->SetWorldRotation(NewLocation);
}

void AMyTimelineCurve::TimelineProgress_Up_Down(float Value)
{
	FVector NewLocation = FMath::Lerp(StartLocElvator, EndLocElvator, Value);
	FString a = FString::SanitizeFloat(NewLocation.Z);
	ElvatorMesh->SetWorldLocation(NewLocation);

	if (Value == 1) {
		Swap(StartLocElvator, EndLocElvator);
	}
}



// Called when the game starts or when spawned
void AMyTimelineCurve::BeginPlay()
{
	Super::BeginPlay();

	bIsOn = false;
	if (CurveFloatOpen && CurveFloatClose) {
		FOnTimelineFloat TimelineProgressDoor, TimelineProgress_Up_Down;
		TimelineProgressDoor.BindUFunction(this, FName("TimelineProgressDoor"));
		CurveTimelineOpen.AddInterpFloat(CurveFloatOpen, TimelineProgressDoor);
		CurveTimelineClose.AddInterpFloat(CurveFloatClose, TimelineProgressDoor);
		TimelineProgress_Up_Down.BindUFunction(this, FName("TimelineProgress_Up_Down"));
		CurveTimelineElvator.AddInterpFloat(CurveFloatOpen, TimelineProgress_Up_Down);
		StartLocDoorOpening = EndLocDoorOpening = GetActorRotation();
		EndLocDoorOpening.Yaw += ZOffset;
		StartLocElvator = EndLocElvator = ElvetorPosition->GetComponentLocation();
	}
	InteractionWidget->SetVisibility(false);

}



// Called every frame
void AMyTimelineCurve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurveTimelineOpen.TickTimeline(DeltaTime);
	CurveTimelineClose.TickTimeline(DeltaTime);
	CurveTimelineElvator.TickTimeline(DeltaTime);
}


void AMyTimelineCurve::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyTimelineCurve, bIsOn);
}

void AMyTimelineCurve::CheckIfOpenDoor()
{
	FString a = FString::SanitizeFloat(StartLocElvator.Z);
	FString b = FString::SanitizeFloat(EndLocElvator.Z);

	GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Red, a + "_______" + b);
	if (FMath::Abs(StartLocElvator.Z - EndLocElvator.Z) < 200) {
		GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Red, "doorOpen");
		InteractWithMe();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Red, "Elevator");
		CurveTimelineElvator.PlayFromStart();
	}
}

void AMyTimelineCurve::OnRep_ServerVariableTrueOrFasle()
{
	if (bIsOn) {
		//Light->SetIntensity(0);
		CurveTimelineOpen.PlayFromStart();
	}
	else {
		//Light->SetIntensity(10000);
		CurveTimelineClose.PlayFromStart();
	}
}


void AMyTimelineCurve::InteractWithMe()
{
	//if (HasAuthority()) {
		
		bIsOn = !bIsOn;
		OnRep_ServerVariableTrueOrFasle();
	//}
}

void AMyTimelineCurve::InteractSetSwichObjectPossiton(float z)
{
	EndLocElvator.Z = z-200;
	CheckIfOpenDoor();
}

void AMyTimelineCurve::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

void AMyTimelineCurve::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}



