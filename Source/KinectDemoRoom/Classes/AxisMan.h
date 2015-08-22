// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AxisMan.generated.h"

UCLASS()
class KINECTDEMOROOM_API AAxisMan : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAxisMan();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
