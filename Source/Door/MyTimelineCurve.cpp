
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
	ElvatorDoorMeshLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator door mesh Left"));
	ElvatorDoorMeshLeft->SetupAttachment(ElvatorMesh);
	ElvatorDoorMeshRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elevator door mesh Right"));
	ElvatorDoorMeshRight->SetupAttachment(ElvatorMesh);
	ElvetorPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ElvetorPosition->SetupAttachment(ElvatorMesh);
	//Create a Static Mesh for our Actor
	SM_OutsideMeshUp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TAG_Button_OU_0"));
	SM_OutsideMeshUp->SetupAttachment(RootComponent);
	SM_OutsideMeshDown = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TAG_Button_OD_0"));
	SM_OutsideMeshDown->SetupAttachment(RootComponent);
	SM_Inside = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TAG_Button_IN_0"));
	SM_Inside->SetupAttachment(ElvatorMesh);
	bReplicates = true;
}
 
void AMyTimelineCurve::TimelineProgressDoor(float Value)
{
	FVector NewLocation = FMath::Lerp(StartLocDoorOpening, EndLocDoorOpening, Value);
	ElvatorDoorMeshLeft->SetRelativeLocation(NewLocation);
	NewLocation *= -1;
	ElvatorDoorMeshRight->SetRelativeLocation(NewLocation);
}
void AMyTimelineCurve::TimelineProgress_Up_Down(float Value)
{
	FVector NewLocation = FMath::Lerp(StartLocElvator, EndLocElvator, Value);
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
		StartLocElvator = EndLocElvator = ElvetorPosition->GetComponentLocation();
	}
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
	DOREPLIFETIME(AMyTimelineCurve, CurveTimelineOpen);
	DOREPLIFETIME(AMyTimelineCurve, CurveTimelineElvator);
	DOREPLIFETIME(AMyTimelineCurve, EndLocElvator);
	DOREPLIFETIME(AMyTimelineCurve, StartLocElvator);

}


void AMyTimelineCurve::Outside()
{
	for (auto It = SM_Outside_Array.CreateIterator(); It; It++)
	{
		(*It)->DestroyComponent();
	}

	SM_Outside_Array.Empty();

	//Register all the components
	RegisterAllComponents();
	//The base name for all our components

	FName InitialName = FName("MyCompName");
	FName InitialName1 = FName("MyCompName1");
	for (int32 i = 0; i < NumToSpawn; i++)
	{
		UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this, InitialName);
		SM_Outside_Array.Add(NewComp);
		FString Str = "TAG_Button_UP_" + FString::FromInt(i + 1);
		InitialName = (*Str);
		if (NewComp)
		{
			if (SM_OutsideMeshUp != nullptr) {
				NewComp->RegisterComponent();
				NewComp->SetStaticMesh(SM_OutsideMeshUp->GetStaticMesh());
				NewComp->SetRelativeScale3D(SM_OutsideMeshUp->GetRelativeScale3D());
				NewComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
				NewComp->SetCollisionObjectType(SM_OutsideMeshUp->GetCollisionObjectType());
				FVector Location = SM_OutsideMeshUp->GetComponentLocation();
				Location.Z += i * XThreshold;
				NewComp->SetWorldLocation(Location);
			}
		}
		UStaticMeshComponent* NewComp1 = NewObject<UStaticMeshComponent>(this, InitialName1);
		SM_Outside_Array.Add(NewComp1);
		FString Str1 = "TAG_Button_UD_" + FString::FromInt(i + 1);
		InitialName1 = (*Str1);
		if (NewComp1)
		{
			if (SM_OutsideMeshDown != nullptr) {
				NewComp1->RegisterComponent();
				NewComp1->SetStaticMesh(SM_OutsideMeshDown->GetStaticMesh());
				NewComp1->SetRelativeScale3D(SM_OutsideMeshDown->GetRelativeScale3D());
				NewComp1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
				NewComp1->SetCollisionObjectType(SM_OutsideMeshDown->GetCollisionObjectType());
				FVector Location1 = SM_OutsideMeshDown->GetComponentLocation();
				Location1.Z += i * XThreshold;
				NewComp1->SetWorldLocation(Location1);
			}
		}
	/*	if (i == 0) {
			NewComp1->DestroyComponent();
		}*/
	}
}
void AMyTimelineCurve::Inside()
{
	for (auto& i : SM_Inside_Array)
	{
		i->DestroyComponent();
	}
	SM_Inside_Array.Empty();
	RegisterAllComponents();
	int32 spawnCount = 0;
	for (int32 i = 0; i < XCol; i++)
	{
		for (int32 j = 0; j < YCol; j++)
		{
			spawnCount++;
			if (spawnCount == 1) {
				continue;
			}
			if (spawnCount > NumToSpawn) {
				break;
			}
			FString Str = "TAG_Button_IN_" + FString::FromInt(spawnCount - 1);
			FName name = FName(Str);
			UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this, name);
			SM_Inside_Array.Add(NewComp);
			if (NewComp)
			{
				if (SM_Inside != nullptr) {
					//Register the new component
					NewComp->RegisterComponent();
					NewComp->SetStaticMesh(SM_Inside->GetStaticMesh());
					NewComp->SetRelativeScale3D(SM_Inside->GetRelativeScale3D());
					NewComp->SetRelativeRotation(SM_Inside->GetRelativeRotation());
					NewComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
					NewComp->SetCollisionObjectType(SM_Inside->GetCollisionObjectType());
					NewComp->SetIsReplicated(1);
					FVector Location = SM_Inside->GetComponentLocation();
					Location.Y += i * YThreshold;
					Location.Z += j * ZThreshold;
					NewComp->SetRelativeLocation(Location);
					NewComp->AttachToComponent(ElvatorMesh, FAttachmentTransformRules::KeepWorldTransform);
				}
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


bool AMyTimelineCurve::CheckIfButton(FString str)
{
	FString a = "TAG_Button_";//need equal a elevator script 
	if (str.Len() > a.Len() + 3) {
		for (int i = 0; i < a.Len(); i++) {
			if (str[i] != a[i]) {
				return false;
			}
		}
	}
	return true;
}

int AMyTimelineCurve::GetButtonNumber(FString str)
{
	FString a = "TAG_Button_II_";//need equal a elevator script 
	FString num = "";
	for (int i = a.Len(); i < str.Len(); i++) {
		num += str[i];
	}
	return (FCString::Atoi(*num) * XThreshold);
}

void AMyTimelineCurve::InteractSetSwichObjectPossiton(FString ComponentName)
{
	if (NextCustBool && CheckIfButton(ComponentName)){
		GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Red, ComponentName);
		NextCustBool = 0;
		EndLocElvator.Z = GetButtonNumber(ComponentName);

		CurveFloatOpen->GetCurves().GetData()->CurveToEdit->SetKeyTime(second, FMath::Abs((StartLocElvator.Z - EndLocElvator.Z)) / 200);

		if (FMath::Abs(StartLocElvator.Z - EndLocElvator.Z) < 100) {
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




