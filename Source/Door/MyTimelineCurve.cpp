
#include "MyTimelineCurve.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include <Net/UnrealNetwork.h>
#include "Kismet/KismetMathLibrary.h"


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
	SM_OutsideMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_OutSide_0"));
	SM_OutsideMesh->SetupAttachment(RootComponent);
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
	FTransform NewLocation;
	NewLocation.SetLocation(FMath::Lerp(StartLocDoorOpening.GetLocation(), EndLocDoorOpening.GetLocation(), Value));
	NewLocation.SetRotation(FMath::Lerp(StartLocDoorOpening.GetRotation(), EndLocDoorOpening.GetRotation(), Value));

	ElvatorDoorMeshLeft->SetRelativeTransform(NewLocation);
	if (!isOneDoor) {
		NewLocation.SetLocation(NewLocation.GetLocation() * -1);
		ElvatorDoorMeshRight->SetRelativeTransform(NewLocation);
	}
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
	if (ElvatorDoorMeshRight == nullptr) {
		isOneDoor = 1;
	}
	if (ElvatorDoorMeshLeft == nullptr && !isOneDoor) {
		ElvatorDoorMeshLeft = ElvatorDoorMeshRight;
		isOneDoor = 1;
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


void AMyTimelineCurve::SetMeshParametr(UStaticMeshComponent* SpawnMesh, UStaticMeshComponent* Mesh, int32 index)
{
	if (SpawnMesh)
	{
		if (Mesh != nullptr) {
			SpawnMesh->RegisterComponent();
			SpawnMesh->SetStaticMesh(Mesh->GetStaticMesh());
			SpawnMesh->SetRelativeScale3D(Mesh->GetRelativeScale3D());
			SpawnMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			SpawnMesh->SetCollisionObjectType(Mesh->GetCollisionObjectType());
			FVector Location = Mesh->GetComponentLocation();
			Location.Z += index * XThreshold;
			SpawnMesh->SetWorldLocation(Location);
		}
	}
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
	for (int32 i = 0; i < NumToSpawn; i++)
	{
		UStaticMeshComponent* UpMesh = NewObject<UStaticMeshComponent>(this, InitialName);
		SM_Outside_Array.Add(UpMesh);
		FString Str = "TAG_Button_UP_" + FString::FromInt(i + 1);
		InitialName = (*Str);
		SetMeshParametr(UpMesh, SM_OutsideMeshUp, i);
		UStaticMeshComponent* DownMesh = NewObject<UStaticMeshComponent>(this, InitialName);
		SM_Outside_Array.Add(DownMesh);
		Str = "TAG_Button_UD_" + FString::FromInt(i + 1);
		InitialName = (*Str);
		SetMeshParametr(DownMesh, SM_OutsideMeshDown, i);

		UStaticMeshComponent* OutSideMesh = NewObject<UStaticMeshComponent>(this, InitialName);
		SM_Outside_Array.Add(DownMesh);
		Str = "SM_OutSide" + FString::FromInt(i + 1);
		InitialName = (*Str);
		SetMeshParametr(OutSideMesh, SM_OutsideMesh, i);
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
					FRotator Rotation = SM_Inside->GetRelativeRotation();
					FVector RightVector = UKismetMathLibrary::GetRightVector(Rotation);
					FVector UpVector = UKismetMathLibrary::GetUpVector(Rotation);
					Location += RightVector * (i * YThreshold);
					Location += UpVector * (j * ZThreshold);
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




