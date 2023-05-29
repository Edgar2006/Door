
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
	//Create a Static Mesh for our Actor
	SM_Outside = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SM_Outside->SetupAttachment(RootComponent);
	v = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stati"));
	v->SetupAttachment(ElvatorMesh);
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
		NextCustBool = 1;
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
	NextCustBool = 1;
	BoolOpenDoor = false;
	AnimationDoneCheck = true;
	if (CurveFloatOpen) {
		first = CurveFloatOpen->GetCurves().GetData()->CurveToEdit->GetFirstKeyHandle();
		second = CurveFloatOpen->GetCurves().GetData()->CurveToEdit->GetNextKey(first);
		FOnTimelineFloat TimelineProgressDoor, TimelineProgress_Up_Down;
		FOnTimelineEventStatic TimelineProgressDoorFinishedCallback, TimelineProgress_Up_DownFinishedCallback;
		TimelineProgressDoor.BindUFunction(this, FName("TimelineProgressDoor"));
		TimelineProgressDoorFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineProgressDoorFinishedCallback") });
		CurveTimelineOpen.AddInterpFloat(CurveFloatClose, TimelineProgressDoor);
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
	DOREPLIFETIME(AMyTimelineCurve, NextCustBool);

}
void AMyTimelineCurve::Outside()
{
	//Empty the array and delete all it's components
	for (auto It = SM_Outside_Array.CreateIterator(); It; It++)
	{
		(*It)->DestroyComponent();
	}

	SM_Outside_Array.Empty();

	//Register all the components
	RegisterAllComponents();
	//The base name for all our components
	FName InitialName = FName("MyCompName");

	for (int32 i = 0; i < NumToSpawn; i++)
	{
		//Create a new Component
		//The first parameter is the "parent" of the our new component
		UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this, InitialName);

		//Add a reference to our array
		SM_Outside_Array.Add(NewComp);

		//Change the name for the next possible item
		FString Str = "MyCompName" + FString::FromInt(i + 1);

		//Convert the FString to FName
		InitialName = (*Str);

		//If the component is valid, set it's static mesh, relative location and attach it to our parent
		if (NewComp)
		{
			GLog->Log("Registering comp...");

			//Register the new component
			NewComp->RegisterComponent();

			//Set the static mesh of our component
			NewComp->SetStaticMesh(SM_Outside->GetStaticMesh());
			NewComp->SetRelativeScale3D(SM_Outside->GetRelativeScale3D());
			NewComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			NewComp->SetCollisionObjectType(SM_Outside->GetCollisionObjectType());

			//Set a random location based on the values we enter through the editor
			FVector Location = SM_Outside->GetComponentLocation();

			Location.Z += (i + 1) * XThreshold;

			NewComp->SetWorldLocation(Location);
			//NewComp->SetupAttachment(ElvatorMesh);


		}
	}
}
void AMyTimelineCurve::Inside()
{
	//Empty the array and delete all it's components
	for (auto It = SM_Inside_Array.CreateIterator(); It; It++)
	{
		(*It)->DestroyComponent();
	}

	SM_Inside_Array.Empty();

	//Register all the components
	RegisterAllComponents();
	//The base name for all our components
	FName InitialName = FName("MyCompName1");

	for (int32 i = 0; i < NumToSpawn; i++)
	{
		

		for (int32 j = 0; j < NumToSpawn; j++)
		{
			FString Str = FString::FromInt((i * NumToSpawn) + j);
			FName q = FName(Str);

			UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this, q);

			//Add a reference to our array
			SM_Inside_Array.Add(NewComp);

			//Change the name for the next possible item

			//Convert the FString to FName
			InitialName = (*Str);

			//If the component is valid, set it's static mesh, relative location and attach it to our parent
			if (NewComp)
			{
				GLog->Log("Registering comp1...");

				//Register the new component
				NewComp->RegisterComponent();

				//Set the static mesh of our component

				NewComp->SetStaticMesh(v->GetStaticMesh());
				NewComp->SetRelativeScale3D(v->GetRelativeScale3D());
				NewComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
				NewComp->SetCollisionObjectType(v->GetCollisionObjectType());

				//Set a random location based on the values we enter through the editor
				FVector Location = v->GetComponentLocation();
					
				Location.X += (i + 1) * YThreshold;
				Location.Z += (j + 1) * YThreshold;

				NewComp->SetWorldLocation(Location);
				NewComp->AttachToComponent(ElvatorMesh, FAttachmentTransformRules::KeepWorldTransform);

				//Attach the component to the root component
				//NewComp->SetupAttachment(ElvatorMesh);
			}
		}
	}
}
void AMyTimelineCurve::OnConstruction(const FTransform& Transform)
{
	Outside();
	Inside();
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
	if (NextCustBool) {
		NextCustBool = 0;
		EndLocElvator.Z = z - 200;
		CurveFloatOpen->GetCurves().GetData()->CurveToEdit->SetKeyTime(second, FMath::Abs((StartLocElvator.Z - EndLocElvator.Z)) / 200);

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
}













void AMyTimelineCurve::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

void AMyTimelineCurve::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}



