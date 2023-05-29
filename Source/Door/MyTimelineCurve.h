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

	FTimeline CurveTimelineElvator;
	FTimeline CurveTimelineOpen;
	FKeyHandle first;
	FKeyHandle second;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* CurveFloatOpen;
	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* CurveFloatClose;
	UPROPERTY()
		FRotator StartLocDoorOpening;
	UPROPERTY()
		FRotator EndLocDoorOpening;
	UPROPERTY()
		FVector StartLocElvator;
	UPROPERTY()
		FVector EndLocElvator;
	UPROPERTY(EditAnywhere, Category = "Timeline")
		float ZOffset;
	UPROPERTY(EditAnywhere)
		USceneComponent* _RootComponent;
	UPROPERTY(EditAnywhere)
		USceneComponent* ElvetorPosition;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ElvatorDoorMesh;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ElvatorMesh;
	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractionWidget;

	UPROPERTY(ReplicatedUsing = OnRep_ServerOpenDoor)
		bool BoolOpenDoor;
	UPROPERTY(ReplicatedUsing = OnRep_ServerElvetor)
		bool AnimationDoneCheck;
	UPROPERTY(ReplicatedUsing = OnRep_ServerElvetor)
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



	/*The static mesh of our actor*/
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* SM_Outside;


	/*The static mesh of our actor*/
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* v;


	/*The random location of our component will get generated based on
	a given X and Y from within our editor.*/

	/*The max x of the location vector that the static mesh will be spawned*/
	UPROPERTY(EditAnywhere, Category = "Construction")
		float XThreshold;

	/*The max y of the location vector that the static mesh will be spawned*/
	UPROPERTY(EditAnywhere, Category = "Construction")
		float YThreshold;

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
	virtual void InteractSetSwichObjectPossiton(float Z) override;
	virtual void ShowInteractionWidget() override;
	virtual void HideInteractionWidget() override;
	virtual void OnConstruction(const FTransform& Transform) override;



};
