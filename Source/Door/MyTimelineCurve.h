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


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimeline CurveTimelineElvator;
	FTimeline CurveTimelineOpen;
	FTimeline CurveTimelineClose;
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

	UPROPERTY(ReplicatedUsing = OnRep_ServerVariableTrueOrFasle)
		bool bIsOn;
	UFUNCTION()
		void OnRep_ServerVariableTrueOrFasle();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
		void CheckIfOpenDoor();
		



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;	

	virtual void InteractWithMe() override;
	virtual void InteractSetSwichObjectPossiton(float Z) override;
	virtual void ShowInteractionWidget() override;
	virtual void HideInteractionWidget() override;




};
