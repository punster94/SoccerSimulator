#pragma once

#include "PlayerBase.h"
#include "StateMachine.h"
#include "GoalKeeper.generated.h"

UCLASS()
class SOCCERSIMULATOR_API AGoalKeeper : public APlayerBase
{
	GENERATED_BODY()

public:

	AGoalKeeper();

	virtual ~AGoalKeeper();

	virtual void Tick(float DeltaTime) override;

	virtual void SetTeam(class ASoccerTeam* TeamReference) override;

	StateMachine<AGoalKeeper>& GetStateMachine();

	void SetRearInterposeTargetPosition();

	bool BallWithinRangeForIntercept();

	bool TooFarFromGoalMouth();

	bool OutsideInterceptionRange();

	UPROPERTY(EditAnywhere)
	float InterceptRange;

	UPROPERTY(EditAnywhere)
	float MinPassDistance;

	UPROPERTY(EditAnywhere)
	float MaxInterceptRange;

	UPROPERTY(EditAnywhere)
	float KeeperRadius;

private:

	FVector LookAt;

	StateMachine<AGoalKeeper>* KeeperStateMachine;

	USphereComponent* InterposeTarget;
};
