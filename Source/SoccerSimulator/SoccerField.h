#pragma once

#include "SoccerBall.h"
#include "Goal.h"
#include "SoccerTeam.h"
#include "GameFramework/Actor.h"
#include "SoccerField.generated.h"

UCLASS()
class SOCCERSIMULATOR_API ASoccerField : public AActor
{
	GENERATED_BODY()
	
public:	

	ASoccerField();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	bool Paused();

	bool GameOn();

	bool GoalKeeperHasBall();

	FVector SupportAreaScaledBounds();

	FVector PlayAreaScaledBounds();

	FVector RegionScaledBounds();

	FVector GetLocation();

	FVector GetPlayAreaLocation();

	void SetGameOn(bool bOn);

	FVector GetPositionOfRegionID(int Region);

	ASoccerBall* GetBall();

	bool IsPositionInPlayArea(FVector Position);

	void SetGoalKeeperHasBall(bool HasBall);

	float CurrentDiagonalRatio();

protected:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* DefaultComponent;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* SupportArea;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* PlayArea;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* RegionDefinitionArea;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* SoccerBallComponent;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* BlueGoalComponent;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* RedGoalComponent;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* BlueTeamComponent;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* RedTeamComponent;

	UPROPERTY(EditAnywhere)
	int32 XRegions;

	UPROPERTY(EditAnywhere)
	int32 YRegions;

	UPROPERTY(BlueprintReadWrite)
	bool bPaused;

private:

	bool bGameOn;
	bool bGoalKeeperHasBall;

	ASoccerBall* SoccerBall;

	AGoal* BlueGoal;
	AGoal* RedGoal;

	ASoccerTeam* BlueTeam;
	ASoccerTeam* RedTeam;

	FVector InitialPosition;
	FVector InitialBounds;
	FVector InitialUnscaledBounds;

	float MaximumPlayAreaDiagonal;
	float CurrentPlayAreaDiagonalRatio;

	float CalculateXYDiagonal(FVector Extent);

	void UpdatePlayArea();
};
