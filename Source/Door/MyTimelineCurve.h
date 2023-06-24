// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "InteractionInterface.h"
#include <Components/WidgetComponent.h>

#include <Components/TimelineComponent.h>
#include "Components/StaticMeshComponent.h"

#include "MyTimelineCurve.generated.h"

UCLASS()
class DOOR_API AMyTimelineCurve : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyTimelineCurve();

	UFUNCTION()
		void TimelineProgressDoor(float Value);
	UFUNCTION()
		void TimelineProgress_Up_Down(float Value);
	UFUNCTION()
		void TimelineProgressDoorFinishedCallback();
	UFUNCTION()
		void TimelineProgress_Up_DownFinishedCallback();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(Replicated)
		FTimeline CurveTimelineElvator;
	UPROPERTY(Replicated)
		FTimeline CurveTimelineOpen;
		FKeyHandle first;
		FKeyHandle second;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* CurveFloatOpen;
	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* CurveFloatClose;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline", Meta = (MakeEditWidget = true))
		FVector StartLocDoorOpening;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline", Meta = (MakeEditWidget = true))
		FVector EndLocDoorOpening;
	UPROPERTY(Replicated)
		FVector StartLocElvator;
	UPROPERTY(Replicated)
		FVector EndLocElvator;
	UPROPERTY(EditAnywhere)
		USceneComponent* _RootComponent;
	UPROPERTY(EditAnywhere)
		USceneComponent* ElvetorPosition;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ElvatorDoorMeshLeft;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ElvatorDoorMeshRight;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ElvatorMesh;
	UPROPERTY(ReplicatedUsing = OnRep_ServerOpenDoor)
		bool BoolOpenDoor;
	UPROPERTY(Replicated)
		bool AnimationDoneCheck;
	UPROPERTY(Replicated)
		bool NextCustBool;
	UFUNCTION()
		void OnRep_ServerOpenDoor();
	UFUNCTION()
		void OnRep_ServerElvetor();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

		




	UFUNCTION()
		void Outside();
	UFUNCTION()
		void Inside();
	UFUNCTION()
		bool CheckIfButton(FString str);
	UFUNCTION()
		int GetButtonNumber(FString str);

	/*The static mesh of our actor*/
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* SM_OutsideMeshUp;
	/*The static mesh of our actor*/
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* SM_OutsideMeshDown;
	/*The static mesh of our actor*/
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* SM_Inside;

	/*The random location of our component will get generated based on
	a given X and Y from within our editor.*/

	/*The max x of the location vector that the static mesh will be spawned*/
	UPROPERTY(EditAnywhere, Category = "Construction")
		int32 XCol;
	UPROPERTY(EditAnywhere, Category = "Construction")
		int32 YCol;
	UPROPERTY(EditAnywhere, Category = "Construction")
		float XThreshold;

	/*The max y of the location vector that the static mesh will be spawned*/
	UPROPERTY(EditAnywhere, Category = "Construction")
		float YThreshold;

	UPROPERTY(EditAnywhere, Category = "Construction")
		float ZThreshold;

	/*The number of meshes/components that will get spawned*/
	UPROPERTY(EditAnywhere, Category = "Construction")
		int32 NumToSpawn;	

	/*An array containing references of the spawned components
	This will be used in order to delete old components in case we decide to
	tinker with the NumToSpawn parameter*/
	TArray<UStaticMeshComponent*> SM_Outside_Array;
	TArray<UStaticMeshComponent*> SM_Inside_Array;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
	virtual void InteractWithMe() override;
	virtual void InteractSetSwichObjectPossiton(FString ComponentName) override;
	virtual void OnConstruction(const FTransform& Transform) override;



};
