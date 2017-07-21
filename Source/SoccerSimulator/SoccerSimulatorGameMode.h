// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "SoccerSimulatorGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SOCCERSIMULATOR_API ASoccerSimulatorGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ASoccerSimulatorGameMode();

	virtual void BeginPlay() override;
};
