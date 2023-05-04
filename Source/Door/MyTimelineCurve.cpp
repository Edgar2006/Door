// Fill out your copyright notice in the Description page of Project Settings.


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
	if (Value < 0.1)
	{
		bIsOn = true;
	}
	else
	{
		bIsOn = false;
	}
	FRotator NewLocation = FMath::Lerp(StartLoc, EndLoc, Value);
	SetActorRotation(NewLocation);
}


// Called when the game starts or when spawned
void AMyTimelineCurve::BeginPlay()
{
	Super::BeginPlay();


	if (CurveFloat) {
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimeline.AddInterpFloat(CurveFloat, TimelineProgress);
		StartLoc = EndLoc = GetActorRotation();
		EndLoc.Yaw += ZOffset;
		//CurveTimeline.PlayFromStart();
	}
}

// Called every frame
void AMyTimelineCurve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurveTimeline.TickTimeline(DeltaTime);
}

void AMyTimelineCurve::InteractWithMe()
{
	UE_LOG(LogTemp, Warning, TEXT("You have interactive with me"));
	if (bIsOn) {
		//Light->SetIntensity(0);
		CurveTimeline.PlayFromStart();
		GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Green, "Begin");
		bIsOn = false;
	}
	else {
		//Light->SetIntensity(10000);
		GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Red, "End!!!!");


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

//void AMyTimelineCurve::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (IfDoorOpenAnimationDone)
//	{
//		IfDoorOpenAnimationDone = false;
//		CurveTimeline.PlayFromStart();
//	}
//	GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Green, "Begin");
//}
//
//void AMyTimelineCurve::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Red, "End!!!!");
//
//}

