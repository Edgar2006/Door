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
		void TimelineProgress(float Value);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	FTimeline CurveTimelineOpen;
	FTimeline CurveTimelineClose;
	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* CurveFloatOpen;
	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* CurveFloatClose;
	UPROPERTY()
		FRotator StartLoc;
	UPROPERTY()
		FRotator EndLoc;
	UPROPERTY(EditAnywhere, Category = "Timeline")
		float ZOffset;
	UPROPERTY(EditAnywhere)
		USceneComponent* _RootComponent;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LightSwitchMesh;
	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractionWidget;

	UPROPERTY(ReplicatedUsing = OnRep_ServerVariableTrueOrFasle)
		bool bIsOn;
	UFUNCTION()
		void OnRep_ServerVariableTrueOrFasle();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;	

	virtual void InteractWithMe() override;
	virtual void ShowInteractionWidget() override;
	virtual void HideInteractionWidget() override;




};
