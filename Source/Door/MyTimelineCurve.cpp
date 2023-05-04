
#include "MyTimelineCurve.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

// Sets default values

AMyTimelineCurve::AMyTimelineCurve()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root component"));
	RootComponent = _RootComponent;
	LightSwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light switch mesh"));
	LightSwitchMesh->SetupAttachment(RootComponent);
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction widget"));
	InteractionWidget->SetupAttachment(RootComponent);


}

void AMyTimelineCurve::TimelineProgress(float Value)
{
	FRotator NewLocation = FMath::Lerp(StartLoc, EndLoc, Value);
	SetActorRotation(NewLocation);
}


// Called when the game starts or when spawned
void AMyTimelineCurve::BeginPlay()
{
	Super::BeginPlay();


	if (CurveFloatOpen && CurveFloatClose) {
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimelineOpen.AddInterpFloat(CurveFloatOpen, TimelineProgress);
		CurveTimelineClose.AddInterpFloat(CurveFloatClose, TimelineProgress);
		StartLoc = EndLoc = GetActorRotation();
		EndLoc.Yaw += ZOffset;
		bIsOn = true;
	}
	InteractionWidget->SetVisibility(false);

}

// Called every frame
void AMyTimelineCurve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurveTimelineOpen.TickTimeline(DeltaTime);
	CurveTimelineClose.TickTimeline(DeltaTime);

}

void AMyTimelineCurve::InteractWithMe()
{
	UE_LOG(LogTemp, Warning, TEXT("You have interactive with me"));
	if (bIsOn) {
		//Light->SetIntensity(0);
		CurveTimelineOpen.PlayFromStart();
		GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Green, "Begin");
		bIsOn = false;
	}
	else {
		//Light->SetIntensity(10000);
		GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Red, "End!!!!");
		CurveTimelineClose.PlayFromStart();


		bIsOn = true;
	}
}

void AMyTimelineCurve::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

void AMyTimelineCurve::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}



