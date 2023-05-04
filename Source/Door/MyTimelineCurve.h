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


	FTimeline CurveTimeline;
	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* CurveFloat;
	UPROPERTY()
		FRotator StartLoc;
	UPROPERTY()
		FRotator EndLoc;
	UPROPERTY(EditAnywhere, Category = "Timeline")
		float ZOffset;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InteractWithMe() override;
	virtual void ShowInteractionWidget() override;
	virtual void HideInteractionWidget() override;



private:
	UPROPERTY(EditAnywhere)
		USceneComponent* _RootComponent;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LightSwitchMesh;
	bool bIsOn = true;
	UPROPERTY(EditAnywhere)
		UWidgetComponent* InteractionWidget;


};
